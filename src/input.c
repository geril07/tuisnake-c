#include "input.h"
#include "log.h"
#include <unistd.h>

static InputEvent *input_events;

void check_input() {
  char buf[3];
  read(STDIN_FILENO, buf, 3);
  log_message("[INPUT]: %s", buf);
}

void process_input() {}
