#pragma once

#include <yaclib/fault/detail/fiber/queue.hpp>

namespace yaclib::detail::fiber {

class RecursiveMutex {
 public:
  RecursiveMutex() = default;
  ~RecursiveMutex() noexcept = default;

  RecursiveMutex(const RecursiveMutex&) = delete;
  RecursiveMutex& operator=(const RecursiveMutex&) = delete;

  void lock();
  bool try_lock() noexcept;
  void unlock() noexcept;

 protected:
  void LockHelper();

  FiberQueue _queue;
  uint32_t _occupied_count{0};
  Fiber::Id _owner_id{0};
};

}  // namespace yaclib::detail::fiber
