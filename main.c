#include <stdio.h>
#include <string.h>

#include "spj.h"

int main() {
  printf("We can now iterate the whole string:\n");

  char *jsonbytes = "{\"Here some\":\"JSON bytes\"}";
  int datasize = strlen(jsonbytes);

  spj_parse(jsonbytes, datasize);

  printf("\n");

  return 0;
}
