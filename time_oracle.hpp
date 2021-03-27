#pragma once

#include <chrono>

class time_oracle {
 public:
  virtual std::chrono::time_point<std::chrono::steady_clock> now() const = 0;
};