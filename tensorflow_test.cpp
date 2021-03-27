#include "tensorflow_test.hpp"

#pragma warning(push)
#pragma warning(disable : 4190)
#include <tensorflow/c/c_api.h>
#pragma warning(pop)

#include <iostream>

void tensorflow_test() {
  std::cout << "Tensorflow version: " << TF_Version() << '\n';
}