#pragma once

#include <array>

#include "offset.hpp"

using screen_buffer =
    std::array<unsigned char, offset::screen_end - offset::screen_begin>;

class game_facade {
 public:
  virtual ~game_facade() = default;
  virtual void read_screen(screen_buffer& buffer) const = 0;
  virtual unsigned char read_byte(offset where) const = 0;
};
