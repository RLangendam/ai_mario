#pragma once

#include <memory>

#include "keyboard_facade.hpp"
#include "window_handle.hpp"

class keyboard_facade_implementation : public keyboard_facade {
 public:
  explicit keyboard_facade_implementation(
      std::shared_ptr<window_handle>&& handle);

 private:
  virtual void keys_down(std::list<unsigned short> keys) override;
  virtual void keys_up(std::list<unsigned short> keys) override;

 private:
  std::shared_ptr<window_handle> handle;
};
