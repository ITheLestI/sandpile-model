#pragma once
#include <cinttypes>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

#include "BMPWriter.h"
#include "Queue.h"
#include "ArgParser.h"

struct Point {
  int16_t x;
  int16_t y;
  Point(int16_t x, int16_t y) : x(x), y(y) {}
};

class Sandpile {
  static const int32_t kExtendAmount = 10;
  static constexpr const char* kFilenameTemplate = "iteration";
  bool has_created_out_folder_ = false;

  int32_t displayed_width_ = 1;
  int32_t displayed_height_ = 1;
  int32_t displayed_x0_ = 0;
  int32_t displayed_y0_ = 0;

  int32_t real_width_ = 1;
  int32_t real_height_ = 1;

  uint64_t** sandpile_ = nullptr;
  uint64_t iteration_ = 0;
  Queue queue_;

  void ExtendUpwards();
  void ExtendDownwards();
  void ExtendLeftwards();
  void ExtendRightwards();
  bool ResizeForPixel(int32_t x1, int32_t y1);
public:
  void RealPrintToConsole();
  Sandpile() = delete;
  Sandpile(const char* filename);
  void SetFromTSV(const char* filename);
  bool WriteToBMP(const char* folderpath);
  void FillQueue();
  void SpillSandpile(const Arguments& args);
  uint64_t ConstGetPixel(int32_t x, int32_t y) const;
  uint32_t GetWidth() const;
  uint32_t GetHeight() const;
  ~Sandpile();
  Sandpile(const Sandpile& src) = delete;
  Sandpile& operator=(const Sandpile& src) = delete;
};
