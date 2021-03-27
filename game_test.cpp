#include "game.hpp"

#include <catch2/catch.hpp>
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4515)
#include <fakeit/catch/fakeit.hpp>
#pragma warning(pop)

namespace {
void set_relative_to_screen(screen_buffer& buffer, offset where,
                            unsigned char value) {
  buffer[where - offset::screen_begin] = value;
}
}  // namespace

SCENARIO("game should reflect the state from RAM") {
  using namespace fakeit;
  fakeit::Mock<game_facade> mock;

  When(Method(mock, read_screen))
      .Do([](auto& buffer) {
        set_relative_to_screen(buffer, offset::coins_tens, 4);
        set_relative_to_screen(buffer, offset::coins_ones, 2);
        set_relative_to_screen(buffer, offset::score_ten_thousands, 1);
        set_relative_to_screen(buffer, offset::score_thousands, 3);
        set_relative_to_screen(buffer, offset::score_hundreds, 5);
        set_relative_to_screen(buffer, offset::score_tens, 6);
        set_relative_to_screen(buffer, offset::score_ones, 7);
        set_relative_to_screen(buffer, offset::timer_hundreds, 9);
        set_relative_to_screen(buffer, offset::timer_tens, 8);
        set_relative_to_screen(buffer, offset::timer_ones, 0);
      })
      .Do([](auto& buffer) {
        set_relative_to_screen(buffer, offset::coins_tens, 44);
        set_relative_to_screen(buffer, offset::coins_ones, 1);
        set_relative_to_screen(buffer, offset::score_ten_thousands, 44);
        set_relative_to_screen(buffer, offset::score_thousands, 44);
        set_relative_to_screen(buffer, offset::score_hundreds, 44);
        set_relative_to_screen(buffer, offset::score_tens, 44);
        set_relative_to_screen(buffer, offset::score_ones, 44);
        set_relative_to_screen(buffer, offset::timer_hundreds, 44);
        set_relative_to_screen(buffer, offset::timer_tens, 8);
        set_relative_to_screen(buffer, offset::timer_ones, 5);
      });

  When(Method(mock, read_byte).Using(offset::lives)).Return(3).Return(1);
  When(Method(mock, read_byte).Using(offset::game_over)).Return(0x39).Return(0);

  game g{mock.get()};

  Verify(Method(mock, read_screen)).Never();
  Verify(Method(mock, read_byte)).Never();

  g.update();

  CHECK(g.get_coins() == 42);
  CHECK(g.get_score() == 13567);
  CHECK(g.get_timer() == 980);

  Verify(Method(mock, read_screen)).Once();
  Verify(Method(mock, read_byte)).Never();

  CHECK(g.get_lives() == 3);

  Verify(Method(mock, read_screen)).Once();
  Verify(Method(mock, read_byte).Using(offset::lives)).Once();
  Verify(Method(mock, read_byte).Using(offset::game_over)).Never();

  CHECK(g.is_game_over());

  Verify(Method(mock, read_screen)).Once();
  Verify(Method(mock, read_byte).Using(offset::lives)).Once();
  Verify(Method(mock, read_byte).Using(offset::game_over)).Once();

  g.update();

  CHECK(g.get_coins() == 1);
  CHECK(g.get_score() == 0);
  CHECK(g.get_timer() == 85);

  CHECK(g.get_lives() == 1);

  CHECK(!g.is_game_over());

  Verify(Method(mock, read_screen)).Twice();
  Verify(Method(mock, read_byte)).Exactly(4);
}