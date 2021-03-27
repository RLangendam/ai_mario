#pragma once

#include "game_facade.hpp"

class game {
 public:
  explicit game(game_facade& facade);

  void update();
  void run();
  std::uint16_t get_score() const;
  std::uint16_t get_coins() const;
  std::uint16_t get_timer() const;
  bool is_game_over() const;
  std::uint16_t get_lives() const;

 private:
  std::uint16_t read_decimal_position(offset offset) const;

 private:
  screen_buffer buffer;
  game_facade& facade;
};
