#include <stdlib.h>

// Use buffer to format string and write to specified file descriptor
#define fdprintf(fd, b, s, ...)      \
  sprintf(buffer, s, ##__VA_ARGS__); \
  write(fd, b, strlen(b));

enum {
  NCOUNTERS,  // Number of counters
  BLOCKS,     // Count of all memory blocks
  MALLOCS,    // Number of successful calls to malloc
  FREES,      // Number of successful calls to free
};

extern size_t Counters[NCOUNTERS];  // array of counters

void init_counters();
void dump_counters();
