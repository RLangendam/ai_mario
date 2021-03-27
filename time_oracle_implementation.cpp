#include "time_oracle_implementation.hpp"

std::chrono::time_point<std::chrono::steady_clock> time_oracle_implementation::now()
    const {
  return std::chrono::steady_clock::now();
}
