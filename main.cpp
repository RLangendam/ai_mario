#include <windows.h>

#include <iostream>
#include <thread>

#include "game.hpp"
#include "game_facade_implementation.hpp"
#include "keyboard_control.hpp"
#include "keyboard_facade_implementation.hpp"
#include "print_error.hpp"
#include "tensorflow.hpp"
#include "time_oracle_implementation.hpp"

int main() {
  tensorflow tf;
  std::cout << "Tensorflow version: " << tf.get_version() << '\n';

  using namespace std::chrono_literals;

  startup("D:\\gameboy\\VisualBoyAdvance.exe", "VisualBoyAdvance.exe D:\\gameboy\\mario1.gb");
  
  game_facade_implementation game_facade{"VisualBoyAdvance"};
  game g{game_facade};

  g.run();
  // std::this_thread::sleep_for(5s);

  // keyboard_facade_implementation keyboard_facade;
  // time_oracle_implementation oracle;
  // keyboard_control keyboard{keyboard_facade, oracle, 100ms};

  // keyboard.press({'Z'});
  // while (true) {
  // keyboard.update();
  // std::this_thread::sleep_for(1ms);
  // }
  return 0;
}