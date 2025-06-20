#include "chess.h"
#include <stdio.h>

void dump_with_movement_info(Game *game, const char *pos_str)
{
    Pos pos = pos_from(pos_str);
    printf("Available moves for ");
    pos_dump(pos);
    printf(" are:\n");
    game_find_valid_moves(game, pos);
    for(size_t i = 0; i < game->valid_move_list.count; ++i) {
        move_dump(game->valid_move_list.items[i]);
        putchar('\n');
    }
    game_dump(game);
}

int main(void)
{
    Game game = {0};
    game_init(&game);
    game_set_board_with_basic_start_pos(&game);
    game_dump(&game);

    // Pos pawn_pos = pos_from("b2");
    // for(; pawn_pos.col < 8; ++pawn_pos.col) {
    //     Pos target = pawn_pos;
    //     target.row = 5;
    //     game_do_move(&game, (Move){ .from = pawn_pos, .to = target });
    // }

    game_do_move(&game, (Move){ .from = pos_from("a1"), .to = pos_from("a3") });
    dump_with_movement_info(&game, "a3");
    dump_with_movement_info(&game, "c1");
    dump_with_movement_info(&game, "b7");
}
