#include "game.h"
#include "input.h"
#include "tui.h"
#include "utils.h"
#include <assert.h>
#include <bits/time.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

const wchar_t SNAKE_CHAR = 's';
const wchar_t APPLE_CHAR = '@';

const int SNAKE_SPEED = 25;
const double SNAKE_Y_SPEED_RATIO = 3;
const int DIRECTION_Y = DIRECTION_BOTTOM | DIRECTION_TOP;
const int DIRECTION_X = DIRECTION_LEFT | DIRECTION_RIGHT;

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
  if (len == 0)
    return;

  for (int i = 0; i < len; i++) {
    SnakeCell snake_cell = game_state->snake_cells[i];

    tui_grid_cell_at(grid, snake_cell.col, snake_cell.row, grid->cols,
                     grid->rows)
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

  // borders excluded
  int col = utils_random_range(1, tui->cols - 2);
  int row = utils_random_range(1, tui->rows - 2);

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

bool game_snake_is_opposite_direction(SnakeDirection first,
                                      SnakeDirection second) {
  switch (first) {
  case DIRECTION_RIGHT: {
    return second == DIRECTION_LEFT;
  }
  case DIRECTION_LEFT: {
    return second == DIRECTION_RIGHT;
  }
  case DIRECTION_TOP: {
    return second == DIRECTION_BOTTOM;
  }
  case DIRECTION_BOTTOM: {
    return second == DIRECTION_TOP;
  }
  case DIRECTION_NONE:
    break;
  }
  return false;
}

void game_process_input() {
  InputEvent *input_event = input_get_event();
  if (input_event == NULL)
    return;

  assert(game_state != NULL);

  SnakeDirection cur_direction = game_state->snake_direction;
  SnakeDirection gs_next_direction = game_state->next_snake_direction;

  SnakeDirection next_direction = DIRECTION_NONE;

  switch (input_event->key) {
  case INPUT_A: {
    next_direction = DIRECTION_LEFT;
    break;
  }
  case INPUT_D: {
    next_direction = DIRECTION_RIGHT;
    break;
  }
  case INPUT_S: {
    next_direction = DIRECTION_BOTTOM;
    break;
  }
  case INPUT_W: {
    next_direction = DIRECTION_TOP;
    break;
  }
  case INPUT_NONE:
    break;
  }
  free(input_event);

  if (next_direction == gs_next_direction)
    return;
  if (game_snake_is_opposite_direction(cur_direction, next_direction))
    return;

  game_state->next_snake_direction = next_direction;
}

bool game_tick_check_border_collision(SnakeCell *head) {
  assert(tui != NULL);
  assert(head != NULL);

  int last_col = tui->cols - 1;
  int last_row = tui->rows - 1;

  if (head->col >= last_col || head->row >= last_row)
    return true;

  if (head->col <= 0 || head->row <= 0)
    return true;

  return false;
}

bool game_tick_snake_check_collision(SnakeCell *head) {
  assert(game_state != NULL);

  for (int i = 0; i < game_state->snake_cells_len - 1; i++) {
    SnakeCell cell = game_state->snake_cells[i];
    if (cell.col == head->col && cell.row == head->row)
      return true;
  }

  return false;
}

void game_tick_snake_moved() {
  uint64_t millis = utils_get_milliseconds();
  game_state->last_snake_update = millis;
}

bool game_tick_should_update_snake() {
  bool is_y_direction = game_state->snake_direction & DIRECTION_Y;
  uint64_t d_time = utils_get_milliseconds() - game_state->last_snake_update;
  int d_speed = d_time / SNAKE_SPEED;
  if (is_y_direction)
    d_speed /= SNAKE_Y_SPEED_RATIO;

  return d_speed >= 1;
}

void game_tick_update_snake() {
  assert(game_state != NULL);
  assert(game_state->snake_cells_len != 0);
  assert(game_state->snake_cells != NULL);

  if (!game_tick_should_update_snake())
    return;

  if (game_state->next_snake_direction != DIRECTION_NONE)
    game_state->snake_direction = game_state->next_snake_direction;

  SnakeDirection direction = game_state->snake_direction;

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

  case DIRECTION_RIGHT: {
    new_head->col++;
    break;
  }

  case DIRECTION_LEFT: {
    new_head->col--;
    break;
  }
  case DIRECTION_NONE:
    break;
  }
  bool is_border_collision = game_tick_check_border_collision(new_head);
  bool is_snake_collision = game_tick_snake_check_collision(new_head);
  bool is_end = is_border_collision || is_snake_collision;

  if (is_end) {
    game_state->is_end = true;
    return;
  }

  Apple *current_apple = game_is_apple_exists_on_cell(new_head);

  if (current_apple != NULL) {
    game_snake_cell_prepend(new_head);
    game_remove_apple(current_apple);
  } else {
    game_snake_update_cells(new_head);
  }
  game_tick_snake_moved();
}

void game_tick_update() {
  assert(game_state != NULL);
  if (game_state->is_end)
    return;

  game_process_input();
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
  /* game_apple_add((tui->cols / 2) + 30, tui->rows / 2); */
  /* game_apple_add((tui->cols / 2) + 40, tui->rows / 2); */
  /* game_apple_add((tui->cols / 2) + 50, tui->rows / 2); */
  /* game_apple_add((tui->cols / 2) + 60, tui->rows / 2); */

  game_state->snake_direction = DIRECTION_LEFT;
  game_state->next_snake_direction = game_state->snake_direction;
  ;
}

void game_cleanup() {
  if (game_state != NULL)
    free(game_state);
}
