#pragma once

#include <yaclib/fault/detail/fiber/queue.hpp>
#include <yaclib/fault/inject.hpp>

#include <mutex>

namespace yaclib::detail::fiber {

class Mutex {
 public:
  Mutex() = default;
  ~Mutex() noexcept = default;

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  void lock();
  bool try_lock() noexcept;
  void unlock() noexcept;

  using native_handle_type = std::mutex::native_handle_type;

  inline native_handle_type native_handle();

 private:
  FiberQueue _queue;
  bool _occupied{false};
};

}  // namespace yaclib::detail::fiber
