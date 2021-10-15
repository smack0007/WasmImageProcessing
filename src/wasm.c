#include "image.h"

#define WASM_EXPORT(name) __attribute__((export_name(name)))

#define WASM_BUFFER_SIZE 800*500*4
byte wasmBuffer[WASM_BUFFER_SIZE];

void js_console_log(const char* string, int length);

unsigned long strlen(const char* string) {
  int length = 0;
  while (string[length] != '\0') {
    length++;
  }
  return length;
}

void console_log(const char* string) {
  js_console_log(string, strlen(string));
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
  console_log("Hello World!");
  process(width, height, wasmBuffer);    
}