#pragma once

#include <memory>

#include "game_facade.hpp"

namespace detail {
class process_information;
class game_facade_impl;
}  // namespace detail

class game_facade_implementation : public game_facade {
 public:
  explicit game_facade_implementation(char const* application_name,
                                      char const* rom_name);
  virtual ~game_facade_implementation() override;

  virtual void read_screen(screen_buffer& buffer) const override;
  virtual unsigned char read_byte(offset where) const override;
  virtual std::shared_ptr<window_handle> get_window_handle() override;

 private:
  std::unique_ptr<detail::game_facade_impl> impl;
};
