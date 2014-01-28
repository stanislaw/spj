#include <stdio.h>
#include <string.h>

#include "spj.h"

int main() {

  char *jsonbytes = "{\"Here some\":\"JSON bytes\"}";
  int datasize = strlen(jsonbytes);

  spj_parse(jsonbytes, datasize);

  return 0;
}
