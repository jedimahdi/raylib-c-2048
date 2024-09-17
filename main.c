#include "board.h"
#include "linear.h"
#include <stdio.h>

int main(void) {
  V2i direction = DIRECTION_DOWN;
  V2i tile = get_starting_tile(direction);
  do {
    printf("v2i(%d, %d)\n", tile.x, tile.y);
  } while (get_next_tile(&tile, direction));

  return 0;
}
