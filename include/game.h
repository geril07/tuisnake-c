#ifndef GAME_H
#define GAME_H

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

typedef struct {
  Apple *apples;
  int apples_len;
  time_t last_apple_spawn;

  SnakeCell *snake_cells;
  int snake_cells_len;
  char snake_direction;
} GameState;

void game_init();
void game_cleanup();
void game_render(char *buffer);
void game_tick_update();

#endif // !GAME_H
