#include "events.h"
#include "log.h"
#include "state.h"
#include "tui.h"
#include <signal.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

void redraw() {
  tui_clear_screen();
  tui_draw_screen();
}
//
// main is where all program execution starts
//
int main(int argc, char **argv) {
  log_init();
  log_message("INIT");
  tui_init();
  state_init();

  signal(SIGINT, events_handle_exit);
  signal(SIGWINCH, events_handle_resize);

  tui_hide_cursor();
  tui_enable_raw_mode();

  while (true) {
    state_update_tui_back_buffer();
    redraw();
    usleep(25e3);
    /* char ch = getchar(); */
    /* local_tui.buffer[0] = ch; */
  }

  events_handle_exit();

  return 0;
}
