#include <catch2/catch.hpp>

#include "game.hpp"
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4515)
#include <fakeit/catch/fakeit.hpp>
#pragma warning(pop)

#include "keyboard_control.hpp"
#include "time_oracle.hpp"

SCENARIO(
    "keyboard_control should hold a single key during a given time frame") {
  using namespace fakeit;
  using namespace std::chrono_literals;

  Mock<time_oracle> oracle;
  Mock<keyboard_facade> facade;

  auto const present{std::chrono::steady_clock::now()};

  When(Method(oracle, now))
      .Return(present)
      .Return(present + 10ms)
      .Return(present + 20ms)
      .Return(present + 30ms)
      .Return(present + 40ms)
      .Return(present + 50ms);

  When(Method(facade, keys_down)).AlwaysReturn();
  When(Method(facade, keys_up)).AlwaysReturn();

  WHEN("Pressing a single key once") {
    keyboard_control keyboard{facade.get(), oracle.get(), 10ms};
    std::list<unsigned short> const keys_to_press{'A'};
    keyboard.press(keys_to_press);

    THEN("The key should be released after 10ms") {
      Verify(Method(facade, keys_down).Using(keys_to_press)).Once();
      Verify(Method(facade, keys_up)).Never();

      keyboard.update();

      Verify(Method(facade, keys_down)).Once();
      Verify(Method(facade, keys_up)).Never();

      keyboard.update();

      Verify(Method(facade, keys_down)).Once();
      Verify(Method(facade, keys_up).Using(keys_to_press)).Once();
    }
  }

  WHEN("Pressing a single key twice") {
    keyboard_control keyboard{facade.get(), oracle.get(), 10ms};
    std::list<unsigned short> const keys_to_press{'A'};
    keyboard.press(keys_to_press);
    keyboard.update();
    keyboard.update();
    keyboard.press(keys_to_press);
    keyboard.update();

    THEN("The key should be released after 10ms and pressed again") {
      Verify(Method(facade, keys_down).Using(keys_to_press)).Twice();
      Verify(Method(facade, keys_up).Using(keys_to_press)).Once();

      keyboard.update();
      Verify(Method(facade, keys_up).Using(keys_to_press)).Twice();
    }
  }

  WHEN("Pressing multiple keys, one of which twice") {
    keyboard_control keyboard{facade.get(), oracle.get(), 20ms};
    keyboard.press({'A', 'B'});
    keyboard.update();

    keyboard.press({'A'});
    keyboard.update();
    keyboard.update();

    THEN("The keys should be released, one after the other") {
      Verify(Method(facade, keys_down)).Once();
      Verify(Method(facade, keys_down).Using({'A', 'B'})).Once();

      Verify(Method(facade, keys_up)).Once();
      Verify(Method(facade, keys_up).Using({'B'})).Once();

      keyboard.update();

      Verify(Method(facade, keys_down)).Once();
      Verify(Method(facade, keys_up)).Twice();
      Verify(Method(facade, keys_up).Using({'A'})).Once();
    }
  }
}