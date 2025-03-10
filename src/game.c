#include "game.h"
#include "log.h"
#include "tui.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

const wchar_t SNAKE_CHAR = L'󱔎';
const wchar_t APPLE_CHAR = L'';

GameState *game_state;

void game_render_apples(TUIGrid *grid) {
  assert(game_state != NULL);
  assert(tui != NULL);

  for (int i = 0; i < game_state->apples_len; i++) {
    Apple apple = game_state->apples[i];
    tui_grid_cell_at(grid, apple.col, apple.row, tui->cols, tui->rows)
        ->cell_char = APPLE_CHAR;
  }
}

void game_render_snake(TUIGrid *grid) {
  assert(game_state != NULL);
  assert(tui != NULL);
  int len = game_state->snake_cells_len;
  if (len == 0)
    return;

  for (int i = 0; i < len; i++) {
    SnakeCell snake_cell = game_state->snake_cells[i];
    tui_grid_cell_at(grid, snake_cell.col, snake_cell.row, tui->cols, tui->rows)
        ->cell_char = SNAKE_CHAR;
  }
}

void game_render(TUIGrid *grid) {
  game_render_apples(grid);
  game_render_snake(grid);
}

SnakeCell *game_snake_create_cell() {
  SnakeCell *snake_cell = malloc(sizeof(SnakeCell));
  assert(snake_cell != NULL);

  return snake_cell;
}

void game_snake_cell_prepend(SnakeCell *cell) {
  assert(game_state != NULL);
  if (game_state->snake_cells_len == 0) {
    SnakeCell *snake_cells = malloc(sizeof(SnakeCell));
    assert(snake_cells != NULL);
    snake_cells[0] = *cell;
    game_state->snake_cells = snake_cells;
    game_state->snake_cells_len = 1;
    return;
  }

  int new_len = (game_state->snake_cells_len + 1);
  SnakeCell *new_snake_cells =
      realloc(game_state->snake_cells, new_len * sizeof(SnakeCell));

  memmove(new_snake_cells + 1, new_snake_cells, sizeof(SnakeCell));
  new_snake_cells[0] = *cell;
  free(game_state->snake_cells);
  game_state->snake_cells = new_snake_cells;
  game_state->snake_cells_len = new_len;
};

void game_apple_add(int col, int row) {
  assert(game_state != NULL);
  if (game_state->apples_len == 0) {
    Apple *apples = malloc(sizeof(Apple));
    apples[0].col = col;
    apples[0].row = row;
    game_state->apples = apples;
    game_state->apples_len = 1;
  }
}

void game_apple_spawn() {
  assert(tui != NULL);
  assert(game_state != NULL);

  int col = utils_random_range(0, tui->cols - 1);
  int row = utils_random_range(0, tui->rows - 1);
  log_message("[APPLE SPAWN]: col - %d, row - %d", col, row);

  game_apple_add(col, row);
}

void game_tick_update_apples() {
  assert(game_state != NULL);

  if (game_state->apples_len == 0) {
    game_apple_spawn();
    return;
  }
}

void game_tick_update_snake() {
  assert(game_state != NULL);

  Direction direction = game_state->snake_direction;

  switch (direction) {
  case TOPDIRECTION_TOP: {
    game_state->snake_cells[0].row--;
    break;
  }
  case DIRECTION_BOTTOM: {
    game_state->snake_cells[0].row++;
    break;
  }
  case DIRECTION_LEFT: {
    game_state->snake_cells[0].col++;
    break;
  }
  case DIRECTION_RIGHT: {
    game_state->snake_cells[0].col--;
    break;
  }
  default: {
    break;
  }
  }

  /* if (direction == 'l') { */
  /*   game_state->snake_cells[0].col++; */
  /* } */
  /* if (direction == 'b') { */
  /*   game_state->snake_cells[0].row++; */
  /* } */
}

void game_tick_update() {
  game_tick_update_snake();
  game_tick_update_apples();
}

void game_init_snake() {
  SnakeCell *cell = game_snake_create_cell();
  tui_get_center_point(&cell->col, &cell->row);
  game_snake_cell_prepend(cell);
  free(cell);
}

void game_init() {
  game_state = malloc(sizeof(GameState));
  assert(game_state != NULL);

  game_init_snake();
  game_apple_spawn();

  game_state->snake_direction = DIRECTION_LEFT;
}

void game_cleanup() {
  if (game_state != NULL)
    free(game_state);
}
