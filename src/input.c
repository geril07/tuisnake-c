#include "input.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static InputEvent *last_input_event;

bool input_compare_inputs(InputEvent *first, InputEvent *second) {
  assert(first != NULL);
  assert(second != NULL);

  return first->key == second->key;
}

InputEvent *input_create_empty_event() {
  InputEvent *input_event = malloc(sizeof(InputEvent));
  assert(input_event != NULL);
  input_event->key = INPUT_NONE;
  return input_event;
}

InputEvent *input_get_event() {
  InputEvent *input_event = input_read();
  if (input_event == NULL) {
    last_input_event = NULL;
    return input_event;
  }

  if (last_input_event == NULL) {
    last_input_event = input_create_empty_event();
    memcpy(last_input_event, input_event, sizeof(InputEvent));

    return input_event;
  }

  bool is_same_input = input_compare_inputs(input_event, last_input_event);
  if (is_same_input)
    return input_get_event();

  memcpy(last_input_event, input_event, sizeof(InputEvent));
  return input_event;
}

InputEvent *input_read() {
  char buf;
  if (read(STDIN_FILENO, &buf, 1) > 0) {
    InputEvent *input_event = input_create_empty_event();
    switch (buf) {
    case 'a': {
      input_event->key = INPUT_A;
      break;
    }
    case 'w': {
      input_event->key = INPUT_W;
      break;
    }
    case 's': {
      input_event->key = INPUT_S;
      break;
    }
    case 'd': {
      input_event->key = INPUT_D;
      break;
    }
    default: {
      free(input_event);
      return NULL;
    }
    }
    return input_event;
  };

  return NULL;
}
