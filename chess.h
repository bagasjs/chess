#ifndef CHESS_H_
#define CHESS_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    ERROR_NONE = 0,
    ERROR_EMPTY_CELL,
    ERROR_INVALID_POS,
} Error;

typedef enum {
    PIECE_INVALID = 0,
    PIECE_WHITE,
    PIECE_BLACK,
} PieceKind;

typedef enum {
    CELL_EMPTY = 0,
    CELL_W_PAWN,
    CELL_W_KNIGHT,
    CELL_W_BISHOP,
    CELL_W_ROOK,
    CELL_W_QUEEN,
    CELL_W_KING,
    CELL_B_PAWN,
    CELL_B_KNIGHT,
    CELL_B_BISHOP,
    CELL_B_ROOK,
    CELL_B_QUEEN,
    CELL_B_KING,
} Cell;

char cell_repr(Cell cell);
PieceKind cell_piece_kind(Cell cell);

typedef struct {
    int8_t row;
    int8_t col;
} Pos;
#define POS(row, col) ((Pos){ (row), (col) })
#define IS_VALID_POS(pos) ((0 <= (pos).row && (pos).row < 8) && (0 <= (pos).col && (pos).col < 8))

Pos pos_from(const char *pos);
void pos_dump(Pos pos);

typedef struct {
    Pos from;
    Pos to;

    Cell piece;
    Cell take;
    bool promotion;
    bool castling;
    bool en_passant;
    bool check;
    bool mate;
} Move;
#define MOVE(from, to) ((Move){ (from), (to) })
void move_dump(Move move);

typedef struct {
    Move *items;
    size_t count;
    size_t capacity;
} MoveList;
void move_list_push(MoveList *list, Move move);

typedef struct {
    Cell board[8 * 8];
    MoveList history;
    // For castling
    bool white_king_moved;
    bool white_kingside_rook_moved;
    bool white_queenside_rook_moved;
    bool black_king_moved;
    bool black_kingside_rook_moved;
    bool black_queenside_rook_moved;

    // Non-serialized state
    bool white_king_check;
    bool black_king_check;
} Game;

void game_init(Game *game);
Cell game_board_get(Game *game, Pos pos);
void game_board_set(Game *game, Pos pos, Cell cell);
void game_set_board_with_basic_start_pos(Game *game);
void game_dump(Game *game);
void game_do_move(Game *game, Move move);
Error game_find_valid_moves(Game *game, Pos pos, MoveList *moves);

#endif // CHESS_H_
