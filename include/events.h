#pragma once

void events_handle_sigint(int sig);
void events_handel_sigwinch(int sig) {};
void events_handle_exit();
void events_handle_resize();
