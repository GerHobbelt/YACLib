#pragma once

#include <yaclib/coroutine/coroutine.hpp>
#include <yaclib/coroutine/detail/promise_type.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/strand.hpp>
#include <yaclib/executor/thread_pool.hpp>
#include <yaclib/fault/atomic.hpp>

#include <iostream>  // debug

/*
TODO
1. Guard
2. reorganise code
3. Compile-time parameter
4. try_lock
5. interfaces
*/

namespace yaclib {

class AsyncMutex;
class LockAwaiter;
class UnlockAwaiter;
class GuardAwaiter;

class AsyncMutex {
 public:
  AsyncMutex();
  LockAwaiter Lock();
  UnlockAwaiter Unlock(IExecutor& executor = CurrentThreadPool());
  // todo rename
  void SimpleUnlock();

 private:
  friend class LockAwaiter;
  friend class UnlockAwaiter;

  void* NotLocked() noexcept {
    return this;
  }

  void* LockedNoWaiters() noexcept {
    return nullptr;
  }

  yaclib_std::atomic<void*> _state;  // nullptr - lock, no waiters; this - no lock, otherwise - head of the list
  Job* _waiters;
};

class UnlockAwaiter {
 public:
  explicit UnlockAwaiter(AsyncMutex& mutex, IExecutor& executor) : _mutex(mutex), _executor(executor) {
  }

  bool await_ready() noexcept {
    // std::cout << "await_ready begin" << std::endl;
    if (_mutex._waiters != _mutex.LockedNoWaiters()) {
      return false;
    }

    void* old_state = _mutex.LockedNoWaiters();
    if (_mutex._state.compare_exchange_strong(old_state, _mutex.NotLocked(), std::memory_order_release,
                                              std::memory_order_relaxed)) {
      // made not locked
      return true;
    }

    // now we have a non-empty list of mutex' waiters

    Job* head = static_cast<Job*>(_mutex._state.exchange(_mutex.LockedNoWaiters()));

    // reverse

    Job* prev = nullptr;
    do {
      Job* tmp = static_cast<Job*>(head->next);
      head->next = prev;
      prev = head;
      head = tmp;
    } while (head != nullptr);

    YACLIB_ERROR(head == nullptr, "Broken invariant");

    _mutex._waiters = prev;

    // std::cout << "await_ready end" << std::endl;
    return false;
  }

  void await_resume() noexcept {
  }

  template <class V, class E>
  auto await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
    Job* next = _mutex._waiters;
    _mutex._waiters = static_cast<Job*>(next->next);

    Submit(_executor, [handle]() mutable {
      handle.resume();
    });
    // handle.resume();

    auto& promise = *static_cast<detail::PromiseType<void, int>*>(next);  // should it work properly?

    auto u = yaclib_std::coroutine_handle<detail::PromiseType<void, int>>::from_promise(promise);
    return u;
  }

 private:
  AsyncMutex& _mutex;
  IExecutor& _executor;
};

class LockAwaiter {
 public:
  explicit LockAwaiter(AsyncMutex& mutex);

  bool await_ready() noexcept {
    void* old_state = _mutex._state.load(std::memory_order_acquire);
    if (old_state != _mutex.NotLocked()) {
      return false;  // mutex is locked by another exec. thread
    }
    return _mutex._state.compare_exchange_strong(old_state, _mutex.LockedNoWaiters(), std::memory_order_acquire,
                                                 std::memory_order_relaxed);
  }
  void await_resume() {
  }

  template <class V, class E>
  bool await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
    Job* promise_ptr = &handle.promise();
    void* old_state = _mutex._state.load(std::memory_order_acquire);
    while (true) {
      if (old_state == _mutex.NotLocked()) {
        if (_mutex._state.compare_exchange_weak(old_state, _mutex.LockedNoWaiters(), std::memory_order_acquire,
                                                std::memory_order_relaxed)) {
          return false;  // continue
        }
      } else {
        // try to push on stack
        promise_ptr->next = reinterpret_cast<Job*>(old_state);
        if (_mutex._state.compare_exchange_weak(old_state, reinterpret_cast<void*>(promise_ptr),
                                                std::memory_order_release, std::memory_order_relaxed)) {
          return true;  // push on top of the stack
        }
      }
    }
  }

 protected:
  AsyncMutex& _mutex;
};

class GuardAwaiter : public LockAwaiter {
 public:
  explicit GuardAwaiter(AsyncMutex& mutex) : LockAwaiter(mutex), _owns(false) {
    _owns = true;
  }

  GuardAwaiter(GuardAwaiter&& oth) : _owns(oth._owns), LockAwaiter(std::move(oth)) {
    oth._owns = false;
  }

  GuardAwaiter await_resume() {
    return std::move(*this);
  }

  LockAwaiter Lock() {
    _owns = true;
    return *this;
  }

  UnlockAwaiter Unlock(IExecutor& executor = CurrentThreadPool()) {
    _owns = false;
    return UnlockAwaiter(_mutex, executor);
  }

  ~GuardAwaiter() {
    std::cout << "Inside dtor! Owns = " << std::boolalpha << _owns << std::endl;
    if (_owns) {
      Unlock();
      std::cout << "Unlock done" << std::endl;
    }
  }

 private:
  bool _owns;
};

}  // namespace yaclib
