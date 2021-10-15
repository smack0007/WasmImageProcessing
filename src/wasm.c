#include "image.h"

#define WASM_BUFFER_SIZE 800*500*4
byte wasmBuffer[WASM_BUFFER_SIZE];

byte* wasmGetBufferOffset() {
  return &wasmBuffer[0];
}

unsigned long wasmGetBufferLength() {
  return WASM_BUFFER_SIZE;
}

void wasmProcess(unsigned int width, unsigned int height) {
  process(width, height, wasmBuffer);    
}