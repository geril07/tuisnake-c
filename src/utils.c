#include <stdlib.h>

int utils_random_range(int min, int max) {
  return min + rand() % (max - min + 1);
}
