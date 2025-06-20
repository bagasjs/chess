#include <chess.h>

// external function
void putcell(int codepoint, int row, int col);

uint8_t MEMORY[32 * 1024] = {0};
size_t allocated = 0;

void *platform_heap_alloc(size_t size)
{
    return &MEMORY[size + 1];
}

void platform_heap_free(void *ptr)
{
}

void draw_board(Game *game)
{
    for(int8_t row = 7; row >= 0; --row) {
        for(int8_t col = 0; col < 8; ++col) {
            Cell cell = game_board_get(game, POS(row, col));
            putcell(cell_repr(cell), row, col);
        }
    }
}

Game game = {0};
bool has_pick = false;
Pos pick = POS(-1, -1);

void handle_cell_click_event(int row, int col)
{
    Pos pos = POS(row, col);

    if(pick.row == row && pick.col == col) {
        has_pick = false;
        return;
    } 

    if(has_pick) {
        platform_print_text("Clicked at ");
        pos_dump(pos);
        platform_putchar('\n');

        Cell src_cell = game_board_get(&game, pick);
        Cell dst_cell = game_board_get(&game, pos);
        if(dst_cell != CELL_EMPTY && cell_piece_kind(src_cell) == cell_piece_kind(dst_cell)) {
            pick = pos;
            return;
        }

        if(game_find_valid_moves(&game, pick) == ERROR_NONE) {
            Move move = {0};
            for(size_t i = 0; i < game.valid_move_list.count; ++i) {
                move = game.valid_move_list.items[i];
                move_dump(move);
                platform_putchar('\n');
                if(move.to.row == row && move.to.col == col) {
                    platform_putchar('\n');
                    game_do_move(&game, move);
                    draw_board(&game);
                    has_pick = false;
                    return;
                }
            }
        }
    } else {
        if(game_board_get(&game, pos) == CELL_EMPTY) {
            platform_print_text("Picking empty cell?\n");
            return;
        }
        pick.row = row;
        pick.col = col;
        has_pick = true;
        platform_print_text("Picking ");
        pos_dump(pos);
        platform_putchar('\n');
    }
}

void _start()
{
    game_init(&game);
    game_set_board_with_basic_start_pos(&game);
    platform_print_text("Hello, World\n");
    draw_board(&game);
}
