#include <Windows.h>

#include <iostream>
#include <thread>

#include "game.hpp"
#include "game_facade_implementation.hpp"
#include "keyboard_control.hpp"
#include "keyboard_facade_implementation.hpp"
#include "tensorflow.hpp"
#include "time_oracle_implementation.hpp"

int main() {
  tensorflow tf;
  std::cout << "Tensorflow version: " << tf.get_version() << '\n';

  using namespace std::chrono_literals;

  game_facade_implementation game_facade{"VisualBoyAdvance.exe",
                                         "VisualBoyAdvance.exe mario1.gb"};
  game g{game_facade};

  keyboard_facade_implementation keyboard_facade{
      game_facade.get_window_handle()};
  time_oracle_implementation oracle;
  keyboard_control keyboard{keyboard_facade, oracle, 10ms};

  keyboard.press({VK_RETURN});

  std::this_thread::sleep_for(1s);

  g.run();
  std::this_thread::sleep_for(1s);

  for (int i{0}; i < 10; ++i) {
    keyboard.press({'Z'});
    keyboard.update();
    std::this_thread::sleep_for(100ms);
  }
  return 0;
}
