#include "image.h"

#define WASM_EXPORT(name) __attribute__((export_name(#name)))

#define WASM_BUFFER_SIZE 800*500*4
byte wasmBuffer[WASM_BUFFER_SIZE];

WASM_EXPORT(getBufferOffset)
byte* wasmGetBufferOffset() {
  return &wasmBuffer[0];
}

WASM_EXPORT(getBufferLength)
unsigned long wasmGetBufferLength() {
  return WASM_BUFFER_SIZE;
}

WASM_EXPORT(process)
void wasmProcess(unsigned int width, unsigned int height) {
  process(width, height, wasmBuffer);    
}