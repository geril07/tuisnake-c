#include "game.h"
#include "log.h"
#include "tui.h"
#include "utils.h"
#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

const wchar_t SNAKE_CHAR = L'󱔎';
const wchar_t APPLE_CHAR = L'';

GameState *game_state;

void game_render_apples(TUIGrid *grid) {
  assert(game_state != NULL);
  assert(tui != NULL);

  int len = game_state->apples_len;
  if (len == 0)
    return;

  for (int i = 0; i < len; i++) {
    Apple apple = game_state->apples[i];
    tui_grid_cell_at(grid, apple.col, apple.row, tui->cols, tui->rows)
        ->cell_char = APPLE_CHAR;
  }
}

void game_render_snake(TUIGrid *grid) {
  assert(game_state != NULL);
  assert(tui != NULL);
  int len = game_state->snake_cells_len;
  log_message("game_render_snake: before, %d", len);
  if (len == 0)
    return;

  for (int i = 0; i < len; i++) {
    SnakeCell snake_cell = game_state->snake_cells[i];

    tui_grid_cell_at(grid, snake_cell.col, snake_cell.row, grid->cols,
                     grid->rows)
        ->cell_char = SNAKE_CHAR;
  }
  log_message("game_render_snake: after");
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

  int new_len = game_state->snake_cells_len + 1;
  SnakeCell *new_snake_cells =
      realloc(game_state->snake_cells, new_len * sizeof(SnakeCell));

  memmove(new_snake_cells + 1, new_snake_cells,
          game_state->snake_cells_len * sizeof(SnakeCell));

  new_snake_cells[0] = *cell;
  game_state->snake_cells = new_snake_cells;
  game_state->snake_cells_len = new_len;
};

void game_apple_add(int col, int row) {
  assert(game_state != NULL);

  if (game_state->apples_len == 0) {
    Apple *apples = malloc(sizeof(Apple));
    assert(apples != NULL);
    apples[0].col = col;
    apples[0].row = row;
    game_state->apples = apples;
    game_state->apples_len = 1;
    return;
  }

  Apple *apples = game_state->apples;
  int new_apples_len = game_state->apples_len + 1;

  Apple *new_apples = realloc(apples, new_apples_len * sizeof(Apple));
  assert(new_apples != NULL);
  new_apples[new_apples_len - 1].col = col;
  new_apples[new_apples_len - 1].row = row;
  game_state->apples = new_apples;
  game_state->apples_len = new_apples_len;
}

void game_apple_spawn() {
  assert(tui != NULL);
  assert(game_state != NULL);

  int col = utils_random_range(0, tui->cols - 1);
  int row = utils_random_range(0, tui->rows - 1);
  /* int col = (tui->cols / 2) + 10; */
  /* int row = (tui->rows / 2); */
  /* log_message("[APPLE SPAWN]: col - %d, row - %d", col, row); */

  game_apple_add(col, row);
}

void game_tick_update_apples() {
  assert(game_state != NULL);

  if (game_state->apples_len == 0) {
    game_apple_spawn();
    return;
  }
}

Apple *game_is_apple_exists_on_cell(SnakeCell *cell) {
  if (game_state->apples_len == 0)
    return NULL;

  Apple *apples = game_state->apples;
  int apples_len = game_state->apples_len;

  for (int i = 0; i < apples_len; i++) {
    Apple *apple = &apples[i];
    if (cell->col == apple->col && cell->row == apple->row)
      return apple;
  }

  return NULL;
}

void game_remove_apple(Apple *filtered_apple_ptr) {

  Apple *apples = game_state->apples;
  int apples_len = game_state->apples_len;

  int new_size = 0;
  Apple *new_apples = malloc(apples_len * sizeof(Apple));

  for (int i = 0; i < apples_len; i++) {

    if (&apples[i] == filtered_apple_ptr) {
      continue;
    }

    new_apples[new_size++] = apples[i];
  }

  Apple *new_apples_r = realloc(new_apples, new_size * sizeof(Apple));
  if (new_apples_r == NULL && new_size > 0) {
    free(apples);
    game_state->apples = NULL;
    game_state->apples_len = 0;
    return;
  }

  free(apples);

  game_state->apples = new_apples_r;
  game_state->apples_len = new_size;
}

void game_snake_update_cells(SnakeCell *new_head) {
  /* return; */
  assert(game_state != NULL);
  assert(new_head != NULL);

  int snake_len = game_state->snake_cells_len;
  SnakeCell *cells = game_state->snake_cells;

  for (int i = snake_len - 1; i > 0; i--) {
    cells[i] = cells[i - 1];
  }

  cells[0] = *new_head;
}

bool game_tick_check_border(SnakeCell *cell) {
  assert(tui != NULL);
  assert(cell != NULL);

  int last_col = tui->cols - 1;
  int last_row = tui->rows - 1;

  if (cell->col >= last_col || cell->row >= last_row) {
    return true;
  }

  return false;
}

void game_tick_update_snake() {
  assert(game_state != NULL);
  assert(game_state->snake_cells_len != 0);
  assert(game_state->snake_cells != NULL);

  Direction direction = game_state->snake_direction;
  SnakeCell *old_head = &game_state->snake_cells[0];
  SnakeCell *new_head = malloc(sizeof(Apple)); // game_state->snake_cells[0];
  memcpy(new_head, old_head, sizeof(Apple));

  switch (direction) {
  case DIRECTION_TOP: {
    new_head->row--;
    break;
  }

  case DIRECTION_BOTTOM: {
    new_head->row++;
    break;
  }

  case DIRECTION_LEFT: {
    new_head->col++;
    break;
  }

  case DIRECTION_RIGHT: {
    new_head->col--;
    break;
  }
  }
  bool is_end = game_tick_check_border(new_head);

  if (is_end) {
    game_state->is_end = true;
    return;
  }

  Apple *current_apple = game_is_apple_exists_on_cell(new_head);

  if (current_apple != NULL) {
    log_message("BEFORE APPLE");
    game_snake_cell_prepend(new_head);
    game_remove_apple(current_apple);
    log_message("AFTER APPLE");
  } else {
    game_snake_update_cells(new_head);
  }
}

void game_tick_update() {
  assert(game_state != NULL);
  if (game_state->is_end)
    return;

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
  /* game_apple_spawn(); */
  game_apple_add((tui->cols / 2) + 30, tui->rows / 2);
  game_apple_add((tui->cols / 2) + 40, tui->rows / 2);
  game_apple_add((tui->cols / 2) + 50, tui->rows / 2);
  game_apple_add((tui->cols / 2) + 60, tui->rows / 2);

  game_state->snake_direction = DIRECTION_LEFT;
}

void game_cleanup() {
  if (game_state != NULL)
    free(game_state);
}
