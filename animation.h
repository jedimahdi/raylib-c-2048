#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

#define MOVE_ANIMATION_DURATION 0.15
#define APPEAR_ANIMATION_DURATION 0.2
#define ANIMATION_DURATION (MOVE_ANIMATION_DURATION + APPEAR_ANIMATION_DURATION)
#define SCALE_UP_DURATION (APPEAR_ANIMATION_DURATION / 2)
#define SCALE_DOWN_DURATION (APPEAR_ANIMATION_DURATION - SCALE_UP_DURATION)

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

float EaseIn(float x);
float Flip(float x);
float EaseOut(float x);
float SmoothStep(float x);
void UpdateAnimation(Animation *animation);
void ClearAnimations(Animation *animation);
void AddMoveAnimation(Animation *animation, int cell, bool is_merge,
                      Vector2 from_pos, Vector2 to_pos);
void AddMergeAnimation(Animation *animation, int cell, Vector2 pos);
void AddAppearAnimation(Animation *animation, int cell, Vector2 pos);
bool IsAnimationPlaying(Animation *animation);
void PlayAnimation(Animation *animation);
void StopAnimation(Animation *animation);

#endif // -- ANIMATION_H
