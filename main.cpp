#include <thread>

#include "game.hpp"
#include "game_facade_implementation.hpp"
#include "keyboard_control.hpp"
#include "keyboard_facade_implementation.hpp"
#include "time_oracle_implementation.hpp"

int main() {
  using namespace std::chrono_literals;

  game_facade_implementation game_facade{"VisualBoyAdvance"};
  game g{game_facade};

  g.run();
  std::this_thread::sleep_for(5s);

  keyboard_facade_implementation keyboard_facade;
  time_oracle_implementation oracle;
  keyboard_control keyboard{keyboard_facade, oracle, 100ms};
  keyboard.press({'Q'});
  while (true) {
    keyboard.update();
    std::this_thread::sleep_for(1ms);
  }
  return 0;
}