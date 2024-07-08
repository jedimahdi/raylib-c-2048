#include "animation.h"
#include "array.h"
#include <raymath.h>

float EaseIn(float x) { return x * x; }
float Flip(float x) { return 1 - x; }
float EaseOut(float x) { return Flip(EaseIn(Flip(x))); }
float SmoothStep(float x) { return Lerp(EaseIn(x), EaseOut(x), x); }

void ClearAnimations(Animation *animation) {
  animation->elapsed_time = 0;
  animation->moves.count = 0;
  animation->appears.count = 0;
  animation->merges.count = 0;
}

void AddMoveAnimation(Animation *animation, int cell, bool is_merge,
                      Vector2 from_pos, Vector2 to_pos) {
  Move move = (Move){
      .from = from_pos, .to = to_pos, .is_merge = is_merge, .number = cell};
  da_append(&animation->moves, move);
}

void AddMergeAnimation(Animation *animation, int cell, Vector2 pos) {
  Merge merge = {.number = cell, .in = pos};
  da_append(&animation->merges, merge);
}

void AddAppearAnimation(Animation *animation, int cell, Vector2 pos) {
  Appear appear = {.number = cell, .in = pos};
  da_append(&animation->appears, appear);
}

void UpdateAnimation(Animation *animation) {
  float dt = GetFrameTime();
  animation->elapsed_time += dt;
  if (animation->elapsed_time > ANIMATION_DURATION) {
    animation->is_animation_playing = false;
  }
}

bool IsAnimationPlaying(Animation *animation) {
  return animation->is_animation_playing;
}

void PlayAnimation(Animation *animation) {
  animation->is_animation_playing = true;
}

void StopAnimation(Animation *animation) {
  animation->is_animation_playing = false;
}
