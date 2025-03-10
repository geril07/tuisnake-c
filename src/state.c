#include "state.h"
#include "game.h"
#include "tui.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SomeState *state;

static void state_update_from_tui() {
  assert(state != NULL);
  assert(tui != NULL);
  if (state == NULL || tui == NULL)
    return;

  state->cols = tui->cols;
  state->rows = tui->rows;
}

void state_render() {

  TUIGrid *grid = tui_init_grid();
  assert(grid != NULL);

  /* surface_render(gridp); */
  game_render(grid);
  tui_grid_free(tui->grid);
  tui->grid = grid;
  tui_draw_screen();
  /* tui_apply_new_grid(buf); */
}

void state_update() {
  if (tui == NULL || state == NULL)
    return;
  char *rows_msg = "Rows: ";
  char *cols_msg = "Cols: ";
  char cols_result[12];
  char rows_result[12];
  sprintf(rows_result, "%s%d", rows_msg, tui->rows);
  sprintf(cols_result, "%s%d", cols_msg, tui->cols);

  state_render();
}

void state_update_on_resize() {
  state_update_from_tui();
  state_update();
}

void state_init() {
  state = malloc(sizeof(SomeState));
  state_update_from_tui();
  state_update();
}
