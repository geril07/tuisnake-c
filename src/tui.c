#include "tui.h"
#include "log.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

TUIData *tui;

void tui_clear_screen() {
  printf("\x1b[2J");
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
}

void tui_hide_cursor() {
  printf("\x1b[?25l");
  fflush(stdout);
}

void tui_show_cursor() {
  printf("\x1b[?25h");
  fflush(stdout);
}

char *tui_create_buffer() {
  assert(tui != NULL);

  assert(tui->cols != 0);
  assert(tui->rows != 0);

  char *buf = malloc(tui->cols * tui->rows);
  assert(buf != NULL);
  memset(buf, ' ', tui->rows * tui->cols);

  return buf;
}

void tui_free(TUIData *tui_ctx) {
  if (tui_ctx->front_buffer != NULL) {
    free(tui_ctx->front_buffer);
  }
}

void tui_enable_raw_mode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);          // Get current terminal attributes
  term.c_lflag &= ~(ECHO | ICANON);        // Disable echo and canonical mode
  tcsetattr(STDIN_FILENO, TCSANOW, &term); // Apply changes immediately
}

void tui_disable_raw_mode() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= (ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void tui_draw_screen() {
  if (tui == NULL || tui->front_buffer == NULL || tui->rows == 0 ||
      tui->cols == 0)
    return;

  // move cursor to top bottom
  printf("\x1b[H");

  int rows = tui->rows;
  int cols = tui->cols;
  char *buf = tui->front_buffer;

  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < cols; y++) {
      char value = buf[x * cols + y];
      if (value == '\0') {
        value = ' ';
      }
      printf("%c", value);
    }
  }

  fflush(stdout);
}

void tui_show_back_buffer() {
  if (tui == NULL)
    return;

  if (tui->front_buffer != NULL)
    free(tui->front_buffer);

  tui->front_buffer = tui->back_buffer;
}

void tui_init() {
  TUIData *data = malloc(sizeof(TUIData));
  tui = data;

  tui_update_terminal_size();
}
