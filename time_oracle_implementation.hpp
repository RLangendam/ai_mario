#pragma once

#include "time_oracle.hpp"

class time_oracle_implementation : public time_oracle {
  virtual std::chrono::time_point<std::chrono::steady_clock> now()
      const override;
};