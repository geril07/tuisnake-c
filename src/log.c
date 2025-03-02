#include <stdarg.h>
#include <stdio.h>

static FILE *log_file = NULL;

void log_init() {
  log_file = fopen("log.txt", "a");

  if (log_file == NULL) {
    perror("Error opening log file");
  }
}

void log_message(const char *format, ...) {
  if (log_file == NULL)
    return; // Ensure stream is set

  va_list args;
  va_start(args, format);
  vfprintf(log_file, format, args);
  va_end(args);

  fprintf(log_file, "\n");

  fflush(log_file);
}
