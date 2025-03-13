#include <stdint.h>
#include <stdlib.h>
#include <time.h>

int utils_random_range(int min, int max) {
  return min + rand() % (max - min + 1);
}

uint64_t utils_get_nanoseconds() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (uint64_t)ts.tv_sec * (uint64_t)1e9 + ts.tv_nsec;
}

uint64_t utils_get_microseconds() {
  return utils_get_nanoseconds() / (uint64_t)1e3;
}

uint64_t utils_get_milliseconds() {
  return utils_get_nanoseconds() / (uint64_t)1e6;
}
