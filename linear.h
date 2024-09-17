#ifndef LINEAR_H_
#define LINEAR_H_

#include "raylib.h"

typedef struct {
  int x, y;
} V2i;

int mini(int a, int b);
int maxi(int a, int b);
int clampi(int x, int a, int b);

V2i v2i(int x, int y);
V2i v2i_add(V2i a, V2i b);
V2i v2i_sub(V2i a, V2i b);
V2i v2i_scale(V2i a, int scale);
V2i v2i_clamp(V2i x, V2i a, V2i b);
bool v2i_eq(V2i a, V2i b);

typedef Vector2 V2f;

V2f v2f(float x, float y);
V2f v2f_add(V2f a, V2f b);
V2f v2f_sub(V2f a, V2f b);
V2f v2f_lerp(V2f start, V2f end, float t);

#define DIRECTION_RIGHT v2i(1, 0)
#define DIRECTION_LEFT v2i(-1, 0)
#define DIRECTION_UP v2i(0, -1)
#define DIRECTION_DOWN v2i(0, 1)

V2i reverse_direction(V2i d);

#endif // LINEAR_H_
