#include "surface.h"
#include "tui.h"
#include <assert.h>
#include <stdlib.h>

const int BASE_WIDTH = 60;
const int BASE_HEIGHT = BASE_WIDTH / 2;
const char BORDER_CHAR = '#';

Surface *surface;

void surface_render(char *buffer) {
  if (surface == NULL)
    return;

  int cols = tui->cols;
  int rows = tui->rows;
  int start_col = surface->start_col;
  int end_col = surface->start_col + surface->width;
  int start_row = surface->start_row;
  int end_row = surface->start_row + surface->height;

  // HORIZONTAL
  for (int x = start_col; x <= end_col; x++) {
    /* int x = start_x + i; */
    *tui_buffer_at(buffer, x, start_row, cols, rows) = BORDER_CHAR;
    *tui_buffer_at(buffer, x, end_row, cols, rows) = BORDER_CHAR;
  }
  // VERTICAL
  for (int y = start_row; y <= end_row; y++) {
    /* int x = start_x + i; */
    *tui_buffer_at(buffer, start_col, y, cols, rows) = BORDER_CHAR;
    *tui_buffer_at(buffer, end_col, y, cols, rows) = BORDER_CHAR;
  }
}

void surface_calculate_size(int *start_row, int *start_col) {
  if (tui == NULL || tui->cols == 0 || tui->rows == 0)
    return;

  /* *start_col = tui->cols / 2; */
  /* *start_row = tui->rows / 2; */
  *start_col = 60;
  *start_row = 5;
}

void surface_recalculate() {
  if (surface == NULL || tui == NULL)
    return;

  int height = tui->rows - 8;
  surface->width = height * 2;
  surface->height = height;
  surface_calculate_size(&surface->start_row, &surface->start_col);
}

void surface_init() {
  assert(tui != NULL);

  surface = malloc(sizeof(Surface));
  surface_recalculate();
};

void surface_cleanup() {
  if (surface == NULL)
    return;

  free(surface);
}
