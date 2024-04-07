#include "BMPWriter.h"

#include <cinttypes>
#include <fstream>

uint64_t Restrict(uint64_t value) {
  const size_t kNumColors = sizeof(kColorTable) / 4 - 1;
  if (value > kNumColors) {
    return kNumColors;
  }
  return value;
}

bool SaveBMP(const char* filename, const Sandpile& sandpile) {
  const uint16_t kBitsPerPixel = 4;
  const char kDummyBytes[4] = {'\x00'};
  const char kPaddingBytes[3] = {'\x00', '\x00', '\x00'};
  const uint32_t width = sandpile.GetWidth();
  const uint32_t height = sandpile.GetHeight();
  const uint32_t file_size = width * height + kDataOffset;
  const uint8_t padding = (32 - (width * kBitsPerPixel % 32)) / 8 % 4;

  std::ofstream bmp(filename, std::ios::binary);
  if (!bmp.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return false;
  }

  // Write header
  bmp.write("BM", 2);
  bmp.write(reinterpret_cast<const char*>(&file_size), 4);
  bmp.write(kDummyBytes, 4);
  bmp.write(reinterpret_cast<const char*>(&kDataOffset), 4);

  // Write Info Header

  // Size of info header
  const uint32_t kInfoHeaderSize = 40;
  bmp.write(reinterpret_cast<const char*>(&kInfoHeaderSize), 4);

  // img Width
  bmp.write(reinterpret_cast<const char*>(&width), 4);

  // img Height
  bmp.write(reinterpret_cast<const char*>(&height), 4);

  // Planes
  bmp.write("\x01\x00", 2);

  // Bits per pixel
  bmp.write(reinterpret_cast<const char*>(&kBitsPerPixel), 2);

  // Compression
  bmp.write(kDummyBytes, 4);

  // Image size
  bmp.write(kDummyBytes, 4);

  // Horizontal resolution
  bmp.write(kDummyBytes, 4);

  // Vertical resolution
  bmp.write(kDummyBytes, 4);

  // Number of colors
  const int32_t kColorsCount = sizeof(ColorTable) / 4;
  bmp.write(reinterpret_cast<const char*>(&kColorsCount), 4);

  // Number of important colors
  bmp.write(kDummyBytes, 4);

  // Color table
  bmp.write(reinterpret_cast<const char*>(&kColorTable), sizeof(kColorTable));

  uint8_t data;
  for (int32_t y = height - 1; y >= 0; --y) {
    for (int32_t x = 0; x < width; x += 2) {
      data = Restrict(sandpile.ConstGetPixel(x, y)) << 4;

      if (x + 1 < width) {
        data |= Restrict(sandpile.ConstGetPixel(x + 1, y));
      }
      bmp.write(reinterpret_cast<char*>(&data), 1);
    }
    bmp.write(kPaddingBytes, padding);
  }
  return true;
}
