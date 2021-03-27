#pragma once

#include "keyboard_facade.hpp"

class keyboard_facade_implementation : public keyboard_facade {
 public:
  virtual void keys_down(std::list<unsigned short> keys) override;
  virtual void keys_up(std::list<unsigned short> keys) override;
};
