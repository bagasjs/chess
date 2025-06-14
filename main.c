#include "chess.h"
#include <stdio.h>

int main(void)
{
    Game game = {0};
    MoveList available_moves = {0};
    game_init(&game);
    game_set_board_with_basic_start_pos(&game);

    Pos pos = pos_from("g1");

    game_dump(&game);

    game_do_move(&game, (Move){ .from = pos_from("e2"), .to = pos_from("e4") });
    printf("Available moves for ");
    pos_dump(pos);
    printf(" are:\n");
    game_find_valid_moves(&game, pos, &available_moves);
    for(size_t i = 0; i < available_moves.count; ++i) {
        move_dump(available_moves.items[i]);
        putchar('\n');
    }
    game_dump(&game);

    game_do_move(&game, (Move){ .from = pos_from("e7"), .to = pos_from("e5") });
    printf("Available moves for ");
    pos_dump(pos);
    printf(" are:\n");
    game_find_valid_moves(&game, pos, &available_moves);
    for(size_t i = 0; i < available_moves.count; ++i) {
        move_dump(available_moves.items[i]);
        putchar('\n');
    }
    game_dump(&game);
}
