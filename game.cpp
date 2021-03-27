#include "game.hpp"

#include <iostream>

game::game(game_facade& facade) : facade{facade} {}

void game::update() { facade.read_screen(buffer); }

void game::run() {
  update();
  std::cout << "Score: " << get_score() << " Coins: " << get_coins()
            << " Timer: " << get_timer() << " Game Over: " << std::boolalpha
            << is_game_over() << " Lives: " << get_lives() << "\n";
}

std::uint16_t game::get_score() const {
  std::uint16_t result{0};
  result += 10000 * read_decimal_position(offset::score_ten_thousands);
  result += 1000 * read_decimal_position(offset::score_thousands);
  result += 100 * read_decimal_position(offset::score_hundreds);
  result += 10 * read_decimal_position(offset::score_tens);
  result += read_decimal_position(offset::score_ones);
  return result;
}

std::uint16_t game::get_coins() const {
  std::uint16_t result{0};
  result += 10 * read_decimal_position(offset::coins_tens);
  result += read_decimal_position(offset::coins_ones);
  return result;
}

std::uint16_t game::get_timer() const {
  std::uint16_t result{0};
  result += 100 * read_decimal_position(offset::timer_hundreds);
  result += 10 * read_decimal_position(offset::timer_tens);
  result += read_decimal_position(offset::timer_ones);
  return result;
}

bool game::is_game_over() const {
  return 0x39 == facade.read_byte(offset::game_over);
}

std::uint16_t game::get_lives() const {
  return facade.read_byte(offset::lives);
}

std::uint16_t game::read_decimal_position(offset offset) const {
  auto result = buffer[offset - offset::screen_begin];
  if (result == 44) {
    result = 0;
  }
  return result;
}