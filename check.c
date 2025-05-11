#include "genericc.h"
#include <stdio.h>

int main(void) {
  printf("SUPPORTS_VEC_INIT: %s\n", SUPPORTS_VEC_INIT ? "true" : "false");
  printf("SUPPORTS_VEC_FOREACH: %s\n", SUPPORTS_VEC_FOREACH ? "true" : "false");
  printf("SUPPORTS_VEC_FIND: %s\n", SUPPORTS_VEC_FIND ? "true" : "false");
  return 0;
}
