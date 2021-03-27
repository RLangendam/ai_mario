cmake_minimum_required(VERSION 3.14)

set(HUNTER_PACKAGES Catch FakeIt)

include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)

project(ai_mario)

find_package(Catch2 CONFIG REQUIRED)
find_package(FakeIt CONFIG REQUIRED)

add_executable(ai_mario "main.cpp")

add_library(
  ai_mario_lib
  "offset.hpp"
  "game_facade_implementation.cpp"
  "game_facade_implementation.hpp"
  "game_facade.hpp"
  "game.hpp"
  "game.cpp"
  "keyboard_control.hpp"
  "keyboard_control.cpp"
  "keyboard_facade.hpp"
  "keyboard_facade_implementation.hpp"
  "keyboard_facade_implementation.cpp")

add_executable(ai_mario_tests "test_main.cpp" "game_test.cpp")

target_link_libraries(ai_mario ai_mario_lib)
target_link_libraries(ai_mario_tests Catch2::Catch2 FakeIt::FakeIt ai_mario_lib)

target_compile_features(ai_mario_lib PRIVATE cxx_std_20)
target_compile_features(ai_mario PRIVATE cxx_std_20)
target_compile_features(ai_mario_tests PRIVATE cxx_std_14)

if(MSVC)
  target_compile_options(ai_mario_lib PRIVATE /W4 /WX)
  target_compile_options(ai_mario PRIVATE /W4 /WX)
  target_compile_options(ai_mario_tests PRIVATE /W4 /WX)
else()
  target_compile_options(ai_mario_lib PRIVATE -Wall -Wextra -pedantic -Werror)
  target_compile_options(ai_mario PRIVATE -Wall -Wextra -pedantic -Werror)
  target_compile_options(ai_mario_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

include(CTest)
include(Catch)
catch_discover_tests(ai_mario_tests)
