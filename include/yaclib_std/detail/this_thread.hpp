#pragma once

#if YACLIB_FAULT_THIS_THREAD == 2  // TODO(myannyax) Implement
#  include <yaclib/fault/detail/fiber/scheduler.hpp>

namespace yaclib_std::this_thread {

template <class Clock, class Duration>
inline void sleep_until(const std::chrono::time_point<Clock, Duration>& sleep_time) {
  yaclib::detail::fiber::Scheduler::GetScheduler()->SleepUntil(sleep_time);
}

template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration) {
  yaclib::detail::fiber::Scheduler::GetScheduler()->SleepFor(sleep_duration);
}

inline constexpr auto* yield = &yaclib::detail::fiber::Scheduler::RescheduleCurrent;

inline constexpr auto* get_id = &yaclib::detail::fiber::Scheduler::GetId;

}  // namespace yaclib_std::this_thread
//#elif YACLIB_FAULT_THIS_THREAD == 1  // TODO(myannyax) Maybe implement
//#  error "YACLIB_FAULT=THREAD not implemented yet"
#else
#  include <thread>

namespace yaclib_std::this_thread {

using std::this_thread::sleep_for;

using std::this_thread::sleep_until;

using std::this_thread::yield;

using std::this_thread::get_id;

}  // namespace yaclib_std::this_thread
#endif
