#include "linear.h"
#include "raymath.h"

int mini(int a, int b) { return a < b ? a : b; }

int maxi(int a, int b) { return a < b ? b : a; }

int clampi(int x, int a, int b) { return mini(maxi(a, x), b); }

V2i v2i(int x, int y) { return (V2i){.x = x, .y = y}; }

V2i v2i_add(V2i a, V2i b) { return v2i(a.x + b.x, a.y + b.y); }

V2i v2i_sub(V2i a, V2i b) { return v2i(a.x - b.x, a.y - b.y); }

V2i v2i_scale(V2i a, int scale) { return v2i(a.x * scale, a.y * scale); }

V2i v2i_clamp(V2i x, V2i a, V2i b) {
  return v2i(clampi(x.x, a.x, b.x), clampi(x.y, a.y, b.y));
}

bool v2i_eq(V2i a, V2i b) { return a.x == b.x && a.y == b.y; }

V2f v2f(float x, float y) { return (V2f){.x = x, .y = y}; }

V2f v2f_add(V2f a, V2f b) { return Vector2Add(a, b); }

V2f v2f_sub(V2f a, V2f b) { return Vector2Subtract(a, b); }

V2f v2f_lerp(V2f start, V2f end, float t) { return Vector2Lerp(start, end, t); }

V2i reverse_direction(V2i d) { return v2i(-d.x, -d.y); }
