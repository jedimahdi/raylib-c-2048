#include "board.h"
#include "array.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void MoveLeft(Board *board);
static void MoveRight(Board *board);
static void MoveUp(Board *board);
static void MoveDown(Board *board);
static Rectangle GetCellRect(int row, int col);
static void DrawEmptyBoard(void);
static void UpdateAnimation(Board *board);
static void ClearAnimations(Animation *animation);

static Color GetCellColor(int number) {
  switch (number) {
  case 2:
    return (Color){57, 42, 26, 255};
  case 4:
    return (Color){71, 54, 22, 255};
  case 8:
    return (Color){127, 65, 11, 255};
  case 16:
    return (Color){141, 54, 8, 255};
  case 32:
    return (Color){145, 33, 7, 255};
  case 64:
    return (Color){167, 37, 7, 255};
  case 128:
    return (Color){97, 77, 12, 255};
  case 256:
    return (Color){237, 197, 63, 255};
  case 512:
    return (Color){237, 200, 80, 255};
  case 1024:
    return (Color){237, 197, 63, 255};
  case 2048:
    return (Color){237, 194, 46, 255};
  case 4096:
    return (Color){237, 112, 46, 255};
  case 8192:
    return (Color){237, 76, 46, 255};
  default:
    return (Color){237, 76, 46, 255};
  };
}

static Rectangle GetCellRect(int row, int col) {
  return (Rectangle){.height = CELL_HEIGHT,
                     .width = CELL_WIDTH,
                     .x = col * CELL_WIDTH + ((col + 1) * CELL_GAP_SIZE),
                     .y = row * CELL_HEIGHT + ((row + 1) * CELL_GAP_SIZE)};
}

static Vector2 GetCellPosition(int row, int col) {
  return (Vector2){.x = col * CELL_WIDTH + ((col + 1) * CELL_GAP_SIZE),
                   .y = row * CELL_HEIGHT + ((row + 1) * CELL_GAP_SIZE)};
}

static bool IsCellEmpty(Cell cell) { return cell == 0; }

// static Cell MakeEmptyCell() { return (Cell){.type = CELL_EMPTY, .number = 0};
// }
//
// static Cell MakeCell(int number) {
//   return (Cell){.type = CELL_FULL, .number = number};
// }

static void DrawEmptyBoard(void) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      DrawRectangleRounded(GetCellRect(row, col), 0.05, 0,
                           GetColor(0x392A1A55));
    }
  }
}

static void DrawCellScaled(Cell cell, Vector2 position, float scale) {
  float height = CELL_HEIGHT * scale;
  float width = CELL_WIDTH * scale;
  Rectangle rect = {.height = height,
                    .width = width,
                    .x = position.x - ((width - CELL_WIDTH) / 2),
                    .y = position.y - ((height - CELL_HEIGHT) / 2)};
  DrawRectangleRounded(rect, 0.05, 0, GetCellColor(cell));
  char number_str[6];
  sprintf(number_str, "%d", cell);
  DrawText(number_str, position.x + (CELL_WIDTH / 2) - 12,
           position.y + (CELL_HEIGHT / 2) - 22, 52, LIGHTGRAY);
}

static void DrawCell(Cell cell, Rectangle cell_rect) {
  DrawCellScaled(cell, (Vector2){.x = cell_rect.x, .y = cell_rect.y}, 1);
}

static void DrawCells(Board *board) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Cell cell = board->cells[row][col];
      if (!IsCellEmpty(cell)) {
        Rectangle rect = GetCellRect(row, col);
        DrawCell(cell, rect);
      }
    }
  }
}

static void DrawAnimationCells(Board *board) {
  if (board->animation.elapsed_time <= MOVE_ANIMATION_DURATION) {
    for (int i = 0; i < board->animation.moves.count; i++) {
      Move move = board->animation.moves.items[i];
      Cell cell = move.number;
      Rectangle rect = (Rectangle){
          .height = CELL_HEIGHT,
          .width = CELL_WIDTH,
          .x = Lerp(move.from.x, move.to.x,
                    board->animation.elapsed_time / MOVE_ANIMATION_DURATION),
          .y = Lerp(move.from.y, move.to.y,
                    board->animation.elapsed_time / MOVE_ANIMATION_DURATION)};
      DrawCell(cell, rect);
    }
  } else {
    float elapsed_time =
        board->animation.elapsed_time - MOVE_ANIMATION_DURATION;
    for (int i = 0; i < board->animation.moves.count; i++) {
      Move move = board->animation.moves.items[i];
      if (!move.is_merge) {
        Rectangle rect = (Rectangle){
            .height = CELL_HEIGHT,
            .width = CELL_WIDTH,
            .x = move.to.x,
            .y = move.to.y,
        };
        Cell cell = move.number;
        DrawCell(cell, rect);
      }
    }

    for (int i = 0; i < board->animation.merges.count; i++) {
      Merge merge = board->animation.merges.items[i];

      if (elapsed_time < SCALE_UP_DURATION) {
        float scale = Lerp(1, 1.1, elapsed_time / SCALE_UP_DURATION);
        Cell cell = merge.number;
        DrawCellScaled(cell, merge.in, scale);
      } else {
        float scale = Lerp(
            1.1, 1, (elapsed_time - SCALE_UP_DURATION) / SCALE_DOWN_DURATION);
        Cell cell = merge.number;
        DrawCellScaled(cell, merge.in, scale);
      }
    }

    for (int i = 0; i < board->animation.appears.count; i++) {
      Appear appear = board->animation.appears.items[i];
      float scale = Lerp(0, 1, elapsed_time / APPEAR_ANIMATION_DURATION);
      Cell cell = appear.number;
      DrawCellScaled(cell, appear.in, scale);
    }
  }
}

void DrawBoard(Board *board) {
  DrawEmptyBoard();
  if (board->animation.is_animation_playing) {
    DrawAnimationCells(board);
  } else {
    DrawCells(board);
  }
}

void InitBoard(Board *board) {
  memset(board, 0, sizeof(*board));
  Cell cell1 = 2;
  Cell cell2 = 2;
  board->cells[1][1] = cell1;
  board->cells[3][2] = cell2;
}

void AddRandomCell(Board *board) {
  Point empty_cells[BOARD_COLS * BOARD_ROWS];
  size_t empty_cells_count = 0;

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Cell cell = board->cells[row][col];
      if (IsCellEmpty(cell)) {
        empty_cells[empty_cells_count] = (Point){.x = col, .y = row};
        empty_cells_count++;
      }
    }
  }
  if (empty_cells_count == 0) {
    return;
  }
  int r = rand() % empty_cells_count;
  Point choosen = empty_cells[r];
  board->cells[choosen.y][choosen.x] = 2;
  Rectangle rect = GetCellRect(choosen.y, choosen.x);
  Appear appear = {.number = 2, .in = {.x = rect.x, .y = rect.y}};
  da_append(&board->animation.appears, appear);
}

static bool AnyMoveHappen(Board *board) {
  for (size_t i = 0; i < board->animation.moves.count; i++) {
    Move move = board->animation.moves.items[i];
    if (move.from.x != move.to.x || move.from.y != move.to.y)
      return true;
  }
  return false;
}

void UpdateBoard(Board *board) {
  if (IsKeyPressed(KEY_A)) {
    ClearAnimations(&board->animation);
    MoveLeft(board);
    if (AnyMoveHappen(board)) {
      board->animation.is_animation_playing = true;
      AddRandomCell(board);
    }
  }
  if (IsKeyPressed(KEY_D)) {
    ClearAnimations(&board->animation);
    MoveRight(board);
    if (AnyMoveHappen(board)) {
      board->animation.is_animation_playing = true;
      AddRandomCell(board);
    }
  }
  if (IsKeyPressed(KEY_W)) {
    ClearAnimations(&board->animation);
    MoveUp(board);
    if (AnyMoveHappen(board)) {
      board->animation.is_animation_playing = true;
      AddRandomCell(board);
    }
  }
  if (IsKeyPressed(KEY_S)) {
    ClearAnimations(&board->animation);
    MoveDown(board);
    if (AnyMoveHappen(board)) {
      board->animation.is_animation_playing = true;
      AddRandomCell(board);
    }
  }

  if (board->animation.is_animation_playing) {
    UpdateAnimation(board);
  }
}

static void UpdateAnimation(Board *board) {
  float dt = GetFrameTime();
  board->animation.elapsed_time += dt;
  if (board->animation.elapsed_time > ANIMATION_DURATION) {
    board->animation.is_animation_playing = false;
  }
}

static void ClearAnimations(Animation *animation) {
  animation->elapsed_time = 0;
  animation->moves.count = 0;
  animation->appears.count = 0;
  animation->merges.count = 0;
}

static void AddMoveAnimation(Board *board, Cell cell, bool is_merge,
                             Vector2 from_pos, Vector2 to_pos) {
  Move move = (Move){
      .from = from_pos, .to = to_pos, .is_merge = is_merge, .number = cell};
  da_append(&board->animation.moves, move);
}

static void AddMergeAnimation(Board *board, Cell cell, Vector2 pos) {
  Merge merge = {.number = cell, .in = pos};
  da_append(&board->animation.merges, merge);
}

static int CalculateTargetColLeft(Board *board, Cell cell, int row, int col,
                                  bool merge_map[BOARD_ROWS][BOARD_COLS]) {
  int target_col = col;
  for (; target_col > 0; target_col--)
    if (!IsCellEmpty(board->cells[row][target_col - 1]))
      break;

  if (target_col > 0 && board->cells[row][target_col - 1] == cell &&
      !merge_map[row][target_col - 1]) {
    target_col--;
  }
  return target_col;
}

static int CalculateTargetColRight(Board *board, Cell cell, int row, int col,
                                   bool merge_map[BOARD_ROWS][BOARD_COLS]) {
  int target_col = col;
  for (; target_col < BOARD_COLS - 1; target_col++)
    if (!IsCellEmpty(board->cells[row][target_col + 1]))
      break;

  if (target_col < BOARD_COLS - 1 &&
      board->cells[row][target_col + 1] == cell &&
      !merge_map[row][target_col + 1])
    target_col++;
  return target_col;
}

static int CalculateTargetRowUp(Board *board, Cell cell, int row, int col,
                                bool merge_map[BOARD_ROWS][BOARD_COLS]) {
  int target_row = row;
  for (; target_row > 0; target_row--)
    if (!IsCellEmpty(board->cells[target_row - 1][col]))
      break;

  if (target_row > 0 && board->cells[target_row - 1][col] == cell &&
      !merge_map[target_row - 1][col])
    target_row--;
  return target_row;
}

static int CalculateTargetRowDown(Board *board, Cell cell, int row, int col,
                                  bool merge_map[BOARD_ROWS][BOARD_COLS]) {
  int target_row = row;
  for (; target_row < BOARD_ROWS - 1; target_row++)
    if (!IsCellEmpty(board->cells[target_row + 1][col]))
      break;

  if (target_row < BOARD_COLS - 1 &&
      board->cells[target_row + 1][col] == cell &&
      !merge_map[target_row + 1][col])
    target_row++;
  return target_row;
}

static void MoveLeft(Board *board) {
  bool merge_map[BOARD_ROWS][BOARD_COLS] = {0};

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Cell cell = board->cells[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_col = CalculateTargetColLeft(board, cell, row, col, merge_map);
      bool is_merge =
          col != target_col && !IsCellEmpty(board->cells[row][target_col]);
      Vector2 from_pos = GetCellPosition(row, col);
      Vector2 to_pos = GetCellPosition(row, target_col);
      AddMoveAnimation(board, cell, is_merge, from_pos, to_pos);
      if (is_merge) {
        AddMergeAnimation(board, cell * 2, to_pos);
        merge_map[row][target_col] = true;
      }
      board->cells[row][col] = EMPTY_CELL;
      board->cells[row][target_col] = is_merge ? cell * 2 : cell;
    }
  }
}

static void MoveRight(Board *board) {
  bool merge_map[BOARD_ROWS][BOARD_COLS] = {0};

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = BOARD_COLS - 1; col >= 0; col--) {
      Cell cell = board->cells[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_col =
          CalculateTargetColRight(board, cell, row, col, merge_map);
      bool is_merge =
          col != target_col && !IsCellEmpty(board->cells[row][target_col]);
      Vector2 from_pos = GetCellPosition(row, col);
      Vector2 to_pos = GetCellPosition(row, target_col);
      AddMoveAnimation(board, cell, is_merge, from_pos, to_pos);
      if (is_merge) {
        AddMergeAnimation(board, cell * 2, to_pos);
        merge_map[row][target_col] = true;
      }
      board->cells[row][col] = EMPTY_CELL;
      board->cells[row][target_col] = is_merge ? cell * 2 : cell;
    }
  }
}

static void MoveUp(Board *board) {
  bool merge_map[BOARD_ROWS][BOARD_COLS] = {0};

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Cell cell = board->cells[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_row = CalculateTargetRowUp(board, cell, row, col, merge_map);
      bool is_merge =
          row != target_row && !IsCellEmpty(board->cells[target_row][col]);
      Vector2 from_pos = GetCellPosition(row, col);
      Vector2 to_pos = GetCellPosition(target_row, col);
      AddMoveAnimation(board, cell, is_merge, from_pos, to_pos);
      if (is_merge) {
        AddMergeAnimation(board, cell * 2, to_pos);
        merge_map[target_row][col] = true;
      }
      board->cells[row][col] = EMPTY_CELL;
      board->cells[target_row][col] = is_merge ? cell * 2 : cell;
    }
  }
}

static void MoveDown(Board *board) {
  bool merge_map[BOARD_ROWS][BOARD_COLS] = {0};

  for (int row = BOARD_ROWS - 1; row >= 0; row--) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Cell cell = board->cells[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_row = CalculateTargetRowDown(board, cell, row, col, merge_map);
      bool is_merge =
          row != target_row && !IsCellEmpty(board->cells[target_row][col]);
      Vector2 from_pos = GetCellPosition(row, col);
      Vector2 to_pos = GetCellPosition(target_row, col);
      AddMoveAnimation(board, cell, is_merge, from_pos, to_pos);
      if (is_merge) {
        AddMergeAnimation(board, cell * 2, to_pos);
        merge_map[target_row][col] = true;
      }
      board->cells[row][col] = EMPTY_CELL;
      board->cells[target_row][col] = is_merge ? cell * 2 : cell;
    }
  }
}
