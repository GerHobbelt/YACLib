#pragma once
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/scheduler.hpp>
#include <yaclib/fault/detail/fiber/steady_clock.hpp>

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
    return Wait(duration + SteadyClock::now());
  }

  template <typename Clock, typename Duration>
  bool Wait(const std::chrono::time_point<Clock, Duration>& time_point) {
    auto* fiber = fault::Scheduler::Current();
    auto* queue_node = static_cast<BiNodeWaitQueue*>(fiber);
    _queue.PushBack(queue_node);
    auto* scheduler = fault::Scheduler::GetScheduler();
    auto time = scheduler->Sleep(time_point.time_since_epoch().count());
    if (scheduler->_sleep_list.find(time) != scheduler->_sleep_list.end()) {
      scheduler->_sleep_list[time].Erase(static_cast<BiNodeSleep*>(fiber));
      if (scheduler->_sleep_list[time].Empty()) {
        scheduler->_sleep_list.erase(time);
      }
    }
    bool res = _queue.Erase(queue_node);
    return res;
  }

  void NotifyAll();

  void NotifyOne();

  bool Empty();

  ~FiberQueue();

 private:
  BiList _queue;
};
}  // namespace yaclib::detail::fiber
