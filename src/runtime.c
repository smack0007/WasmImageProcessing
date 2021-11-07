#include "runtime.h"

unsigned long strlen(const char* string) {
  int length = 0;
  while (string[length] != '\0') {
    length++;
  }
  return length;
}