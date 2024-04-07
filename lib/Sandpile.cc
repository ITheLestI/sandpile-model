#include "Sandpile.h"

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>

#include "BMPWriter.h"

void PrintToConsole(const Sandpile& sandpile) {
  for (uint32_t y = 0; y < sandpile.GetHeight(); ++y) {
    for (uint32_t x = 0; x < sandpile.GetWidth(); ++x) {
      uint64_t pixel = sandpile.ConstGetPixel(x, y);
      if (pixel == 0) {
        std::cout << "0 ";
      } else {
        std::cout << pixel << " ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void Sandpile::RealPrintToConsole() {
  for (int32_t y = 0; y < real_height_; ++y) {
    for (int32_t x = 0; x < real_width_; ++x) {
      uint64_t pixel = sandpile_[y][x];
      if (pixel == 0) {
        if (x >= displayed_x0_ && x < displayed_x0_ + displayed_width_ &&
            y >= displayed_y0_ && y < displayed_y0_ + displayed_height_) {
          std::cout << "0 ";
        } else {
          std::cout << ". ";
        }
      } else {
        std::cout << pixel << " ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

Sandpile::Sandpile(const char* filename) {
  SetFromTSV(filename);
  PrintToConsole(*this);
}

void Sandpile::SetFromTSV(const char* filename) {
  std::ifstream tsv(filename);
  if (!tsv.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return;
  }
  int32_t min_x = std::numeric_limits<uint16_t>::max();
  int32_t min_y = std::numeric_limits<uint16_t>::max();
  int32_t max_x = -min_x;
  int32_t max_y = -min_y;
  // Calculate width and height
  while (!tsv.eof()) {
    int32_t x, y;
    uint64_t sand;
    tsv >> x >> y >> sand;
    min_x = std::min(min_x, x);
    min_y = std::min(min_y, y);
    max_x = std::max(max_x, x);
    max_y = std::max(max_y, y);
  }
  real_height_ = abs(max_y - min_y) + 1;
  displayed_height_ = real_height_;
  real_width_ = abs(max_x - min_x) + 1;
  displayed_width_ = real_width_;
  if (real_width_ == 1) ++real_width_;
  if (real_height_ == 1) ++real_height_;

  sandpile_ = new uint64_t*[real_height_];
  for (int32_t i = 0; i < real_height_; ++i) {
    sandpile_[i] = new uint64_t[real_width_];
    std::fill(sandpile_[i], sandpile_[i] + real_width_, 0);
  }

  tsv.clear();
  tsv.seekg(0, std::ios::beg);
  while (!tsv.eof()) {
    int32_t x, y;
    uint64_t sand;
    tsv >> x >> y >> sand;
    sandpile_[max_y - y][x - min_x] = sand;
    if (sand >= 4) {
      queue_.PushBack(new Point(x - min_x, max_y - y));
    }
  }
}

bool Sandpile::WriteToBMP(const char* foldername) {
  namespace fs = std::filesystem;
  fs::path path(foldername);
  if (fs::exists(path) && fs::is_directory(path)) {
    has_created_out_folder_ = true;
    fs::current_path(path);
  }
  if (!has_created_out_folder_) {
    std::cout << "Folder " << fs::absolute(path) << " doesn't exist, creating."
              << std::endl;
    fs::create_directory(path);
    fs::current_path(path);
  }
  char filename[256];
  sprintf(filename, "%s%lld.bmp", kFilenameTemplate, iteration_);
  #if DEBUG
    std::cout << "Writing to " << filename << std::endl;
  #endif
  return SaveBMP(filename, *this);
}

uint64_t Sandpile::ConstGetPixel(int32_t x, int32_t y) const {
  if (displayed_x0_ + x >= real_width_ || displayed_y0_ + y >= real_height_ ||
      displayed_x0_ + x < 0 || displayed_y0_ + y < 0) {
    std::cerr << "Index out of bounds for x=" << x << ", y=" << y << std::endl;
    return 0;
  }
  return sandpile_[displayed_y0_ + y][displayed_x0_ + x];
}

void Sandpile::ExtendRightwards() {
  uint64_t** new_sandpile = new uint64_t*[real_height_];
  real_width_ += kExtendAmount;
  for (int32_t i = 0; i < real_height_; ++i) {
    new_sandpile[i] = new uint64_t[real_width_];
    std::fill(new_sandpile[i], new_sandpile[i] + real_width_, 0);
    for (int k = 0; k < displayed_x0_ + displayed_width_; ++k) {
      new_sandpile[i][k] = sandpile_[i][k];
    }
    delete[] sandpile_[i];
  }
  delete[] sandpile_;
  sandpile_ = new_sandpile;
}

void Sandpile::ExtendLeftwards() {
  uint64_t** new_sandpile = new uint64_t*[real_height_];
  real_width_ += kExtendAmount;
  for (int32_t i = 0; i < real_height_; ++i) {
    new_sandpile[i] = new uint64_t[real_width_];
    std::fill(new_sandpile[i], new_sandpile[i] + real_width_, 0);
    std::copy(sandpile_[i], sandpile_[i] + displayed_width_,
              new_sandpile[i] + kExtendAmount);
    delete[] sandpile_[i];
  }
  delete[] sandpile_;
  sandpile_ = new_sandpile;
  displayed_x0_ += kExtendAmount;
}

void Sandpile::ExtendDownwards() {
  uint64_t** new_sandpile = new uint64_t*[real_height_ + kExtendAmount];
  real_height_ += kExtendAmount;
  for (int32_t i = 0; i < real_height_; ++i) {
    new_sandpile[i] = new uint64_t[real_width_];
    std::fill(new_sandpile[i], new_sandpile[i] + real_width_, 0);
    if (i < real_height_ - kExtendAmount) {
      std::copy(sandpile_[i], sandpile_[i] + real_width_, new_sandpile[i]);
      delete[] sandpile_[i];
    }
  }
  delete[] sandpile_;
  sandpile_ = new_sandpile;
}

void Sandpile::ExtendUpwards() {
  uint64_t** new_sandpile = new uint64_t*[real_height_ + kExtendAmount];
  real_height_ += kExtendAmount;
  for (int32_t i = 0; i < kExtendAmount; ++i) {
    new_sandpile[i] = new uint64_t[real_width_];
    std::fill(new_sandpile[i], new_sandpile[i] + real_width_, 0);
  }
  for (int32_t i = kExtendAmount; i < real_height_; ++i) {
    new_sandpile[i] = new uint64_t[real_width_];
    std::copy(sandpile_[i - kExtendAmount],
              sandpile_[i - kExtendAmount] + real_width_, new_sandpile[i]);
    delete[] sandpile_[i - kExtendAmount];
  }
  delete[] sandpile_;
  sandpile_ = new_sandpile;
  displayed_y0_ += kExtendAmount;
}

bool Sandpile::ResizeForPixel(int32_t x1, int32_t y1) {
  bool was_basis_change = false;
  if (x1 < 0) {
    ExtendLeftwards();
    #if DEBUG
      std::cout << "LeftExtend" << std::endl;
    #endif
  }
  if (x1 >= real_width_) {
    ExtendRightwards();
    #if DEBUG
      std::cout << "RightExtend" << std::endl;
    #endif
  }
  if (y1 < 0) {
    ExtendUpwards();
    #if DEBUG
      std::cout << "UpExtend" << std::endl;
    #endif
  }
  if (y1 >= real_height_) {
    ExtendDownwards();
    #if DEBUG
      std::cout << "DownExtend" << std::endl;
    #endif
  }
  if (x1 >= displayed_width_ + displayed_x0_) {
    displayed_width_ += 1;
  }
  if (y1 >= displayed_height_ + displayed_y0_) {
    displayed_height_ += 1;
  }
  if (x1 < displayed_x0_) {
    displayed_x0_ -= 1;
    was_basis_change = true;
    displayed_width_ += 1;
  }
  if (y1 < displayed_y0_) {
    displayed_y0_ -= 1;
    was_basis_change = true;
    displayed_height_ += 1;
  }
  return was_basis_change;
}

uint32_t Sandpile::GetWidth() const {
  return static_cast<uint32_t>(displayed_width_);
}

uint32_t Sandpile::GetHeight() const {
  return static_cast<uint32_t>(displayed_height_);
}

void Sandpile::FillQueue() {
  for (int32_t y = 0; y < displayed_height_; ++y) {
    for (int32_t x = 0; x < displayed_width_; ++x) {
      if (ConstGetPixel(x, y) >= 4) {
        queue_.PushBack(new Point(x, y));
      }
    }
  }
}

void Sandpile::SpillSandpile(const Arguments& args) {
  while (!queue_.IsEmpty() && iteration_ < args.max_iter) {
    int32_t offset_x = 0;
    int32_t offset_y = 0;
    while (!queue_.IsEmpty()) {
      Point* point = queue_.PopFront();

      point->x += offset_x;
      point->y += offset_y;
      #if DEBUG
        std::cout << "Point: " << point->x << " " << point->y << std::endl;
        RealPrintToConsole();
      #endif
      sandpile_[displayed_y0_ + point->y][displayed_x0_ + point->x] -= 4;

      // top
      if (ResizeForPixel(displayed_x0_ + point->x,
                         displayed_x0_ + point->y - 1)) {
        offset_y++;
        point->y++;
      }
      sandpile_[displayed_y0_ + point->y - 1][displayed_x0_ + point->x]++;
      #if DEBUG
        RealPrintToConsole();
      #endif

      // bottom
      ResizeForPixel(displayed_x0_ + point->x, displayed_y0_ + point->y + 1);
      sandpile_[displayed_y0_ + point->y + 1][displayed_x0_ + point->x]++;
      #if DEBUG
        RealPrintToConsole();
      #endif

      // left
      if (ResizeForPixel(displayed_x0_ + point->x - 1,
                         displayed_y0_ + point->y)) {
        offset_x++;
        point->x++;
      }
      sandpile_[displayed_y0_ + point->y][displayed_x0_ + point->x - 1]++;
      #if DEBUG
        RealPrintToConsole();
      #endif

      // right
      ResizeForPixel(displayed_x0_ + point->x + 1, displayed_y0_ + point->y);
      sandpile_[displayed_y0_ + point->y][displayed_x0_ + point->x + 1]++;
      #if DEBUG
        RealPrintToConsole();
      #endif

      delete point;
    }

    ++iteration_;
    FillQueue();
    if (args.frequency != 0 && iteration_ % args.frequency == 0) {
      #if DEBUG
        PrintToConsole(*this);
      #endif
      if (!WriteToBMP(args.output_folderpath)) break;
    }
  }
  WriteToBMP(args.output_folderpath);
}

Sandpile::~Sandpile() {
  for (int32_t i = 0; i < real_height_; ++i) {
    delete[] sandpile_[i];
  }
  delete[] sandpile_;
}
