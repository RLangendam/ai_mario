#include <catch2/catch.hpp>

#include "game.hpp"
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4515)
#include <fakeit/catch/fakeit.hpp>
#pragma warning(pop)

#include "keyboard_control.hpp"
#include "time_oracle.hpp"

SCENARIO("keyboard_control should hold keys for given time frames") {
  using namespace fakeit;
  using namespace std::chrono_literals;

  Mock<time_oracle> oracle;
  Mock<keyboard_facade> facade;

  auto present{std::chrono::steady_clock::now()};

  When(Method(oracle, now))
      .Return(present)
      .Return(present + 10ms)
      .Return(present + 20ms);

  keyboard_control keyboard{facade.get(), oracle.get(), 20ms};

  //keyboard.press({'A'});


}