#pragma once
#include "tui.h"
#include <stdbool.h>
#include <stdint.h>
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
  DIRECTION_NONE = 0,
  DIRECTION_TOP = 1 << 1,
  DIRECTION_BOTTOM = 1 << 2,
  DIRECTION_RIGHT = 1 << 3,
  DIRECTION_LEFT = 1 << 4
} SnakeDirection;

typedef struct {
  Apple *apples;
  int apples_len;
  time_t last_apple_spawn;

  SnakeCell *snake_cells;
  int snake_cells_len;
  SnakeDirection snake_direction;
  SnakeDirection next_snake_direction;
  uint64_t last_snake_update;

  bool is_end;
} GameState;

extern GameState *game_state;

void game_init();
void game_cleanup();
void game_render(TUIGrid *grid);
void game_tick_update();
