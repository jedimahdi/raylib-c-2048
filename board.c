#include "board.h"
#include "linear.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

bool is_tile_inbounds(V2i tile) {
  return tile.x >= 0 && tile.y >= 0 && tile.x < BOARD_SIZE &&
         tile.y < BOARD_SIZE;
}

V2i get_starting_tile(V2i direction) {
  return v2i_clamp(v2i_scale(direction, BOARD_SIZE), v2i(0, 0),
                   v2i(BOARD_SIZE - 1, BOARD_SIZE - 1));
}

bool get_next_tile(V2i *tile, V2i direction) {
  V2i reversed_direction = reverse_direction(direction);
  V2i curr_tile = *tile;
  V2i next = v2i_add(curr_tile, reversed_direction);
  if (is_tile_inbounds(next)) {
    *tile = next;
    return true;
  }

  next = v2i_sub(v2i_add(curr_tile, v2i(abs(reversed_direction.y),
                                        abs(reversed_direction.x))),
                 v2i_scale(reversed_direction, BOARD_SIZE - 1));
  if (is_tile_inbounds(next)) {
    *tile = next;
    return true;
  }
  return false;
}

Tile make_idle_tile(int value, V2f pos) {
  return (Tile){.type = TILE_IDLE,
                .scale = 1,
                .new_number = value,
                .number = value,
                .from_pos = pos,
                .to_pos = pos,
                .pos = pos};
}
Tile make_appear_tile(int value, V2f pos) {
  return (Tile){.type = TILE_APPEAR,
                .number = value,
                .new_number = value,
                .scale = 0,
                .from_pos = pos,
                .to_pos = pos,
                .pos = pos};
}
Tile make_move_tile(int value, V2f from_pos, V2f to_pos) {
  return (Tile){.type = TILE_MOVE,
                .scale = 1,
                .new_number = value,
                .number = value,
                .from_pos = from_pos,
                .to_pos = to_pos,
                .pos = from_pos};
}
Tile make_merge_tile(int value, int new_value, V2f from_pos, V2f to_pos) {
  return (Tile){.type = TILE_MERGE,
                .scale = 1,
                .new_number = new_value,
                .number = value,
                .from_pos = from_pos,
                .to_pos = to_pos,
                .pos = from_pos};
}

int get_tile_value(Board *board, V2i tile) {
  return board->tiles_map[tile.y][tile.x];
}

void set_tile_value(Board *board, V2i tile, int value) {
  board->tiles_map[tile.y][tile.x] = value;
}

V2f get_tile_position(V2i tile) {
  int row = tile.y;
  int col = tile.x;
  return (V2f){.x = col * TILE_WIDTH + ((col + 1) * TILE_GAP_SIZE),
               .y = row * TILE_HEIGHT + ((row + 1) * TILE_GAP_SIZE)};
}

bool add_random_tile(Board *board) {
  V2i empty_tiles[MAX_TILES];
  int empty_tiles_count = 0;

  V2i iteration_direction = DIRECTION_RIGHT;
  V2i tile = get_starting_tile(iteration_direction);
  do {
    if (get_tile_value(board, tile) == EMPTY_TILE_VALUE) {
      empty_tiles[empty_tiles_count++] = tile;
    }
  } while (get_next_tile(&tile, iteration_direction));

  if (empty_tiles_count == 0)
    return false;

  int random_index = GetRandomValue(0, empty_tiles_count - 1);
  V2i choosen_tile = empty_tiles[random_index];
  set_tile_value(board, choosen_tile, 2);
  V2f pos = get_tile_position(choosen_tile);
  board->tiles[board->tiles_count++] = make_appear_tile(2, pos);
  return true;
}

void move_tiles(Board *board, V2i direction) {
  bool merge_map[BOARD_SIZE][BOARD_SIZE] = {0};
  V2i tile = get_starting_tile(direction);
  do {
    if (get_tile_value(board, tile) == EMPTY_TILE_VALUE)
      continue;

    int tile_value = get_tile_value(board, tile);

    V2i target_tile = tile;
    V2i next_tile = v2i_add(target_tile, direction);

    while (is_tile_inbounds(next_tile) &&
           (get_tile_value(board, next_tile) == EMPTY_TILE_VALUE ||
            (get_tile_value(board, next_tile) == tile_value &&
             !merge_map[next_tile.y][next_tile.x]))) {
      target_tile = next_tile;
      next_tile = v2i_add(next_tile, direction);
      if (get_tile_value(board, target_tile) != EMPTY_TILE_VALUE) {
        break;
      }
    }

    int target_tile_value = get_tile_value(board, target_tile);
    V2f tile_pos = get_tile_position(tile);
    V2f target_tile_pos = get_tile_position(target_tile);

    if (v2i_eq(tile, target_tile)) {
      board->tiles[board->tiles_count++] = make_idle_tile(tile_value, tile_pos);
    } else if (target_tile_value == tile_value) {
      board->tiles[board->tiles_count++] = make_merge_tile(
          target_tile_value, target_tile_value * 2, get_tile_position(tile),
          get_tile_position(target_tile));
      set_tile_value(board, tile, EMPTY_TILE_VALUE);
      set_tile_value(board, target_tile, target_tile_value * 2);
      merge_map[target_tile.y][target_tile.x] = true;
    } else {
      board->tiles[board->tiles_count++] = make_move_tile(
          tile_value, get_tile_position(tile), get_tile_position(target_tile));
      set_tile_value(board, tile, EMPTY_TILE_VALUE);
      set_tile_value(board, target_tile, tile_value);
    }
  } while (get_next_tile(&tile, direction));
}

static Color get_tile_color(int number) {
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

void draw_tile(Tile tile) {
  float height = TILE_HEIGHT * tile.scale;
  float width = TILE_WIDTH * tile.scale;
  Rectangle rect = {.height = height,
                    .width = width,
                    .x = tile.pos.x - ((width - TILE_WIDTH) / 2),
                    .y = tile.pos.y - ((height - TILE_HEIGHT) / 2)};
  DrawRectangleRounded(rect, 0.05, 0, get_tile_color(tile.number));
  char number_str[6];
  sprintf(number_str, "%d", tile.number);
  int font_size = floorf(Lerp(0, 52, tile.scale));
  int half_text_size = MeasureText(number_str, font_size) / 2;
  DrawText(number_str, tile.pos.x + (TILE_WIDTH / 2) - half_text_size,
           tile.pos.y + (TILE_HEIGHT / 2) - 22, font_size, LIGHTGRAY);
}
