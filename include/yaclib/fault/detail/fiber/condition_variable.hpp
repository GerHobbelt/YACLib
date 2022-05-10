#pragma once

#include <yaclib/fault/detail/fiber/mutex.hpp>
#include <yaclib/fault/injector.hpp>

#include <condition_variable>

namespace yaclib::detail::fiber {

class ConditionVariable {
 public:
  ConditionVariable() noexcept = default;

  ~ConditionVariable() = default;

  ConditionVariable(const ConditionVariable&) = delete;
  ConditionVariable& operator=(const ConditionVariable&) = delete;

  void notify_one() noexcept;
  void notify_all() noexcept;

  void wait(std::unique_lock<yaclib::detail::fiber::Mutex>& lock) noexcept;

  template <typename Predicate>
  void wait(std::unique_lock<yaclib::detail::fiber::Mutex>& lock, Predicate predicate) {
    while (!predicate()) {
      wait(lock);
    }
  }

  template <typename Clock, typename Duration>
  std::cv_status wait_until(std::unique_lock<yaclib::detail::fiber::Mutex>& lock,
                            const std::chrono::time_point<Clock, Duration>& time_point) {
    GetInjector()->SetPauseInject(true);
    lock.unlock();
    bool timeout = _queue.Wait(time_point);
    lock.lock();
    GetInjector()->SetPauseInject(false);
    return timeout ? std::cv_status::timeout : std::cv_status::no_timeout;
  }

  template <typename Clock, typename Duration, typename Predicate>
  bool wait_until(std::unique_lock<yaclib::detail::fiber::Mutex>& lock,
                  const std::chrono::time_point<Clock, Duration>& time_point, Predicate predicate) {
    wait_until(lock, time_point);
    return predicate();
  }

  template <typename Rep, typename Period>
  std::cv_status wait_for(std::unique_lock<yaclib::detail::fiber::Mutex>& lock,
                          const std::chrono::duration<Rep, Period>& duration) {
    GetInjector()->SetPauseInject(true);
    lock.unlock();
    bool timeout = _queue.Wait(duration);
    lock.lock();
    GetInjector()->SetPauseInject(false);
    return timeout ? std::cv_status::timeout : std::cv_status::no_timeout;
  }

  template <typename Rep, typename Period, typename Predicate>
  bool wait_for(std::unique_lock<yaclib::detail::fiber::Mutex>& lock,
                const std::chrono::duration<Rep, Period>& duration, Predicate predicate) {
    wait_for(lock, duration);
    return predicate();
  }

  using native_handle_type = std::condition_variable::native_handle_type;

  native_handle_type native_handle();

 private:
  FiberQueue _queue;
};

}  // namespace yaclib::detail::fiber
