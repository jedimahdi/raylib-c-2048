#ifndef BOARD_H
#define BOARD_H

#include "animation.h"
#include <stdbool.h>

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

typedef struct {
  int x;
  int y;
} Point;

typedef enum { CELL_EMPTY, CELL_FULL } CellType;

typedef int Cell;


typedef struct {
  Cell cells[BOARD_ROWS][BOARD_COLS];
  Animation animation;
} Board;

void InitBoard(Board *board);
void UpdateBoard(Board *board);
void DrawBoard(Board *board);

#endif // BOARD_H
