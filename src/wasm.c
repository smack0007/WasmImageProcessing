#include "runtime.h"
#include "image.h"

#define WASM_EXPORT(name) __attribute__((export_name(name)))

#define WASM_BUFFER_SIZE 800*500*4
byte wasmBuffer[WASM_BUFFER_SIZE];

void jsLog(const char* string, int length);

void wasmLog(const char* string) {
  jsLog(string, strlen(string));
}

WASM_EXPORT("getBufferOffset")
byte* wasmGetBufferOffset() {
  return &wasmBuffer[0];
}

WASM_EXPORT("getBufferLength")
unsigned long wasmGetBufferLength() {
  return WASM_BUFFER_SIZE;
}

WASM_EXPORT("process")
void wasmProcess(unsigned int width, unsigned int height) {
  wasmLog("Hello World!");
  process(width, height, wasmBuffer);    
}