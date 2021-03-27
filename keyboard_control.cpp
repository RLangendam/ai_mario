#include "keyboard_control.hpp"

keyboard_control::keyboard_control(keyboard_facade& facade) : facade{facade} {}

void keyboard_control::press(std::list<unsigned short> keys) {
  using namespace std::chrono_literals;
  std::list<unsigned short> keys_to_press;
  for (auto key : keys) {
    auto found{presses.find(key)};
    if (found == presses.end()) {
      keys_to_press.emplace_back(key);
      presses.emplace(key, std::chrono::steady_clock::now() + 100ms);
    } else {
      found->second = std::chrono::steady_clock::now() + 100ms;
    }
  }
  facade.keys_down(keys_to_press);
}

void keyboard_control::update() {
  auto const now{std::chrono::steady_clock::now()};
  std::list<unsigned short> keys_to_release;
  for (auto& [key, time_point] : presses) {
    if (now > time_point) {
      keys_to_release.emplace_back(key);
    }
  }
  facade.keys_up(keys_to_release);
  for (auto key : keys_to_release) {
    presses.erase(key);
  }
}
