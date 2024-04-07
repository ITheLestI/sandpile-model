#pragma once
#include <cinttypes>

class Arguments {
public:
  const char* input_filepath;
  const char* output_folderpath = "./output/";
  uint64_t max_iter = 100000000;
  uint64_t frequency = 100;
  bool is_error = false;

  Arguments(int argc, const char** argv);
  bool IsValid();
};
