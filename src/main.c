#include "events.h"
#include "game.h"
#include "log.h"
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

void rerender() {
  TUIGrid *grid = tui_init_grid();
  assert(grid != NULL);

  /* surface_render(gridp); */
  game_render(grid);
  surface_render(grid);

  tui_grid_free(tui->grid);
  tui->grid = grid;

  redraw();
}

//
// main is where all program execution starts
//
int main(int argc, char **argv) {
  log_init();
  srand(time(NULL));
  tui_init();
  game_init();
  surface_init();

  signal(SIGINT, events_handle_sigint);
  signal(SIGWINCH, events_handel_sigwinch);
  atexit(events_handle_exit);

  tui_hide_cursor();
  tui_enable_alternate_buffer();
  tui_enable_raw_mode();
  /* setlocale(LC_ALL, ""); */

  while (true) {
    game_tick_update();

    rerender();
    usleep(20e3);
  }

  events_handle_exit();

  return 0;
}
