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
Pos pick = POS(0, 0);

void handle_cell_click_event(int row, int col)
{
    if(pick.row == row && pick.col == col) {
        has_pick = false;
    } else {
        if(has_pick) {
            if(game_find_valid_moves(&game, pick) != ERROR_NONE) {
                Move move = {0};
                for(size_t i = 0; i < game.valid_move_list.count; ++i) {
                    move = game.valid_move_list.items[i];
                    if(move.to.row != row || move.to.col != col) {
                        platform_print_text("Failed to move ");
                        platform_putchar(cell_repr(move.piece));
                        platform_print_text(" from ");
                        platform_print_int(pick.row);
                        platform_print_int(pick.col);
                        platform_print_text(" to t");
                        platform_print_int(row);
                        platform_print_int(col);
                        platform_putchar('\n');
                    }
                }
                platform_print_text("Something went wrong while looking for a valid move");
                game_do_move(&game, 
                draw_board(&game);
            }
            has_pick = false;
        } else {
            pick.row = row;
            pick.col = col;
            has_pick = true;
        }
    }
}

void _start()
{
    game_init(&game);
    game_set_board_with_basic_start_pos(&game);

    platform_print_text("Hello, World\n");
    draw_board(&game);
}
