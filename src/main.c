#include "events.h"
#include "game.h"
#include "log.h"
#include "state.h"
#include "surface.h"
#include "tui.h"
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
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
  srand(time(NULL));
  tui_init();
  game_init();
  surface_init();
  state_init();

  signal(SIGINT, events_handle_exit);
  signal(SIGWINCH, events_handle_resize);

  tui_hide_cursor();
  tui_enable_raw_mode();
  tui_enable_alternate_buffer();

  while (true) {
    game_tick_update();

    state_update_tui_back_buffer();
    redraw();
    usleep(25e3);
    /* char ch = getchar(); */
    /* local_tui.buffer[0] = ch; */
  }

  events_handle_exit();

  return 0;
}
