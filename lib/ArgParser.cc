#include "ArgParser.h"

#include <charconv>
#include <cinttypes>
#include <cstring>
#include <iostream>
#include <optional>

bool Arguments::IsValid() { return !is_error; }

std::optional<uint64_t> ParseInt(const char* str) {
  uint64_t result;
  auto [ptr, ec] = std::from_chars(str, str + strlen(str), result);
  if (ec != std::errc()) {
    return std::nullopt;
  }
  return result;
}

Arguments::Arguments(int argc, const char** argv) {
  struct {
    bool has_input_filepath = false;
    bool has_output_filepath = false;
    bool has_frequency = false;
    bool has_max_iter = false;
  } args_status;

  int i = 1;
  while (i < argc) {
    if (strncmp(argv[i], "--input=", strlen("--input=")) == 0 ||
        (strcmp(argv[i], "-i") == 0 && i + 1 < argc)) {
      if (argv[i][1] == '-') {
        input_filepath = argv[i] + strlen("--input=");
      } else {
        input_filepath = argv[++i];
      }
      args_status.has_input_filepath = true;
      ++i;
      continue;
    }

    if (strncmp(argv[i], "--output=", strlen("--output=")) == 0 ||
        (strcmp(argv[i], "-o") == 0 && i + 1 < argc)) {
      if (argv[i][1] == '-') {
        output_folderpath = argv[i] + strlen("--output=");
      } else {
        output_folderpath = argv[++i];
      }
      args_status.has_output_filepath = true;
      ++i;
      continue;
    }

    if (strncmp(argv[i], "--freq=", strlen("--freq=")) == 0 ||
        (strcmp(argv[i], "-f") == 0 && i + 1 < argc)) {
      const char* start = argv[i] + strlen("--freq=");
      if (argv[i][1] != '-') {
        start = argv[++i];
      }
      std::optional<uint64_t> result = ParseInt(start);
      if (result) {
        frequency = result.value();
      } else {
        std::cerr << "Incorrect max-iter value" << std::endl;
        is_error = true;
        break;
      }
      args_status.has_frequency = true;
      ++i;
      continue;
    }

    if (strncmp(argv[i], "--max-iter=", strlen("--max-iter=")) == 0 ||
        (strcmp(argv[i], "-m") == 0 && i + 1 < argc)) {
      const char* start = argv[i] + strlen("--max-iter=");
      if (argv[i][1] != '-') {
        start = argv[++i];
      }
      std::optional<uint64_t> result = ParseInt(start);
      if (result) {
        max_iter = result.value();
      } else {
        std::cerr << "Incorrect max-iter value" << std::endl;
        is_error = true;
        break;
      }
      args_status.has_max_iter = true;
      ++i;
      continue;
    }
    ++i;
  }

  if (!args_status.has_input_filepath) {
    std::cerr << "Input file is not specified" << std::endl;
  }
  if (!args_status.has_max_iter) {
    std::cerr << "Max-iter is not specified" << std::endl;
  }
}
