#ifndef STATE_H
#define STATE_H

typedef struct {
  int cols;
  int rows;
} SomeState;

void state_update_on_resize();

void state_update_tui_back_buffer();
void state_init();

#endif // !STATE_H
