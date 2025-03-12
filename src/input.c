#include "input.h"
#include "game.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

/* static InputEvent *input_events; */

InputEvent *check_input() {
  char buf;
  if (read(STDIN_FILENO, &buf, 1) > 0) {
    InputEvent *input_event = malloc(sizeof(InputEvent));
    assert(input_event != NULL);
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
