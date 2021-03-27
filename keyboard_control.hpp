#pragma once

#include <chrono>
#include <unordered_map>

#include "keyboard_facade.hpp"

class keyboard_control {
 public:
  explicit keyboard_control(keyboard_facade& facade);
  void press(std::list<unsigned short> keys);
  void update();

 private:
  std::unordered_map<unsigned short,
                     std::chrono::time_point<std::chrono::steady_clock>>
      presses;
  keyboard_facade& facade;
};
