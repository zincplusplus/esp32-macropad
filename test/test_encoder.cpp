// Host-only sanity check for the gray-code decode, no Arduino/PlatformIO needed:
//   g++ -std=c++17 -I src test/test_encoder.cpp -o /tmp/test_encoder && /tmp/test_encoder
#include <cassert>
#include "../src/encoder_direction.h"

int main() {
  assert(encoderDirection(0b00, 0b01) == 1);
  assert(encoderDirection(0b11, 0b10) == 1);
  assert(encoderDirection(0b00, 0b10) == -1);
  assert(encoderDirection(0b11, 0b01) == -1);
  assert(encoderDirection(0b00, 0b00) == 0);
  assert(encoderDirection(0b01, 0b11) == 0);
  return 0;
}
