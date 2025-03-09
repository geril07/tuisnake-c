#pragma once

#include <stdint.h>
#include <wchar.h>

typedef uint8_t Color;

typedef enum {
  STYLE_NORMAL = 0,         // 0000
  STYLE_BOLD = 1,           // 0001
  STYLE_UNDERLINE = 1 << 1, // 0010
  STYLE_ITALIC = 1 << 2,    // 0100
  STYLE_BLINK = 1 << 3      // 1000
} TextStyle;

typedef struct {
  wchar_t *cell_char;
  Color fg;
  Color bg;
  TextStyle style;
} TUICell;

typedef struct {
  char *back_buffer;
  char *front_buffer;
  int cols;
  int rows;
} TUIData;

extern TUIData *tui;

char *tui_buffer_at(char *buffer, int col, int row, int cols, int rows);
void tui_get_center_point(int *col, int *row);

void tui_clear_screen();

void tui_enable_alternate_buffer();

void tui_disable_alternate_buffer();

void tui_get_terminal_size(int *rows, int *cols);

void tui_update_terminal_size();

void tui_hide_cursor();

void tui_show_cursor();

char *tui_create_buffer();

void tui_free(TUIData *tui_ctx);

void tui_enable_raw_mode();

void tui_disable_raw_mode();

void tui_draw_screen();

void tui_swap_buffer(char *buffer);

void tui_show_back_buffer();

void tui_init();
