/**
 * @param {ArrayBuffer} mem
 * @param {number} ptr
 * @return {number}
 */
function cstrlen(mem, ptr) {
    let len = 0;
    while (mem[ptr] != 0) {
        len++;
        ptr++;
    }
    return len;
}

/**
 * @param {ArrayBuffer} buf
 * @param {number} ptr
 * @return {string}
 */
function stringFromPtr(buf, ptr) {
    const mem = new Uint8Array(buf);
    const len = cstrlen(mem, ptr);
    const bytes = new Uint8Array(buf, ptr, len);
    return new TextDecoder().decode(bytes);
}

const putcell = (codepoint, row, col) => {
    const result = Array.from(document.querySelectorAll("#game .row .col"));
    const index = (7 - row) * 8 + col;
    if(index >= 64) throw Error(`Index error at putcell ${index} => ${row}, ${col}`);
    if(codepoint === 0 || codepoint === 32 || codepoint == 46) {
        // If codepoint == '\0' || codepoint == ' ' || codepoint == '.'
        result[index].innerHTML = "";
    } else {
        const display = String.fromCodePoint(codepoint);
        const displayToImageFile = {
            "P": "WP.png",
            "N": "WN.png",
            "B": "WB.png",
            "R": "WR.png",
            "Q": "WQ.png",
            "K": "WK.png",
            "p": "BP.png",
            "n": "BN.png",
            "b": "BB.png",
            "r": "BR.png",
            "q": "BQ.png",
            "k": "BK.png",
        }
        result[index].innerHTML = `<img class="piece" data-row="${row}" data-col="${col}" src="./assets/${displayToImageFile[display]}"/>`;
    }
}

(async () => {
    const term = document.getElementById("term");
    const platform_print_text = (pText)  =>  {
        const buffer = wasm.instance.exports.memory.buffer;
        const display = stringFromPtr(buffer, pText);
        term.innerHTML += display.replace("\n", "<br>");
    }
    const platform_print_int = value => {
        term.innerHTML += `${value}`
    }
    const platform_putchar = ch => {
        if(ch === 10) {
            term.innerHTML += "<br>";
            return;
        }
        term.innerHTML += String.fromCharCode(ch)
    }


    const wasm = await WebAssembly.instantiateStreaming(fetch("index.wasm"), {
        "env": {
            putcell,
            platform_print_text,
            platform_print_int,
            platform_putchar,
        }
    });

    wasm.instance.exports._start();

    if(wasm.instance.exports.handle_cell_click_event) {
        document.getElementById("game").addEventListener("click", e => {
        });
        document.addEventListener("click", e => {
            const cell = e.target.closest(".col");
            const posStr = cell?.dataset.pos;
            console.log(posStr);
            const ccp = posStr.codePointAt(0);
            const rcp = posStr.codePointAt(1);
            console.assert(!(48 <= ccp && ccp <= 55), `ccp = ${ccp}`);
            console.assert(!(97 <= rcp && rcp <= 103), `rcp = ${rcp}`);
            wasm.instance.exports.handle_cell_click_event(rcp - 49, ccp - 97);
        });
        // document.querySelectorAll("#game .row .col").forEach(dom => {
        //     dom.addEventListener("click", e => {
        //         const posStr = e.target.dataset.pos;
        //         console.log(posStr);
        //         const ccp = posStr.codePointAt(0);
        //         const rcp = posStr.codePointAt(1);
        //         console.assert(!(48 <= ccp && ccp <= 55), `ccp = ${ccp}`);
        //         console.assert(!(97 <= rcp && rcp <= 103), `rcp = ${rcp}`);
        //         wasm.instance.exports.handle_cell_click_event(rcp - 49, ccp - 97);
        //     });
        // });
    }

    if(wasm.instance.exports._frame) {
        console.log("Per frame function is defined. Starting the frame mainloop");
        let prev = null;
        const webAnimationFrameLoop = timestamp => {
            if(prev) {
                wasm.instance.exports._frame(timestamp);
            }
            prev = timestamp;
            window.requestAnimationFrame(webAnimationFrameLoop)
        }
        window.requestAnimationFrame(webAnimationFrameLoop);
    }
})();
