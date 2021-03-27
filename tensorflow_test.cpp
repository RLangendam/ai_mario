#include "tensorflow.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Tensorflow version") {
  tensorflow tf;
  CHECK(tf.get_version() == "2.4.0");
}