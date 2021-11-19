#include <emscripten.h>
#include <stdlib.h>
#include "image.h"

EMSCRIPTEN_KEEPALIVE
byte* wasmAlloc(uint32 width, uint32 height) {
  return malloc(width * height * 4);
}

EMSCRIPTEN_KEEPALIVE
void wasmFree(byte* p) {
  free(p);
}

EMSCRIPTEN_KEEPALIVE
void wasmProcess(uint32 width, uint32 height, byte* buffer) {
  process(width, height, buffer);    
}