#include "libs/chip8/Chip8.hpp"

int main(int argc, char ** argv) {
  Chip8 chip8;
  chip8.start(argv[1]);
  return 0;
}
