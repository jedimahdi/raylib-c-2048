#include "board.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 800.0
#define WINDOW_HEIGHT 800.0
#define BACKGROUND_COLOR GetColor(0x574A3EFF)

int main(void) {
  srand(time(NULL));

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "2048 Game");
  SetTargetFPS(60);

  Board board;
  InitBoard(&board);

  while (!WindowShouldClose()) {
    UpdateBoard(&board);

    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);
    DrawBoard(&board);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
