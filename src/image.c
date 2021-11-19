#include "image.h"

void process(uint32 width, uint32 height, byte* bytes) {  
  for (uint32 y = 0; y < height; y++) {
    for (uint32 x = 0; x < width; x++) { 
      uint64 pixelOffset = (y * width * 4) + (x * 4);
      
      byte r = bytes[pixelOffset];
      byte g = bytes[pixelOffset + 1];
      byte b = bytes[pixelOffset + 2];

      byte gray = (byte)(((uint32)r + (uint32)g + (uint32)b) / 3);

      bytes[pixelOffset] = gray;
      bytes[pixelOffset + 1] = gray;
      bytes[pixelOffset + 2] = gray;
    }
  }
}
