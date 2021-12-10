#include "image.h"

float32 threshold = 0.5f;

float32 brightness(byte r, byte g, byte b) {
  return (r / 255.0f) * 0.3f + (g / 255.0f) * 0.59f + (b / 255.0f) * 0.11f;
}

void process(uint32 width, uint32 height, byte* bytes) {  
  for (uint32 y = 0; y < height; y++) {
    for (uint32 x = 0; x < width; x++) {
      uint64 pixelOffset = (y * width * 4) + (x * 4);
      
      byte r = bytes[pixelOffset];
      byte g = bytes[pixelOffset + 1];
      byte b = bytes[pixelOffset + 2];
      
      float32 value = brightness(r, g, b);

      if (value >= threshold) {
        bytes[pixelOffset] = 255;
        bytes[pixelOffset + 1] = 255;
        bytes[pixelOffset + 2] = 255;
      } else {
        bytes[pixelOffset] = 0;
        bytes[pixelOffset + 1] = 0;
        bytes[pixelOffset + 2] = 0;
        bytes[pixelOffset + 3] = 255;
      }
    }
  }
}
