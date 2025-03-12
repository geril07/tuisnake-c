#include "events.h"
#include "game.h"
#include "log.h"
#include "state.h"
#include "surface.h"
#include "tui.h"
#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

void redraw() {
  tui_clear_screen();
  tui_draw_screen();
}

void rerender() {
  TUIGrid *grid = tui_init_grid();
  assert(grid != NULL);

  /* surface_render(gridp); */
  game_render(grid);

  tui_grid_free(tui->grid);
  tui->grid = grid;
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
  atexit(events_handle_exit);

  tui_hide_cursor();
  tui_enable_raw_mode();
  tui_enable_alternate_buffer();
  setlocale(LC_ALL, "");

  while (true) {
    game_tick_update();

    rerender();
    redraw();
    /* check_input(); */
    usleep(50e3);
    /* char ch = getchar(); */
    /* local_tui.buffer[0] = ch; */
  }

  events_handle_exit();

  return 0;
}
