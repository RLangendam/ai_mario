#pragma once

#include <memory>

#include "game_facade.hpp"

namespace detail {
    class game_facade_impl;
}

class game_facade_implementation : public game_facade {
 public:
  explicit game_facade_implementation(char const* name);
  virtual ~game_facade_implementation() override;

 private:
  virtual void read_screen(screen_buffer& buffer) const override;
  virtual unsigned char read_byte(offset where) const override;

  std::unique_ptr<detail::game_facade_impl> impl;
};
