#pragma once

#include <yaclib/fault/detail/timed_mutex.hpp>

namespace yaclib::detail {

// Not using because maybe in future we will want different types
template <typename Impl>
class RecursiveTimedMutex : public RecursiveMutex<Impl> {
  using Base = RecursiveMutex<Impl>;

 public:
  using Base::Base;

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration) {
    YACLIB_INJECT_FAULT(auto r = Impl::try_lock_for(timeout_duration));
    return r;
  }

  template <typename Clock, typename Duration>
  bool try_lock_until(const std::chrono::duration<Clock, Duration>& timeout_time) {
    YACLIB_INJECT_FAULT(auto r = Impl::try_lock_until(timeout_time));
    return r;
  }
};

}  // namespace yaclib::detail
