#pragma once
#include <cstdint>

// Pure gray-code decode: given the previous and current 2-bit A/B reading,
// return +1 (clockwise step), -1 (counter-clockwise step), or 0 (no full step yet).
inline int8_t encoderDirection(uint8_t prevAB, uint8_t ab) {
  if ((prevAB == 0b00 && ab == 0b01) || (prevAB == 0b11 && ab == 0b10)) return 1;
  if ((prevAB == 0b00 && ab == 0b10) || (prevAB == 0b11 && ab == 0b01)) return -1;
  return 0;
}
