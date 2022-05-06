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
* interfaces
*/

namespace yaclib {

class AsyncMutex;
class LockAwaiter;
template <bool Fifo>
class UnlockAwaiterBase;
class GuardAwaiter;

class AsyncMutex {
 public:
  enum class UnlockType { Default, ContinueSameThread, CriticalSectionSameThread };

 private:
  class UnlockAwaiterBase {
   public:
    explicit UnlockAwaiterBase(AsyncMutex& mutex, IExecutor& executor) : _mutex(mutex), _executor(executor) {
    }

    void await_resume() {
    }

   protected:
    bool AwaitReadyHelper(bool fifo) noexcept {
      if (_mutex._waiters != _mutex.LockedNoWaiters()) {
        return false;
      }

      bool old_need_batch = std::exchange(_mutex._need_cs_batch, false);

      void* old_state = _mutex.LockedNoWaiters();
      if (_mutex._state.compare_exchange_strong(old_state, _mutex.NotLocked(), std::memory_order_release,
                                                std::memory_order_relaxed)) {
        return true;
      }

      Job* head = static_cast<Job*>(_mutex._state.exchange(_mutex.LockedNoWaiters()));

      if (fifo) {
        Job* prev = nullptr;
        do {
          Job* tmp = static_cast<Job*>(head->next);
          head->next = prev;
          prev = head;
          head = tmp;
        } while (head != nullptr);

        YACLIB_ERROR(head == nullptr, "Broken invariant");
      }

      _mutex._waiters = head;
      _mutex._need_cs_batch = !old_need_batch;
      return false;
    }
    AsyncMutex& _mutex;
    IExecutor& _executor;
  };

  template <UnlockType Type, bool Fifo>
  class TypedUnlockAwaiter : public UnlockAwaiterBase {
   public:
    using UnlockAwaiterBase::UnlockAwaiterBase;

    bool await_ready() noexcept {
      return AwaitReadyHelper(Fifo);
    }

    template <class V, class E>
    yaclib_std::coroutine_handle<> await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
      Job* next = _mutex._waiters;

      YACLIB_ERROR(next != nullptr, "Mutex' waiters must be non-empty");

      _mutex._waiters = static_cast<Job*>(next->next);

      if constexpr (Type == UnlockType::ContinueSameThread) {
        _mutex._need_cs_batch = true;
        _executor.Submit(*next);
        return handle;
      } else if constexpr (Type == UnlockType::CriticalSectionSameThread) {
        _executor.Submit(handle.promise());
        return next->GetHandle();
      } else {
        if (_mutex._need_cs_batch) {
          _executor.Submit(handle.promise());
          return next->GetHandle();
        } else {
          _mutex._need_cs_batch = true;
          _executor.Submit(*next);
          return handle;
        }
      }
    }
  };

  template <bool Fifo>
  class ReschedulingUnlockAwaiter : public UnlockAwaiterBase {
   public:
    ReschedulingUnlockAwaiter(AsyncMutex& mutex, IExecutor& continuation_executor, IExecutor& cs_executor)
        : UnlockAwaiterBase(mutex, continuation_executor), _executor_for_cs(cs_executor) {
    }

    bool await_ready() noexcept {
      return AwaitReadyHelper(Fifo);
    }

    template <class V, class E>
    bool await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
      Job* next = _mutex._waiters;

      YACLIB_ERROR(next != nullptr, "Mutex' waiters must be non-empty");

      _mutex._waiters = static_cast<Job*>(next->next);
      _mutex._need_cs_batch = true;

      _executor.Submit(handle.promise());
      _executor_for_cs.Submit(*next);
      return false;
    }

   private:
    IExecutor& _executor_for_cs;  // for critical section
  };

  class LockAwaiter {
   public:
    explicit LockAwaiter(AsyncMutex& mutex);

    bool await_ready() noexcept {
      void* old_state = _mutex._state.load(std::memory_order_acquire);
      if (old_state != _mutex.NotLocked()) {
        return false;  // mutex is locked by another execution thread
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
            return false;
          }
        } else {
          promise_ptr->next = reinterpret_cast<Job*>(old_state);
          if (_mutex._state.compare_exchange_weak(old_state, reinterpret_cast<void*>(promise_ptr),
                                                  std::memory_order_release, std::memory_order_relaxed)) {
            return true;
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

    LockAwaiter& Lock() {
      _owns = true;
      return *this;
    }

    template <UnlockType Type = UnlockType::Default, bool Fifo = false>
    TypedUnlockAwaiter<Type, Fifo> Unlock(IExecutor& executor = CurrentThreadPool()) {
      _owns = false;
      return TypedUnlockAwaiter<Type, Fifo>{_mutex, executor};
    }

    template <bool Fifo = false>
    ReschedulingUnlockAwaiter<Fifo> Unlock(IExecutor& e1, IExecutor& e2) {
      _owns = false;
      return ReschedulingUnlockAwaiter<Fifo>(_mutex, e1, e2);
    }

    ~GuardAwaiter() {
      if (_owns) {
        _mutex.SimpleUnlock();
      }
    }

   private:
    bool _owns;
  };

 public:
  AsyncMutex();

  LockAwaiter Lock();
  bool TryLock();

  template <UnlockType Type = UnlockType::Default, bool Fifo = false>
  TypedUnlockAwaiter<Type, Fifo> Unlock(IExecutor& executor = CurrentThreadPool()) {
    return TypedUnlockAwaiter<Type, Fifo>{*this, executor};
  }

  template <bool Fifo = false>
  ReschedulingUnlockAwaiter<Fifo> Unlock(IExecutor& e1, IExecutor& e2) {
    return ReschedulingUnlockAwaiter<Fifo>(*this, e1, e2);
  }

  template <bool Fifo = false>
  void SimpleUnlock() {  // TODO rename
    YACLIB_ERROR(_state.load(std::memory_order_relaxed) != NotLocked(), "unlock must be called after lock!");
    auto* head = _waiters;
    if (head == LockedNoWaiters()) {
      auto old_state = LockedNoWaiters();
      if (_state.compare_exchange_strong(old_state, NotLocked(), std::memory_order_release,
                                         std::memory_order_relaxed)) {
        return;
      }
      old_state = _state.exchange(LockedNoWaiters(), std::memory_order_acquire);

      YACLIB_DEBUG(old_state != LockedNoWaiters() && old_state != NotLocked(), "There must be awaiters!");

      if constexpr (Fifo) {
        auto* next = static_cast<Job*>(old_state);
        do {
          auto* temp = static_cast<Job*>(next->next);
          next->next = head;
          head = next;
          next = temp;
        } while (next != nullptr);
      }
    }
    YACLIB_DEBUG(head != nullptr, "Must be locked with at least 1 awaiter!");

    _waiters = static_cast<Job*>(head->next);
    CurrentThreadPool().Submit(*head);
  }

  GuardAwaiter Guard() noexcept;

 private:
  YACLIB_INLINE void* NotLocked() noexcept {
    return this;
  }

  YACLIB_INLINE void* LockedNoWaiters() noexcept {
    return nullptr;
  }

  yaclib_std::atomic<void*> _state;  // locked without waiters, not locked, otherwise - head of the awaiterslist
  Job* _waiters;
  bool _need_cs_batch;
};

}  // namespace yaclib
