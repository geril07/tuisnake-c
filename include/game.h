#pragma once

#include "tui.h"
#include <stdbool.h>
#include <time.h>
typedef struct {
  int row;
  int col;
} SnakeCell;

typedef struct {
  SnakeCell *cells;
  int *cells_count;
} Snake;

typedef struct {
  int row;
  int col;
} Apple;

typedef struct {
  Apple *apples;
  int apples_count;
} Food;

// extern Snake *snake;
// extern Food *food;
typedef enum {
  DIRECTION_TOP,
  DIRECTION_BOTTOM,
  DIRECTION_RIGHT,
  DIRECTION_LEFT
} Direction;

typedef struct {
  Apple *apples;
  int apples_len;
  time_t last_apple_spawn;

  SnakeCell *snake_cells;
  int snake_cells_len;
  Direction snake_direction;

  bool is_end;
} GameState;

extern GameState *game_state;

void game_init();
void game_cleanup();
void game_render(TUIGrid *grid);
void game_tick_update();
