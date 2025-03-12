#include "tui.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

TUIData *tui;

TUICell *tui_grid_cell_at(TUIGrid *grid, int col, int row, int cols, int rows) {
  return &grid->cells[row * cols + col];
}

void tui_get_center_point(int *col, int *row) {
  if (tui == NULL || tui->cols == 0 || tui->rows == 0)
    return;

  *col = tui->cols / 2;
  *row = tui->rows / 2;
}

void tui_clear_screen() {
  printf("\x1b[3J");
  fflush(stdout);
}

void tui_enable_alternate_buffer() {
  printf("\x1b[?1049h");
  fflush(stdout);
}

void tui_disable_alternate_buffer() {
  printf("\x1b[?1049l");
  fflush(stdout);
}

void tui_get_terminal_size(int *rows, int *cols) {
  struct winsize w;
  if (ioctl(STDERR_FILENO, TIOCGWINSZ, &w) == 0) {
    *rows = w.ws_row;
    *cols = w.ws_col;
    return;
  }

  *rows = 24;
  *cols = 80;
}

void tui_update_terminal_size() {
  assert(tui != NULL);

  tui_get_terminal_size(&tui->rows, &tui->cols);
  TUIGrid *grid = tui_init_grid();
  assert(grid != NULL);
  tui->grid = grid;
}

void tui_hide_cursor() {
  printf("\x1b[?25l");
  fflush(stdout);
}

void tui_show_cursor() {
  printf("\x1b[?25h");
  fflush(stdout);
}

void tui_grid_free(TUIGrid *grid) {
  if (grid == NULL)
    return;

  if (grid->cells != NULL) {
    free(grid->cells);
  }

  free(grid);
}

void tui_free(TUIData *tui_ctx) {
  if (tui_ctx->grid != NULL) {
    tui_grid_free(tui_ctx->grid);
  }
}

static struct termios orig_term;

void tui_enable_raw_mode() {
  struct termios new_term;
  tcgetattr(STDIN_FILENO, &orig_term); // Get current terminal attributes
  memcpy(&new_term, &orig_term, sizeof(new_term));

  new_term.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term); // Apply changes immediately
}

void tui_reset_terminal() { tcsetattr(STDERR_FILENO, TCSANOW, &orig_term); }

void tui_disable_raw_mode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= (ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void tui_draw_screen() {
  assert(tui != NULL);
  assert(tui->rows != 0);
  assert(tui->cols != 0);

  // move cursor to top bottom
  printf("\x1b[H");

  TUIGrid *grid = tui->grid;
  int rows = grid->rows;
  int cols = grid->cols;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      TUICell *cell = tui_grid_cell_at(grid, col, row, cols, rows);
      wchar_t wchar = cell->cell_char;
      /* char value = buf[x * cols + y]; */
      if (wchar == '\0') {
        wchar = ' ';
      }
      printf("%lc", wchar);
    }
  }

  fflush(stdout);
}

void tui_apply_new_grid(TUIGrid *grid) {
  if (tui == NULL)
    return;

  /* if (tui->grid) */
  /* if (tui->front_buffer != NULL) */
  /*   free(tui->front_buffer); */
  /**/
  /* tui->front_buffer = buffer; */
}

void tui_init_cells(TUIGrid *grid) {
  grid->cells = calloc(grid->cols * grid->rows, sizeof(TUICell));
}

TUIGrid *tui_init_grid() {
  if (tui == NULL || tui->cols == 0 || tui->rows == 0)
    return NULL;

  TUIGrid *grid = malloc(sizeof(TUIGrid));
  assert(grid != NULL);

  grid->cols = tui->cols;
  grid->rows = tui->rows;
  tui_init_cells(grid);

  return grid;
}

void tui_show_back_buffer() {
  if (tui == NULL)
    return;

  /* tui_apply_new_grid(tui->back_buffer); */
  /* tui->front_buffer = tui->back_buffer; */
}

void tui_init() {
  TUIData *data = malloc(sizeof(TUIData));
  tui = data;

  tui_update_terminal_size();
}
