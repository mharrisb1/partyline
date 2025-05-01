#include <stdio.h>
#include <stdlib.h>

#include "partyline.h"

int main(void) {
  char *line;

  while ((line = partyline("ping> ")) != NULL) {
    printf("pong> %s\n", line);
    free(line);
  }

  return 0;
}
