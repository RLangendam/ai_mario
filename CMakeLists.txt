cmake_minimum_required(VERSION 3.14)

set(HUNTER_PACKAGES Catch FakeIt)

include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)

function(download_extract_file url filename hash_type hash dir)

  if(NOT EXISTS ${dir}/${filename})
    file(
      DOWNLOAD ${url} ${dir}/${filename}
      TIMEOUT 60 # seconds
      EXPECTED_HASH ${hash_type}=${hash}
      TLS_VERIFY ON)

    file(ARCHIVE_EXTRACT INPUT ${dir}/${filename} DESTINATION ${dir})
  endif()

endfunction(download_extract_file)

project(ai_mario)

find_package(Catch2 CONFIG REQUIRED)
find_package(FakeIt CONFIG REQUIRED)

set(TENSORFLOW_DIR ${CMAKE_CURRENT_LIST_DIR}/tensorflow)
set(TENSORFLOW_ARCHIVE_NAME_WINDOWS libtensorflow-gpu-windows-x86_64-2.4.0.zip)
set(TENSORFLOW_ARCHIVE_NAME_LINUX libtensorflow-gpu-linux-x86_64-2.4.0.tar.gz)

download_extract_file(
  https://storage.googleapis.com/tensorflow/libtensorflow/${TENSORFLOW_ARCHIVE_NAME_WINDOWS}
  ${TENSORFLOW_ARCHIVE_NAME_WINDOWS}
  SHA1
  228365a5f5907c6c5f909040dcbabe348af96612
  ${TENSORFLOW_DIR})

download_extract_file(
  https://storage.googleapis.com/tensorflow/libtensorflow/${TENSORFLOW_ARCHIVE_NAME_LINUX}
  ${TENSORFLOW_ARCHIVE_NAME_LINUX}
  SHA1
  d233bb8fad7be392152b23d64aa13f6d03e37e20
  ${TENSORFLOW_DIR})

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
  "keyboard_facade_implementation.cpp"
  "tensorflow.hpp"
  "tensorflow.cpp"
  "time_oracle.hpp"
  "time_oracle_implementation.hpp"
  "time_oracle_implementation.cpp")

add_executable(ai_mario_tests "test_main.cpp" "game_test.cpp"
                              "keyboard_control_test.cpp" "tensorflow_test.cpp")

target_include_directories(ai_mario_lib PRIVATE ${TENSORFLOW_DIR}/include)
target_link_directories(ai_mario PRIVATE ${TENSORFLOW_DIR}/lib)
target_link_directories(ai_mario_lib PRIVATE ${TENSORFLOW_DIR}/lib)
target_link_directories(ai_mario_tests PRIVATE ${TENSORFLOW_DIR}/lib)

target_link_libraries(ai_mario ai_mario_lib)
target_link_libraries(ai_mario_lib tensorflow.lib)
target_link_libraries(ai_mario_tests Catch2::Catch2 FakeIt::FakeIt ai_mario_lib)

target_compile_features(ai_mario_lib PRIVATE cxx_std_17)
target_compile_features(ai_mario PRIVATE cxx_std_17)
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

add_custom_command(
  TARGET ai_mario
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${TENSORFLOW_DIR}/lib/tensorflow.dll" $<TARGET_FILE_DIR:ai_mario>)

include(CTest)
include(Catch)
catch_discover_tests(ai_mario_tests)
