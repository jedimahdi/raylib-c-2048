#include "board.h"
#include <raylib.h>
#include <raymath.h>

#define BACKGROUND_COLOR GetColor(0x574A3EFF)
#define BOARD_ROWS 4
#define BOARD_COLS 4
#define MAX_APPEARS 2
#define MAX_MERGES ((int)(MAX_TILES / 2))
#define BOARD_WIDTH 800.0
#define BOARD_HEIGHT 800.0
#define TILE_GAP_SIZE 22
#define MOVE_ANIMATION_DURATION 0.1
#define APPEAR_ANIMATION_DURATION 0.2
#define SCALE_UP_ANIMATION_DURATION 0.2
#define SCALE_DOWN_ANIMATION_DURATION 0.2

static const int screenWidth = 800;
static const int screenHeight = 800;

static Board board = {0};
static float animation_elapsed_time = 0.0f;
static bool game_lost = false;

static void init_game(void);
static void update_game(void);
static void draw_game(void);

static void init_game(void) {
  game_lost = false;
  animation_elapsed_time = MOVE_ANIMATION_DURATION;
  board.tiles_count = 0;

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      board.tiles_map[row][col] = EMPTY_TILE_VALUE;
    }
  }

  add_random_tile(&board);
  add_random_tile(&board);
}

static bool any_move_happen(void) {
  for (int i = 0; i < board.tiles_count; i++) {
    if (board.tiles[i].type == TILE_MOVE || board.tiles[i].type == TILE_MERGE) {
      return true;
    }
  }
  return false;
}

static bool check_game_lost(void) {
  int prev_value = -1;
  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      if (board.tiles_map[row][col] == EMPTY_TILE_VALUE ||
          board.tiles_map[row][col] == prev_value) {
        return false;
      }
      prev_value = board.tiles_map[row][col];
    }
    prev_value = -1;
  }
  prev_value = -1;
  for (int col = 0; col < BOARD_COLS; col++) {
    for (int row = 0; row < BOARD_ROWS; row++) {
      if (board.tiles_map[row][col] == prev_value) {
        return false;
      }
      prev_value = board.tiles_map[row][col];
    }
    prev_value = -1;
  }
  return true;
}

static void hanlde_move(V2i direction) {
  board.tiles_count = 0;
  animation_elapsed_time = 0;
  move_tiles(&board, direction);
  if (check_game_lost()) {
    game_lost = true;
    return;
  }
  if (any_move_happen()) {
    add_random_tile(&board);
  }
}

static void update_game(void) {
  if (IsKeyPressed(KEY_A)) {
    hanlde_move(DIRECTION_LEFT);
  }
  if (IsKeyPressed(KEY_D)) {
    hanlde_move(DIRECTION_RIGHT);
  }
  if (IsKeyPressed(KEY_W)) {
    hanlde_move(DIRECTION_UP);
  }
  if (IsKeyPressed(KEY_S)) {
    hanlde_move(DIRECTION_DOWN);
  }
  if (IsKeyPressed(KEY_R)) {
    init_game();
  }

  animation_elapsed_time += GetFrameTime();

  for (int i = 0; i < board.tiles_count; i++) {
    switch (board.tiles[i].type) {
    case TILE_APPEAR: {
      if (animation_elapsed_time >= MOVE_ANIMATION_DURATION &&
          animation_elapsed_time <=
              MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION) {
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION;
        board.tiles[i].scale =
            Lerp(0, 1, elapsed_time / APPEAR_ANIMATION_DURATION);
      } else if (animation_elapsed_time >
                 MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION) {
        board.tiles[i].scale = 1;
      }
    } break;
    case TILE_MOVE: {
      if (animation_elapsed_time <= MOVE_ANIMATION_DURATION) {
        board.tiles[i].pos =
            v2f_lerp(board.tiles[i].from_pos, board.tiles[i].to_pos,
                     animation_elapsed_time / MOVE_ANIMATION_DURATION);
      } else if (animation_elapsed_time > MOVE_ANIMATION_DURATION) {
        board.tiles[i].pos = board.tiles[i].to_pos;
      }
    } break;

    case TILE_MERGE: {
      if (animation_elapsed_time <= MOVE_ANIMATION_DURATION) {
        board.tiles[i].pos =
            v2f_lerp(board.tiles[i].from_pos, board.tiles[i].to_pos,
                     animation_elapsed_time / MOVE_ANIMATION_DURATION);
      } else if (animation_elapsed_time > MOVE_ANIMATION_DURATION &&
                 animation_elapsed_time <=
                     MOVE_ANIMATION_DURATION + SCALE_UP_ANIMATION_DURATION) {
        board.tiles[i].pos = board.tiles[i].to_pos;
        board.tiles[i].number = board.tiles[i].new_number;
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION;
        board.tiles[i].scale =
            Lerp(1, 1.1, elapsed_time / SCALE_UP_ANIMATION_DURATION);
      } else if (animation_elapsed_time >
                     MOVE_ANIMATION_DURATION + SCALE_UP_ANIMATION_DURATION &&
                 animation_elapsed_time <= MOVE_ANIMATION_DURATION +
                                               SCALE_UP_ANIMATION_DURATION +
                                               SCALE_DOWN_ANIMATION_DURATION) {
        board.tiles[i].pos = board.tiles[i].to_pos;
        board.tiles[i].number = board.tiles[i].new_number;
        float elapsed_time = animation_elapsed_time - MOVE_ANIMATION_DURATION -
                             SCALE_UP_ANIMATION_DURATION;
        board.tiles[i].scale =
            Lerp(1.1, 1, elapsed_time / SCALE_DOWN_ANIMATION_DURATION);
      } else {
        board.tiles[i].pos = board.tiles[i].to_pos;
        board.tiles[i].number = board.tiles[i].new_number;
        board.tiles[i].scale = 1;
      }
    } break;

    case TILE_IDLE:
      break;
    }
  }
}

static void draw_game(void) {
  BeginDrawing();
  ClearBackground(BACKGROUND_COLOR);

  for (int row = 0; row < BOARD_ROWS; row++) {
    for (int col = 0; col < BOARD_COLS; col++) {
      Vector2 pos = get_tile_position(v2i(col, row));
      DrawRectangleRounded((Rectangle){.x = pos.x,
                                       .y = pos.y,
                                       .width = TILE_WIDTH,
                                       .height = TILE_HEIGHT},
                           0.05, 0, GetColor(0x392A1A55));
    }
  }

  for (int i = 0; i < board.tiles_count; i++) {
    draw_tile(board.tiles[i]);
  }

  if (game_lost) {
    const char *lost_msg = "Game lost! To restart game press R.";
    int half_text_size = MeasureText(lost_msg, 32) / 2;
    DrawText(lost_msg, screenWidth / 2 - half_text_size, screenHeight / 2, 32,
             WHITE);
  }

  EndDrawing();
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "2048");
  SetTargetFPS(60);
  init_game();

  while (!WindowShouldClose()) {
    update_game();
    draw_game();
  }

  struct pw_core *core;

  // pw_init();
  // loop = pw_main(NULL /* properties */);

  CloseWindow();
}
