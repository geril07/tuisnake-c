#pragma once

typedef enum {
  INPUT_NONE,
  INPUT_W,
  INPUT_A,
  INPUT_S,
  INPUT_D,
} InputKey;

typedef struct {
  InputKey key;
} InputEvent;

InputEvent *input_read();
InputEvent *input_get_event();
