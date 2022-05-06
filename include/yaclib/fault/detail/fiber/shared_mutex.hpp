#pragma once

#include <yaclib/fault/detail/fiber/queue.hpp>

namespace yaclib::detail::fiber {
class SharedMutex {
 public:
  SharedMutex() = default;
  ~SharedMutex() noexcept = default;

  SharedMutex(const SharedMutex&) = delete;
  SharedMutex& operator=(const SharedMutex&) = delete;

  void lock();
  bool try_lock() noexcept;
  void unlock() noexcept;

  void lock_shared();

  bool try_lock_shared();

  void unlock_shared();

 protected:
  void LockHelper();
  void SharedLockHelper();

  FiberQueue _shared_queue;

  uint32_t _shared_owners_count{0};
  FiberQueue _exclusive_queue;
  bool _occupied{false};
  bool _exclusive_mode{false};
};
}  // namespace yaclib::detail::fiber