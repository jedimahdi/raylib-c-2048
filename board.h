#ifndef BOARD_H_
#define BOARD_H_

#define BOARD_SIZE 4
#define MAX_TILES (BOARD_SIZE * BOARD_SIZE)
#define MAX_MERGES ((int)(MAX_TILES / 2))
#define EMPTY_TILE_VALUE 0
#define BOARD_WIDTH 800.0
#define BOARD_HEIGHT 800.0
#define TILE_GAP_SIZE 22
#define TILE_WIDTH                                                             \
  ((BOARD_WIDTH - (TILE_GAP_SIZE * (BOARD_SIZE + 1))) / BOARD_SIZE)
#define TILE_HEIGHT                                                            \
  ((BOARD_HEIGHT - (TILE_GAP_SIZE * (BOARD_SIZE + 1))) / BOARD_SIZE)

#include "linear.h"
#include <stdbool.h>

typedef enum {
  TILE_IDLE,
  TILE_MOVE,
  TILE_MERGE,
  TILE_APPEAR,
} TileType;

typedef struct {
  TileType type;
  V2f pos;
  V2f from_pos;
  V2f to_pos;
  int number;
  int new_number;
  float scale;
} Tile;

typedef struct {
  int tiles_map[BOARD_SIZE][BOARD_SIZE];
  Tile tiles[MAX_TILES + MAX_MERGES];
  int tiles_count;
} Board;

bool is_tile_inbounds(V2i tile);
V2i get_starting_tile(V2i direction);
bool get_next_tile(V2i *tile, V2i direction);

Tile make_idle_tile(int value, V2f pos);
Tile make_appear_tile(int value, V2f pos);
Tile make_move_tile(int value, V2f from_pos, V2f to_pos);
Tile make_merge_tile(int value, int new_value, V2f from_pos, V2f to_pos);

V2f get_tile_position(V2i tile);
int get_tile_value(Board *board, V2i tile);
void set_tile_value(Board *board, V2i tile, int value);

bool add_random_tile(Board *board);
void move_tiles(Board *board, V2i direction);

void draw_tile(Tile tile);

#endif // BOARD_H_
