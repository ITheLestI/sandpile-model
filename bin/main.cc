#include <iostream>
#include "lib/ArgParser.h"
#include "lib/Sandpile.h"

int main(int argc, const char** argv) {
  Sandpile sandpile("input.tsv");
  Arguments args(argc, argv);
  sandpile.SpillSandpile(args);
  return 0;
}