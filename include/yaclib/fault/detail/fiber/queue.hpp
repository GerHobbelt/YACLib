#pragma once
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/scheduler.hpp>

#include <vector>

namespace yaclib::detail::fiber {

struct NoTimeoutTag {};

class FiberQueue {
 public:
  FiberQueue() = default;
  FiberQueue(FiberQueue&& other) = default;
  FiberQueue& operator=(FiberQueue&& other) noexcept;

  bool Wait(NoTimeoutTag);

  template <typename Rep, typename Period>
  bool Wait(const std::chrono::duration<Rep, Period>& duration) {
    auto* fiber = fault::Scheduler::Current();
    auto* queue_node = dynamic_cast<BiNodeWaitQueue*>(fiber);
    _queue.PushBack(queue_node);
    auto* scheduler = fault::Scheduler::GetScheduler();
    auto time = scheduler->SleepFor(duration);
    if (scheduler->_sleep_list.find(time) != scheduler->_sleep_list.end()) {
      scheduler->_sleep_list[time].Erase(dynamic_cast<BiNodeSleep*>(fiber));
      if (scheduler->_sleep_list[time].Empty()) {
        scheduler->_sleep_list.erase(time);
      }
    }
    bool res = _queue.Erase(queue_node);
    return res;
  }

  template <typename Clock, typename Duration>
  bool Wait(const std::chrono::time_point<Clock, Duration>& time_point) {
    std::chrono::time_point<Clock, Duration> now = Clock::now();
    Duration duration = time_point - now;
    return Wait(duration);
  }

  void NotifyAll();

  void NotifyOne();

  bool Empty();

  ~FiberQueue();

 private:
  BiList _queue;
};
}  // namespace yaclib::detail::fiber
