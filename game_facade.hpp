#pragma once

#include <array>
#include <memory>

#include "offset.hpp"
#include "window_handle.hpp"
using screen_buffer =
    std::array<unsigned char, offset::screen_end - offset::screen_begin>;

class game_facade {
 public:
  virtual ~game_facade() = default;
  virtual void read_screen(screen_buffer& buffer) const = 0;
  virtual unsigned char read_byte(offset where) const = 0;
  virtual std::shared_ptr<window_handle> get_window_handle() = 0;
};
