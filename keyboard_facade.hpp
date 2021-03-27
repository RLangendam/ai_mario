#pragma once

#include <list>

class keyboard_facade {
 public:
  virtual void keys_down(std::list<unsigned short> keys) = 0;
  virtual void keys_up(std::list<unsigned short> keys) = 0;
};
