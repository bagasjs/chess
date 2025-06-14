#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chess.h"

char cell_repr(Cell cell)
{
    static char _cell_repr[] = {
        [CELL_EMPTY]    = '.',
        [CELL_W_PAWN]   = 'P',
        [CELL_W_KNIGHT] = 'N',
        [CELL_W_BISHOP] = 'B',
        [CELL_W_ROOK]   = 'R',
        [CELL_W_QUEEN]  = 'Q',
        [CELL_W_KING]   = 'K',
        [CELL_B_PAWN]   = 'P',
        [CELL_B_KNIGHT] = 'N',
        [CELL_B_BISHOP] = 'B',
        [CELL_B_ROOK]   = 'R',
        [CELL_B_QUEEN]  = 'Q',
        [CELL_B_KING]   = 'K',
    };
    return _cell_repr[cell];
}

PieceKind cell_piece_kind(Cell cell)
{
    static bool _is_black_piece[] = {
        [CELL_EMPTY]    = PIECE_INVALID,
        [CELL_W_PAWN]   = PIECE_WHITE,
        [CELL_W_KNIGHT] = PIECE_WHITE,
        [CELL_W_BISHOP] = PIECE_WHITE,
        [CELL_W_ROOK]   = PIECE_WHITE,
        [CELL_W_QUEEN]  = PIECE_WHITE,
        [CELL_W_KING]   = PIECE_WHITE,
        [CELL_B_PAWN]   = PIECE_BLACK,
        [CELL_B_KNIGHT] = PIECE_BLACK,
        [CELL_B_BISHOP] = PIECE_BLACK,
        [CELL_B_ROOK]   = PIECE_BLACK,
        [CELL_B_QUEEN]  = PIECE_BLACK,
        [CELL_B_KING]   = PIECE_BLACK,
    };
    return _is_black_piece[cell];
}

Pos pos_from(const char *pos)
{
    char col_char = pos[0];
    char row_char = pos[1];
    assert('1' <= row_char && row_char <= '8');
    assert('a' <= col_char && col_char <= 'h');
    return (Pos) { .row = row_char - '1', .col = col_char - 'a', };
}

void pos_dump(Pos pos)
{
    putchar(pos.col + 'a');
    putchar(pos.row + '1');
}

void move_dump(Move move)
{
    putchar(cell_repr(move.piece));
    pos_dump(move.from);
    if(move.take != CELL_EMPTY) putchar('x');
    pos_dump(move.to);
    if(move.promote != CELL_EMPTY) putchar(cell_repr(move.promote));
    if(move.check) putchar(cell_repr('+'));
    if(move.check && move.mate) putchar(cell_repr('#'));
}

void move_list_push(MoveList *list, Move move)
{
    if(list->count + 1 > list->capacity) { 
        size_t item_size = sizeof(*list->items); 
        size_t new_capacity = list->capacity * 2; 
        if(new_capacity == 0) new_capacity = 32; 
        void *new_items = malloc(new_capacity * item_size); 
        assert(new_items != NULL && "Buy More RAM LOL!"); 
        memcpy(new_items, list->items, list->count*item_size); 
        free(list->items); 
        list->items = new_items; 
        list->capacity = new_capacity; 
    } 
    list->items[list->count++] = move; 
}

void game_init(Game *game)
{
    assert(game && "game_init: Invalid game instance");
    game->history.count = 0;
    game->history.items = 0;
    game->valid_move_list.count = 0;
    game->valid_move_list.items = 0;
    memset(game->board, 0, sizeof(game->board));
}

Cell game_board_get(Game *game, Pos pos)
{
    assert(game && "game_board_get: Invalid game instance");
    assert(0 <= pos.row && pos.row < 8);
    assert(0 <= pos.col && pos.col < 8);
    return game->board[pos.row * 8 + pos.col];
}

void game_board_set(Game *game, Pos pos, Cell cell)
{
    assert(game && "game_board_set: Invalid game instance");
    assert(0 <= pos.row && pos.row < 8);
    assert(0 <= pos.col && pos.col < 8);
    game->board[pos.row * 8 + pos.col] = cell;
}

void game_set_board_with_basic_start_pos(Game *game)
{
    assert(game && "game_set_board_with_basic_start_pos: Invalid game instance");
    memset(game->board, 0, sizeof(game->board));
    game_board_set(game, POS(0, 0), CELL_W_ROOK);
    game_board_set(game, POS(0, 1), CELL_W_KNIGHT);
    game_board_set(game, POS(0, 2), CELL_W_BISHOP);
    game_board_set(game, POS(0, 3), CELL_W_QUEEN);
    game_board_set(game, POS(0, 4), CELL_W_KING);
    game_board_set(game, POS(0, 5), CELL_W_BISHOP);
    game_board_set(game, POS(0, 6), CELL_W_KNIGHT);
    game_board_set(game, POS(0, 7), CELL_W_ROOK);
    game_board_set(game, POS(1, 0), CELL_W_PAWN);
    game_board_set(game, POS(1, 1), CELL_W_PAWN);
    game_board_set(game, POS(1, 2), CELL_W_PAWN);
    game_board_set(game, POS(1, 3), CELL_W_PAWN);
    game_board_set(game, POS(1, 4), CELL_W_PAWN);
    game_board_set(game, POS(1, 5), CELL_W_PAWN);
    game_board_set(game, POS(1, 6), CELL_W_PAWN);
    game_board_set(game, POS(1, 7), CELL_W_PAWN);
    game_board_set(game, POS(6, 0), CELL_B_PAWN);
    game_board_set(game, POS(6, 1), CELL_B_PAWN);
    game_board_set(game, POS(6, 2), CELL_B_PAWN);
    game_board_set(game, POS(6, 3), CELL_B_PAWN);
    game_board_set(game, POS(6, 4), CELL_B_PAWN);
    game_board_set(game, POS(6, 5), CELL_B_PAWN);
    game_board_set(game, POS(6, 6), CELL_B_PAWN);
    game_board_set(game, POS(6, 7), CELL_B_PAWN);
    game_board_set(game, POS(7, 0), CELL_B_ROOK);
    game_board_set(game, POS(7, 1), CELL_B_KNIGHT);
    game_board_set(game, POS(7, 2), CELL_B_BISHOP);
    game_board_set(game, POS(7, 3), CELL_B_QUEEN);
    game_board_set(game, POS(7, 4), CELL_B_KING);
    game_board_set(game, POS(7, 5), CELL_B_BISHOP);
    game_board_set(game, POS(7, 6), CELL_B_KNIGHT);
    game_board_set(game, POS(7, 7), CELL_B_ROOK);
}

void game_dump(Game *game)
{
    assert(game && "game_dump: Invalid game instance");
    for(int8_t row = 7; row >= 0; --row) {
        printf("%d ", row + 1);
        for(int8_t col = 0; col < 8; ++col) {
            if(col > 0) putchar(' ');
            Cell cell = game_board_get(game, POS(row, col));
            putchar(cell_repr(cell));
        }
        putchar('\n');
    }
    printf("  A B C D E F G H\n\n");
}

void game_do_move(Game *game, Move move)
{
    assert(game  && "game_dump: Invalid game instance");
    Cell from = game_board_get(game, move.from);
    Cell to = game_board_get(game, move.to);
    game_board_set(game, move.from, CELL_EMPTY);
    game_board_set(game, move.to, from);
}

static Error _game_find_valid_moves_for_king(Game *game, Cell cell, Pos pos)
{
    for(int8_t i = -1; i <= 1; ++i) {
        for(int8_t j = -1; j <= 1; ++j) {
            Move move = {0};
            move.piece = cell;
            move.from = pos;
            move.to.row = pos.row + i;
            move.to.col = pos.col + j;
            if(!IS_VALID_POS(move.to)) continue;

            Cell dst_cell = game_board_get(game, move.to);
            if(dst_cell != CELL_EMPTY) {
                if(cell_piece_kind(cell) != cell_piece_kind(dst_cell)) {
                    move.take = dst_cell;
                } else {
                    // Invalid move
                    continue;
                }
            }
            move_list_push(&game->valid_move_list, move);
        }
    }
    return ERROR_NONE;
}

static Error _game_find_valid_moves_for_knight(Game *game, Cell cell, Pos pos)
{
    static const int8_t knight_moves[8][2] = {
        { -2, -1 }, { -2,  1 },
        { -1, -2 }, { -1,  2 },
        {  1, -2 }, {  1,  2 },
        {  2, -1 }, {  2,  1 },
    };

    for(int i = 0; i < 8; ++i) {
        int8_t dr = knight_moves[i][0];
        int8_t dc = knight_moves[i][1];
        Pos to = { pos.row + dr, pos.col + dc };
        if (!IS_VALID_POS(to)) continue;

        Cell dst = game_board_get(game, to);
        if (dst != CELL_EMPTY && cell_piece_kind(dst) == cell_piece_kind(cell)) continue;

        Move move = {0};
        move.piece = cell;
        move.from = pos;
        move.to = to;
        if (dst != CELL_EMPTY) move.take = dst;
        move_list_push(&game->valid_move_list, move);
    }

    return ERROR_NONE;
}

static Error _game_find_valid_moves_for_rook(Game *game, Cell cell, Pos pos)
{
    // Check to right
    for(int8_t i = 1; i < 8; ++i) {
        Move move = {0};
        move.piece = cell;
        move.from = pos;
        move.to = POS(pos.row, pos.col + i);
        if(!IS_VALID_POS(move.to)) break;
        Cell dst_cell = game_board_get(game, move.to);
        if(dst_cell != CELL_EMPTY) {
            if(cell_piece_kind(cell) != cell_piece_kind(dst_cell)) {
                move.take = dst_cell;
                move_list_push(&game->valid_move_list, move);
            }
            break;
        }
        move_list_push(&game->valid_move_list, move);
    }

    // Check to left
    for(int8_t i = 1; i < 8; ++i) {
        Move move = {0};
        move.piece = cell;
        move.from = pos;
        move.to = POS(pos.row, pos.col - i);
        if(!IS_VALID_POS(move.to)) break;
        Cell dst_cell = game_board_get(game, move.to);
        if(dst_cell != CELL_EMPTY) {
            if(cell_piece_kind(cell) != cell_piece_kind(dst_cell)) {
                move.take = dst_cell;
                move_list_push(&game->valid_move_list, move);
            }
            break;
        }
        move_list_push(&game->valid_move_list, move);
    }

    // Check to bottom
    for(int8_t i = 1; i < 8; ++i) {
        Move move = {0};
        move.piece = cell;
        move.from = pos;
        move.to = POS(pos.row + i, pos.col);
        if(!IS_VALID_POS(move.to)) break;
        Cell dst_cell = game_board_get(game, move.to);
        if(dst_cell != CELL_EMPTY) {
            if(cell_piece_kind(cell) != cell_piece_kind(dst_cell)) {
                move.take = dst_cell;
                move_list_push(&game->valid_move_list, move);
            }
            break;
        }
        move_list_push(&game->valid_move_list, move);
    }

    // Check to top
    for(int8_t i = 1; i < 8; ++i) {
        Move move = {0};
        move.piece = cell;
        move.from = pos;
        move.to = POS(pos.row - i, pos.col);
        if(!IS_VALID_POS(move.to)) break;
        Cell dst_cell = game_board_get(game, move.to);
        if(dst_cell != CELL_EMPTY) {
            if(cell_piece_kind(cell) != cell_piece_kind(dst_cell)) {
                move.take = dst_cell;
                move_list_push(&game->valid_move_list, move);
            }
            break;
        }
        move_list_push(&game->valid_move_list, move);
    }

    return ERROR_NONE;
}

static Error _game_find_valid_moves_for_pawn(Game *game, Cell cell, Pos pos)
{
    int8_t y_start = cell_piece_kind(cell) == PIECE_WHITE ? 1 : 6;
    int8_t y_dir = cell_piece_kind(cell) == PIECE_WHITE ? +1 : -1;
    int8_t y_max_pos = cell_piece_kind(cell) == PIECE_WHITE ? 7 : 0;
    Cell default_promotion = cell_piece_kind(cell) == PIECE_WHITE ? CELL_W_QUEEN : CELL_B_QUEEN;

    // Forward
    {
        Pos  dst_pos  = POS(pos.row + y_dir, pos.col);
        Cell dst_cell = game_board_get(game, dst_pos);
        if(dst_cell == CELL_EMPTY) {
            Move move = {0};
            move.piece = cell;
            move.from = pos;
            move.to = dst_pos;
            if(move.to.row == y_max_pos) move.promote = default_promotion;
            move_list_push(&game->valid_move_list, move);

            if(pos.row == y_start) {
                Pos  dst_pos  = POS(pos.row + 2 * y_dir, pos.col);
                Cell dst_cell = game_board_get(game, dst_pos);
                if(IS_VALID_POS(dst_pos) && dst_cell == CELL_EMPTY) {
                    Move move = {0};
                    move.piece = cell;
                    move.from = pos;
                    move.to = dst_pos;
                    if(move.to.row == y_max_pos) move.promote = default_promotion;
                    move_list_push(&game->valid_move_list, move);
                }
            }
        }
    }
    {
        Pos dst_pos = POS(pos.row + y_dir, pos.col - 1);
        Cell dst_cell = game_board_get(game, dst_pos);
        if(dst_cell != CELL_EMPTY && cell_piece_kind(dst_cell) != cell_piece_kind(cell)) {
            Move move = {0};
            move.piece = cell;
            move.from = pos;
            move.to = dst_pos;
            move.take = dst_cell;
            if(move.to.row == y_max_pos) move.promote = default_promotion;
            move_list_push(&game->valid_move_list, move);
        }
    }
    {
        Pos dst_pos = POS(pos.row + y_dir, pos.col + 1);
        Cell dst_cell = game_board_get(game, dst_pos);
        if(dst_cell != CELL_EMPTY && cell_piece_kind(dst_cell) != cell_piece_kind(cell)) {
            Move move = {0};
            move.piece = cell;
            move.from = pos;
            move.to = dst_pos;
            move.take = dst_cell;
            if(move.to.row == y_max_pos) move.promote = default_promotion;
            move_list_push(&game->valid_move_list, move);
        }
    }
    return ERROR_NONE;
}

static Error _game_find_valid_moves_for_bishop(Game *game, Cell cell, Pos pos)
{
    int8_t dirs[4][2] = {
        {1, 1},   // ↘ bottom-right
        {1, -1},  // ↙ bottom-left
        {-1, 1},  // ↗ top-right
        {-1, -1}  // ↖ top-left
    };

    for(int d = 0; d < 4; ++d) {
        for(int i = 1; i < 8; ++i) {
            Move move = {0};
            move.piece = cell;
            move.from = pos;
            move.to = POS(pos.row + i * dirs[d][0], pos.col + i * dirs[d][1]);
            if (!IS_VALID_POS(move.to)) break;

            Cell dst_cell = game_board_get(game, move.to);
            if (dst_cell != CELL_EMPTY) {
                if (cell_piece_kind(cell) == cell_piece_kind(dst_cell)) break;
                move.take = dst_cell;
                move_list_push(&game->valid_move_list, move);
                break;
            }

            move_list_push(&game->valid_move_list, move);
        }
    }

    return ERROR_NONE;
}

Error game_find_valid_moves(Game *game, Pos pos)
{
    assert(game  && "game_dump: Invalid game instance");
    game->valid_move_list.count = 0;

    Cell cell = game_board_get(game, pos);
    if(cell == CELL_EMPTY) return ERROR_EMPTY_CELL;

    if(cell == CELL_W_KNIGHT || cell == CELL_B_KNIGHT) 
        return _game_find_valid_moves_for_knight(game, cell, pos);
    if(cell == CELL_W_ROOK || cell == CELL_B_ROOK)
        return _game_find_valid_moves_for_rook(game, cell, pos);
    if(cell == CELL_W_BISHOP || cell == CELL_B_BISHOP)
        return _game_find_valid_moves_for_bishop(game, cell, pos);
    if(cell == CELL_W_QUEEN || cell == CELL_B_QUEEN) {
        Error err = _game_find_valid_moves_for_rook(game, cell, pos);
        if(err != ERROR_NONE) return err;
        return _game_find_valid_moves_for_bishop(game, cell, pos);
    }
    if(cell == CELL_W_KING || cell == CELL_B_KING) 
        return _game_find_valid_moves_for_king(game, cell, pos);

    return ERROR_NONE;
}
