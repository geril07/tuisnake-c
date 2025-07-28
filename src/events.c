#include "tui.h"
#include <stdio.h>
#include <stdlib.h>

void events_handle_exit() {
  tui_show_cursor();
  tui_disable_alternate_buffer();
  tui_reset_terminal();
  fflush(stdout);
  exit(0);
}

void events_handle_resize() {
  if (tui == NULL)
    return;

  tui_get_terminal_size(&tui->rows, &tui->cols);
}

void events_handle_sigwinch(int sig) { events_handle_resize(); }

void events_handle_sigint() { events_handle_exit(); }
