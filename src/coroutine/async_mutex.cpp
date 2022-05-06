#include <yaclib/coroutine/async_mutex.hpp>
#include <yaclib/coroutine/coroutine.hpp>
#include <yaclib/coroutine/detail/promise_type.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/strand.hpp>
#include <yaclib/executor/thread_pool.hpp>
#include <yaclib/fault/atomic.hpp>

#include <iostream>  // for debug

namespace yaclib {

AsyncMutex::AsyncMutex() : _state(NotLocked()), _waiters(nullptr), _need_cs_batch(false) {
}

AsyncMutex::LockAwaiter AsyncMutex::Lock() {
  return LockAwaiter{*this};
}

bool AsyncMutex::TryLock() {
  void* old_state = _state.load(std::memory_order_acquire);
  if (old_state != NotLocked()) {
    return false;  // mutex is locked by another execution thread
  }
  return _state.compare_exchange_strong(old_state, LockedNoWaiters(), std::memory_order_acquire,
                                        std::memory_order_relaxed);
}

AsyncMutex::GuardAwaiter AsyncMutex::Guard() noexcept {
  return GuardAwaiter(*this);
}

AsyncMutex::LockAwaiter::LockAwaiter(AsyncMutex& mutex) : _mutex(mutex) {
}
}  // namespace yaclib
