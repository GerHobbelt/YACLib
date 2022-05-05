#pragma once

#include <yaclib/fault/detail/fiber/default_allocator.hpp>
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/stack.hpp>
#include <yaclib/fault/detail/fiber/stack_allocator.hpp>
#include <yaclib/log.hpp>
#include <yaclib/util/func.hpp>

#include <chrono>
#include <list>
#include <map>
#include <random>
#include <vector>

namespace yaclib::detail::fiber {

class Scheduler {
 public:
  friend class FiberQueue;
  friend class Thread;

  Scheduler();

  [[nodiscard]] bool IsRunning() const;

  template <class Clock, class Duration>
  auto SleepUntil(const std::chrono::time_point<Clock, Duration>& sleep_time) {
    std::chrono::time_point<Clock, Duration> now = Clock::now();
    Duration duration = sleep_time - now;
    return SleepFor(duration);
  }

  template <class Rep, class Period>
  auto SleepFor(const std::chrono::duration<Rep, Period>& sleep_duration) {
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(sleep_duration).count();
    if (us <= 0) {
      return us;
    }
    auto time = GetTimeUs() + us;
    auto* current_fiber = Current();
    BiList& sleep_list = _sleep_list[GetTimeUs() + us];
    sleep_list.PushBack(dynamic_cast<BiNodeSleep*>(current_fiber));
    Suspend();
    return time;
  }

  [[nodiscard]] uint64_t GetTimeUs() const;

  static Fiber* Current();

  static Fiber::Id GetId();

  static void RescheduleCurrent();

  void Stop();

  static Scheduler* GetScheduler();

  static void Set(Scheduler* scheduler);

 private:
  static void Suspend();

  void Schedule(Fiber* fiber);

  void AdvanceTime();

  void TickTime();

  Fiber* GetNext();

  void RunLoop();

  void WakeUpNeeded();
  uint64_t _time;
  std::vector<Fiber*> _queue;
  std::map<uint64_t, BiList> _sleep_list;
  bool _running;
};

Fiber* PollRandomElementFromList(std::vector<Fiber*>& list);

BiNode* PollRandomElementFromList(BiList& list);

}  // namespace yaclib::detail::fiber
