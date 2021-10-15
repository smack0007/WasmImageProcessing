#include "image.h"

void process(unsigned int width, unsigned int height, byte* bytes) {
  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) { 
      unsigned long pixelOffset = (y * width * 4) + (x * 4);
      
      byte r = bytes[pixelOffset];
      byte g = bytes[pixelOffset + 1];
      byte b = bytes[pixelOffset + 2];

      byte gray = (byte)(((unsigned int)r + (unsigned int)g + (unsigned int)b) / 3);

      bytes[pixelOffset] = gray;
      bytes[pixelOffset + 1] = gray;
      bytes[pixelOffset + 2] = gray;
    }
  }
}
