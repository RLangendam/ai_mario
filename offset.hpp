#pragma once 

enum class offset : uintptr_t {
  screen_begin = 0x9800,
  score_ten_thousands = 0x9821,
  score_thousands = 0x9822,
  score_hundreds = 0x9823,
  score_tens = 0x9824,
  score_ones = 0x9825,
  coins_tens = 0x9829,
  coins_ones = 0x982A,
  timer_hundreds = 0x9831,
  timer_tens = 0x9832,
  timer_ones = 0x9833,
  screen_end = 0x9A3A, // 0x9BFF
  game_over = 0xC0A4,
  lives = 0xDA15
};

constexpr std::size_t operator-(offset left, offset right) {
  return static_cast<uintptr_t>(left) - static_cast<uintptr_t>(right);
}