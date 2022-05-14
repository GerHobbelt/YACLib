#pragma once

#include <yaclib/fault/detail/fiber/default_allocator.hpp>
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/stack.hpp>
#include <yaclib/fault/detail/fiber/stack_allocator.hpp>
#include <yaclib/fault/inject.hpp>
#include <yaclib/log.hpp>
#include <yaclib/util/func.hpp>

#include <chrono>
#include <map>

namespace yaclib::detail::fiber {
class FiberQueue;
}  // namespace yaclib::detail::fiber

namespace yaclib::fault {

class Scheduler {
 public:
  friend class detail::fiber::FiberQueue;

  Scheduler();

  void Schedule(detail::fiber::Fiber* fiber);

  [[nodiscard]] bool IsRunning() const;

  template <typename Clock, typename Duration>
  auto SleepUntil(const std::chrono::time_point<Clock, Duration>& sleep_time) {
    std::chrono::time_point<Clock, Duration> now = Clock::now();
    Duration duration = sleep_time - now;
    return SleepFor(duration);
  }

  template <typename Rep, typename Period>
  auto SleepFor(const std::chrono::duration<Rep, Period>& sleep_duration) {
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_duration).count();
    if (ns <= 0) {
      return ns;
    }
    auto time = GetTimeNs() + ns;
    auto* current_fiber = Current();
    detail::fiber::BiList& sleep_list = _sleep_list[GetTimeNs() + ns];
    sleep_list.PushBack(static_cast<detail::fiber::BiNodeSleep*>(current_fiber));
    Suspend();
    return time;
  }

  [[nodiscard]] uint64_t GetTimeNs() const;

  static detail::fiber::Fiber* Current();

  static detail::fiber::Fiber::Id GetId();

  static void RescheduleCurrent();

  void Stop();

  static Scheduler* GetScheduler();

  static void Set(Scheduler* scheduler);

 private:
  static void Suspend();

  void AdvanceTime();

  void TickTime();

  detail::fiber::Fiber* GetNext();

  void RunLoop();

  void WakeUpNeeded();

  uint64_t _time;
  detail::fiber::BiList _queue;
  std::map<uint64_t, detail::fiber::BiList> _sleep_list;
  bool _running;
};

}  // namespace yaclib::fault

namespace yaclib::detail::fiber {
BiNode* PollRandomElementFromList(BiList& list);
}  // namespace yaclib::detail::fiber
