#include <emscripten.h>
#include <stdlib.h>
#include "image.h"

EMSCRIPTEN_KEEPALIVE
byte* wasmAlloc(int width, int height) {
  return malloc(width * height * 4);
}

EMSCRIPTEN_KEEPALIVE
void wasmFree(byte* p) {
  free(p);
}

EMSCRIPTEN_KEEPALIVE
void wasmProcess(unsigned int width, unsigned int height, byte* buffer) {
  process(width, height, buffer);    
}