#pragma once

typedef enum {
  INPUT_W,
  INPUT_A,
  INPUT_S,
  INPUT_D,
} InputKey;

typedef struct {
  InputKey key;
} InputEvent;

InputEvent *check_input();
void process_input();
