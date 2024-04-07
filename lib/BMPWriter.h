#pragma once
#include <cinttypes>
#include <fstream>

#include "Sandpile.h"

const struct ColorTable {
  const uint8_t white[4] = {255, 255, 255, 0};
  const uint8_t green[4] = {0, 255, 0, 0};
  const uint8_t yellow[4] = {0, 255, 255, 0};
  const uint8_t magenta[4] = {255, 0, 255, 0};
  const uint8_t black[4] = {0, 0, 0, 0};
} kColorTable;

const uint32_t kHeaderSize = 14;
const uint32_t kInfoHeaderSize = 40;
const uint32_t kDataOffset = kHeaderSize + kInfoHeaderSize + sizeof(kColorTable);

class Sandpile;

bool SaveBMP(const char* filename, const Sandpile& sandpile);
