#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <string.h>

#define STEP_INTERVAL 0.5f
#define CELL_SIZE 100
#define BACKGROUND_COLOR 0xFF181818
#define COLS 4
#define ROWS 4

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int i32;
typedef float f32;

typedef enum {
  DIR_RIGHT = 0,
  DIR_UP,
  DIR_LEFT,
  DIR_DOWN,
  COUNT_DIRS,
} Dir;

static Dir dir_opposite(Dir dir) {
  assert(0 <= dir && dir < COUNT_DIRS && "Invalid direction");
  return (dir + 2) % COUNT_DIRS;
}

typedef struct {
  i32 x, y;
} Cell;

#define BOARD_CAP (ROWS * COLS)
typedef struct {
  u32 cells[BOARD_CAP];
} Board;

typedef enum {
  STATE_GAMEPLAY = 0,
  STATE_GAMEOVER
} State;

#define MAX_SPAWN_COUNT 2
typedef struct {
  Cell cells[MAX_SPAWN_COUNT];
  size_t count;
} NewCells;

typedef struct {
  u32 width;
  u32 height;

  Board board;
  State state;
  NewCells new_cells;

  f32 step_cooldown;
} Game;

static Game game;

static Rectangle cell_rect(Cell cell) {
  Rectangle rect;
  rect.x = cell.x * CELL_SIZE;
  rect.y = cell.y * CELL_SIZE;
  rect.width = CELL_SIZE;
  rect.height = CELL_SIZE;
  return rect;
}

static bool cell_eq(Cell a, Cell b) {
  return a.x == b.x && a.y == b.y;
}

static Cell cell_add(Cell a, Cell b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

static Cell dir_cell_data[COUNT_DIRS] = {
    [DIR_LEFT] = {.x = -1},
    [DIR_RIGHT] = {.x = 1},
    [DIR_UP] = {.y = -1},
    [DIR_DOWN] = {.y = 1},
};

#define dir_cell(dir) (assert((u32)dir < COUNT_DIRS && "Invalid direction"), dir_cell_data[dir])

static Cell step_cell(Cell head, Dir dir) {
  return cell_add(head, dir_cell(dir));
}

void game_init(u32 width, u32 height) {
  memset(&game, 0, sizeof(game));
  game.width = width;
  game.height = height;
}

void game_update(f32 dt) {
  game.step_cooldown -= dt;

  if (game.step_cooldown <= 0.0f) {
  }

  return;
}
