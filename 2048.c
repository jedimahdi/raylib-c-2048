#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define BACKGROUND_COLOR GetColor(0x574A3EFF)
#define BOARD_ROWS 4
#define BOARD_COLS 4
#define MAX_TILES (BOARD_ROWS * BOARD_COLS)
#define MAX_APPEARS 2
#define MAX_MERGES ((int)(MAX_TILES / 2))
#define BOARD_WIDTH 800.0
#define BOARD_HEIGHT 800.0
#define TILE_GAP_SIZE 22
#define TILE_WIDTH                                                             \
  ((BOARD_WIDTH - (TILE_GAP_SIZE * (BOARD_COLS + 1))) / BOARD_COLS)
#define TILE_HEIGHT                                                            \
  ((BOARD_HEIGHT - (TILE_GAP_SIZE * (BOARD_ROWS + 1))) / BOARD_ROWS)
#define MOVE_ANIMATION_DURATION 0.1
#define APPEAR_ANIMATION_DURATION 0.2
#define SCALE_UP_ANIMATION_DURATION 0.2
#define SCALE_DOWN_ANIMATION_DURATION 0.2

typedef struct {
  int row;
  int col;
} BoardPosition;

typedef enum {
  TILE_IDLE,
  TILE_MOVE,
  TILE_MERGE,
  TILE_APPEAR,
} TileType;

typedef struct {
  TileType type;
  Vector2 pos;
  Vector2 from_pos;
  Vector2 to_pos;
  int number;
  int new_number;
  float scale;
} Tile;

static const int screenWidth = 800;
static const int screenHeight = 800;

static int tile_map[BOARD_ROWS][BOARD_COLS];

static Tile tiles[MAX_TILES + MAX_MERGES];
static int tiles_count = 0;

static bool merge_map[BOARD_ROWS][BOARD_COLS];

static bool animation_active = true;
static float animation_elapsed_time = 0.0f;

static void InitGame(void);
static void UpdateGame(void);
static void DrawGame(void);

static Vector2 GetTilePosition(int row, int col) {
  return (Vector2){.x = col * TILE_WIDTH + ((col + 1) * TILE_GAP_SIZE),
                   .y = row * TILE_HEIGHT + ((row + 1) * TILE_GAP_SIZE)};
}

static bool IsCellEmpty(int tile) { return tile == 0; }

static void AddRandomCell(void) {
  BoardPosition empty_cells[MAX_TILES];
  size_t empty_cells_count = 0;

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      int cell = tile_map[row][col];
      if (IsCellEmpty(cell)) {
        empty_cells[empty_cells_count++] =
            (BoardPosition){.row = row, .col = col};
      }
    }
  }
  if (empty_cells_count == 0) {
    return;
  }
  int r = GetRandomValue(0, empty_cells_count - 1);
  BoardPosition choosen = empty_cells[r];
  tile_map[choosen.row][choosen.col] = 2;
  Vector2 pos = GetTilePosition(choosen.row, choosen.col);
  tiles[tiles_count++] = (Tile){.type = TILE_APPEAR,
                                .number = 2,
                                .new_number = 2,
                                .scale = 0,
                                .from_pos = pos,
                                .to_pos = pos,
                                .pos = pos};
}

static void InitGame(void) {
  animation_active = true;
  animation_elapsed_time = MOVE_ANIMATION_DURATION;
  tiles_count = 0;

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      tile_map[row][col] = 0;
    }
  }

  for (int i = 0; i < MAX_TILES; i++) {
    tiles[i].scale = 1;
    tiles[i].type = TILE_IDLE;
    tiles[i].pos = Vector2Zero();
    tiles[i].from_pos = Vector2Zero();
    tiles[i].to_pos = Vector2Zero();
  }

  AddRandomCell();
  AddRandomCell();
}

static Vector2 LerpVector2(Vector2 start, Vector2 end, float amount) {
  return Vector2Add(start, Vector2Scale((Vector2Subtract(end, start)), amount));
}

static int CalculateTargetColLeft(int cell, int row, int col) {
  int target_col = col;
  for (; target_col > 0; target_col--)
    if (!IsCellEmpty(tile_map[row][target_col - 1]))
      break;

  if (target_col > 0 && tile_map[row][target_col - 1] == cell &&
      !merge_map[row][target_col - 1]) {
    target_col--;
  }
  return target_col;
}

static int CalculateTargetColRight(int cell, int row, int col) {
  int target_col = col;
  for (; target_col < BOARD_COLS - 1; target_col++)
    if (!IsCellEmpty(tile_map[row][target_col + 1]))
      break;

  if (target_col < BOARD_COLS - 1 && tile_map[row][target_col + 1] == cell &&
      !merge_map[row][target_col + 1])
    target_col++;
  return target_col;
}

static int CalculateTargetRowUp(int cell, int row, int col) {
  int target_row = row;
  for (; target_row > 0; target_row--)
    if (!IsCellEmpty(tile_map[target_row - 1][col]))
      break;

  if (target_row > 0 && tile_map[target_row - 1][col] == cell &&
      !merge_map[target_row - 1][col])
    target_row--;
  return target_row;
}

static int CalculateTargetRowDown(int cell, int row, int col) {
  int target_row = row;
  for (; target_row < BOARD_ROWS - 1; target_row++)
    if (!IsCellEmpty(tile_map[target_row + 1][col]))
      break;

  if (target_row < BOARD_COLS - 1 && tile_map[target_row + 1][col] == cell &&
      !merge_map[target_row + 1][col])
    target_row++;
  return target_row;
}

// static BoardPosition CalculateTargetPosition(int cell, BoardPosition pos) {
//     CalculateTargetColLeft(cell, pos.row, pos.col);
// }

static void MoveTile(int cell, int row, int col, int target_row,
                     int target_col) {
  Vector2 from_pos = GetTilePosition(row, col);
  Vector2 to_pos = GetTilePosition(target_row, target_col);

  if (col == target_col && row == target_row) {
    tiles[tiles_count++] = (Tile){.type = TILE_IDLE,
                                  .scale = 1,
                                  .new_number = cell,
                                  .number = cell,
                                  .from_pos = from_pos,
                                  .to_pos = to_pos,
                                  .pos = from_pos};
  } else if (!IsCellEmpty(tile_map[target_row][target_col])) {
    tiles[tiles_count++] = (Tile){.type = TILE_MERGE,
                                  .scale = 1,
                                  .new_number = cell * 2,
                                  .number = cell,
                                  .from_pos = from_pos,
                                  .to_pos = to_pos,
                                  .pos = from_pos};
    tile_map[row][col] = 0;
    tile_map[target_row][target_col] = cell * 2;
    merge_map[target_row][target_col] = true;
  } else {
    tiles[tiles_count++] = (Tile){.type = TILE_MOVE,
                                  .scale = 1,
                                  .new_number = cell,
                                  .number = cell,
                                  .from_pos = from_pos,
                                  .to_pos = to_pos,
                                  .pos = from_pos};
    tile_map[row][col] = 0;
    tile_map[target_row][target_col] = cell;
  }
}

static void MoveLeft(void) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      merge_map[row][col] = false;
    }
  }

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      int cell = tile_map[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_col = CalculateTargetColLeft(cell, row, col);
      MoveTile(cell, row, col, row, target_col);
    }
  }
}

static void MoveRight(void) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      merge_map[row][col] = false;
    }
  }

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = BOARD_COLS - 1; col >= 0; col--) {
      int cell = tile_map[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_col = CalculateTargetColRight(cell, row, col);
      MoveTile(cell, row, col, row, target_col);
    }
  }
}

static void MoveUp(void) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      merge_map[row][col] = false;
    }
  }

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      int cell = tile_map[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_row = CalculateTargetRowUp(cell, row, col);
      MoveTile(cell, row, col, target_row, col);
    }
  }
}

static void MoveDown(void) {
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      merge_map[row][col] = false;
    }
  }

  for (int row = BOARD_ROWS - 1; row >= 0; row--) {
    for (int col = 0; col < BOARD_COLS; col++) {
      int cell = tile_map[row][col];
      if (IsCellEmpty(cell))
        continue;

      int target_row = CalculateTargetRowDown(cell, row, col);
      MoveTile(cell, row, col, target_row, col);
    }
  }
}

static bool AnyMoveHappen(void) {
  for (int i = 0; i < tiles_count; i++) {
    if (tiles[i].type == TILE_MOVE || tiles[i].type == TILE_MERGE) {
      return true;
    }
  }
  return false;
}

static bool IsGameLost(void) {
  int prev_value = -1;
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      if (IsCellEmpty(tile_map[row][col]) || tile_map[row][col] == prev_value) {
        return false;
      }
      prev_value = tile_map[row][col];
    }
  }
  prev_value = -1;
  for (int col = 0; col < BOARD_COLS; col++) {
    for (int row = 0; row < BOARD_ROWS; row++) {
      if (tile_map[row][col] == prev_value) {
        return false;
      }
      prev_value = tile_map[row][col];
    }
  }
  return true;
}

static void UpdateGame(void) {
  if (IsKeyPressed(KEY_A)) {
    tiles_count = 0;
    animation_elapsed_time = 0;
    MoveLeft();
    if (AnyMoveHappen() && !IsGameLost()) {
      AddRandomCell();
    }
  }
  if (IsKeyPressed(KEY_D)) {
    tiles_count = 0;
    animation_elapsed_time = 0;
    MoveRight();
    if (AnyMoveHappen() && !IsGameLost()) {
      AddRandomCell();
    }
  }
  if (IsKeyPressed(KEY_W)) {
    tiles_count = 0;
    animation_elapsed_time = 0;
    MoveUp();
    if (AnyMoveHappen() && !IsGameLost()) {
      AddRandomCell();
    }
  }
  if (IsKeyPressed(KEY_S)) {
    tiles_count = 0;
    animation_elapsed_time = 0;
    MoveDown();
    if (AnyMoveHappen() && !IsGameLost()) {
      AddRandomCell();
    }
  }

  // if (animation_active) {
  animation_elapsed_time += GetFrameTime();
  // }

  for (int i = 0; i < tiles_count; i++) {
    switch (tiles[i].type) {
    case TILE_APPEAR: {
      if (animation_elapsed_time >= MOVE_ANIMATION_DURATION &&
          animation_elapsed_time <=
              MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION) {
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION;
        tiles[i].scale = Lerp(0, 1, elapsed_time / APPEAR_ANIMATION_DURATION);
      } else if (animation_elapsed_time >
                 MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION) {
        tiles[i].scale = 1;
      }
    } break;
    case TILE_MOVE: {
      if (animation_elapsed_time <= MOVE_ANIMATION_DURATION) {
        tiles[i].pos =
            LerpVector2(tiles[i].from_pos, tiles[i].to_pos,
                        animation_elapsed_time / MOVE_ANIMATION_DURATION);
      } else if (animation_elapsed_time > MOVE_ANIMATION_DURATION) {
        tiles[i].pos = tiles[i].to_pos;
      }
    } break;

    case TILE_MERGE: {
      if (animation_elapsed_time <= MOVE_ANIMATION_DURATION) {
        tiles[i].pos =
            LerpVector2(tiles[i].from_pos, tiles[i].to_pos,
                        animation_elapsed_time / MOVE_ANIMATION_DURATION);
      } else if (animation_elapsed_time > MOVE_ANIMATION_DURATION &&
                 animation_elapsed_time <=
                     MOVE_ANIMATION_DURATION + SCALE_UP_ANIMATION_DURATION) {
        tiles[i].pos = tiles[i].to_pos;
        tiles[i].number = tiles[i].new_number;
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION;
        tiles[i].scale =
            Lerp(1, 1.1, elapsed_time / SCALE_UP_ANIMATION_DURATION);
      } else if (animation_elapsed_time >
                     MOVE_ANIMATION_DURATION + SCALE_UP_ANIMATION_DURATION &&
                 animation_elapsed_time <= MOVE_ANIMATION_DURATION +
                                               SCALE_UP_ANIMATION_DURATION +
                                               SCALE_DOWN_ANIMATION_DURATION) {
        tiles[i].pos = tiles[i].to_pos;
        tiles[i].number = tiles[i].new_number;
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION -
                             SCALE_UP_ANIMATION_DURATION;
        tiles[i].scale =
            Lerp(1.1, 1, elapsed_time / SCALE_DOWN_ANIMATION_DURATION);
      } else {
        tiles[i].pos = tiles[i].to_pos;
        tiles[i].number = tiles[i].new_number;
        tiles[i].scale = 1;
      }
    } break;

    case TILE_IDLE:
      break;
    }
  }
}

static Color GetTileColor(int number) {
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

static void DrawTile(Tile tile) {
  float height = TILE_HEIGHT * tile.scale;
  float width = TILE_WIDTH * tile.scale;
  Rectangle rect = {.height = height,
                    .width = width,
                    .x = tile.pos.x - ((width - TILE_WIDTH) / 2),
                    .y = tile.pos.y - ((height - TILE_HEIGHT) / 2)};
  DrawRectangleRounded(rect, 0.05, 0, GetTileColor(tile.number));
  char number_str[6];
  sprintf(number_str, "%d", tile.number);
  int font_size = floorf(Lerp(0, 52, tile.scale));
  int half_text_size = MeasureText(number_str, font_size) / 2;
  DrawText(number_str, tile.pos.x + (TILE_WIDTH / 2) - half_text_size,
           tile.pos.y + (TILE_HEIGHT / 2) - 22, font_size, LIGHTGRAY);
}

static void DrawGame(void) {
  BeginDrawing();
  ClearBackground(BACKGROUND_COLOR);

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Vector2 pos = GetTilePosition(row, col);
      DrawRectangleRounded((Rectangle){.x = pos.x,
                                       .y = pos.y,
                                       .width = TILE_WIDTH,
                                       .height = TILE_HEIGHT},
                           0.05, 0, GetColor(0x392A1A55));
    }
  }

  for (int i = 0; i < tiles_count; i++) {
    DrawTile(tiles[i]);
  }

  EndDrawing();
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "2048");
  SetTargetFPS(60);
  InitGame();

  while (!WindowShouldClose()) {
    UpdateGame();
    DrawGame();
  }

  CloseWindow();
}
