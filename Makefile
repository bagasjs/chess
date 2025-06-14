CC := clang
CFLAGS := -Wall -Wextra

WASM_CFLAGS := --target=wasm32 --no-standard-libraries -DCHESS_WASM
WASM_LFLAGS := -Wl,--allow-undefined -Wl,--export-all -Wl,--no-entry

all: main.exe index.wasm

index.wasm: ./index.c ./chess.c
	$(CC) $(CFLAGS) $(WASM_CFLAGS) -o $@ $^ $(WASM_LFLAGS)

main.exe: ./main.c ./chess.c
	$(CC) $(CFLAGS) -o $@ $^
