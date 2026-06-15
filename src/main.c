#include "elf64.h"

#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <output>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "wb");
  if (!file) {
    fprintf(stderr, "Failed to open %s.", argv[1]);
    return 1;
  }

  uint32_t text[] = {
    0xD2800BA8,
    0xD2800540,
    0xD4000001
  };

  write_elf64(file, text, sizeof(text));

  fclose(file);

  return 0;
}
