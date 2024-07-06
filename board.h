#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

#define EMPTY_CELL 0
#define BOARD_WIDTH 800.0
#define BOARD_HEIGHT 800.0
#define BOARD_ROWS 4
#define BOARD_COLS 4
#define CELL_GAP_SIZE 22
#define CELL_WIDTH                                                             \
  ((BOARD_WIDTH - (CELL_GAP_SIZE * (BOARD_COLS + 1))) / BOARD_COLS)
#define CELL_HEIGHT                                                            \
  ((BOARD_HEIGHT - (CELL_GAP_SIZE * (BOARD_ROWS + 1))) / BOARD_ROWS)
#define MOVE_ANIMATION_DURATION 0.15
#define APPEAR_ANIMATION_DURATION 0.2
#define ANIMATION_DURATION (MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION)
#define SCALE_UP_DURATION (APPEAR_ANIMATION_DURATION / 2)
#define SCALE_DOWN_DURATION (APPEAR_ANIMATION_DURATION - SCALE_UP_DURATION)

typedef struct {
  int x;
  int y;
} Point;

typedef enum { CELL_EMPTY, CELL_FULL } CellType;

typedef int Cell;

typedef enum {
  MOVE,
  MOVE_REPLACE,
} ActionType;

typedef struct {
  Vector2 from;
  Vector2 to;
  int number;
  bool is_merge;
} Move;

typedef struct {
  Move *items;
  size_t count;
  size_t capacity;
} Moves;

typedef struct {
  Vector2 in;
  int number;
} Appear;

typedef struct {
  Appear *items;
  size_t count;
  size_t capacity;
} Appears;

typedef struct {
  Vector2 in;
  int number;
} Merge;

typedef struct {
  Merge *items;
  size_t count;
  size_t capacity;
} Merges;

typedef struct {
  bool is_animation_playing;
  float elapsed_time;
  Merges merges;
  Moves moves;
  Appears appears;
} Animation;

typedef struct {
  Cell cells[BOARD_ROWS][BOARD_COLS];
  Animation animation;
} Board;

void InitBoard(Board *board);
void UpdateBoard(Board *board);
void DrawBoard(Board *board);

#endif // BOARD_H
