#pragma once

#include <yaclib/exe/job.hpp>

namespace yaclib::detail {

class PCore : public Job {
 public:
  enum State : char {
    kEmpty = 0,
    kResult = 1,
    kCall = 2,
    kHereCall = 3,
    kHereWrap = 4,
    kWaitDetach = 5,
    kWaitStop = 6,
    kWait = 7,
  };

  void Call() noexcept override;
  void Drop() noexcept override;

  virtual void Here(PCore& caller, State state) noexcept;
};

}  // namespace yaclib::detail
