#pragma once

#include <yaclib/coroutine/coroutine.hpp>
#include <yaclib/coroutine/detail/promise_type.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/strand.hpp>
#include <yaclib/executor/thread_pool.hpp>
#include <yaclib/fault/atomic.hpp>

#include <iostream>  // debug

namespace yaclib {

template <bool FIFO = false>
class AsyncMutex {
 public:
  enum class UnlockType {
    AlwaysReschedule,
    Batching,
  };

 private:
  class LockAwaiter;
  template <UnlockType Type>
  class UnlockAwaiter;
  class GuardAwaiter;
  class LockGuard;

 public:
  AsyncMutex() : _state{NotLocked()}, _waiters{nullptr}, _need_next_cs_here{false} {
  }

  LockAwaiter Lock() {
    return LockAwaiter{*this};
  }

  YACLIB_INLINE bool TryLock() noexcept {
    void* old_state = _state.load(std::memory_order_acquire);
    if (old_state != NotLocked()) {
      return false;  // mutex is locked by another execution thread
    }
    return _state.compare_exchange_strong(old_state, LockedNoWaiters(), std::memory_order_acquire,
                                          std::memory_order_relaxed);
  }

  template <UnlockType Type = UnlockType::Batching>
  UnlockAwaiter<Type> Unlock(IExecutor& executor = CurrentThreadPool()) {
    return UnlockAwaiter<Type>{*this, executor};
  }

  void UnlockHere(IExecutor& executor = CurrentThreadPool()) {
    YACLIB_DEBUG(_state.load(std::memory_order_relaxed) == NotLocked(), "UnlockHere must be called after Lock!");
    auto* head = _waiters;
    if (head == LockedNoWaiters()) {
      auto old_state = LockedNoWaiters();
      if (_state.compare_exchange_strong(old_state, NotLocked(), std::memory_order_release,
                                         std::memory_order_relaxed)) {
        return;
      }
      old_state = _state.exchange(LockedNoWaiters(), std::memory_order_acquire);

      YACLIB_DEBUG(old_state == LockedNoWaiters() || old_state == NotLocked(), "There must be awaiters!");

      if constexpr (FIFO) {
        auto* next = static_cast<detail::BaseCore*>(old_state);
        do {
          auto* temp = static_cast<detail::BaseCore*>(next->next);
          next->next = head;
          head = next;
          next = temp;
        } while (next != nullptr);
      } else {
        head = static_cast<detail::BaseCore*>(old_state);
      }
    }

    _waiters = static_cast<detail::BaseCore*>(head->next);
    executor.Submit(*head);
  }

  GuardAwaiter Guard() noexcept {
    return GuardAwaiter(*this);
  }

  YACLIB_INLINE LockGuard TryGuard() noexcept {
    return LockGuard{*this, TryLock()};
  }

 private:
  template <UnlockType Type>
  class [[nodiscard]] UnlockAwaiter {
   public:
    explicit UnlockAwaiter(AsyncMutex& mutex, IExecutor& executor) : _mutex{mutex}, _executor{executor} {
    }

    YACLIB_INLINE bool await_ready() noexcept {
      if (_mutex._waiters != _mutex.LockedNoWaiters()) {
        return false;
      }

      const bool old_need_next_cs_here = std::exchange(_mutex._need_next_cs_here, false);

      void* old_state = _mutex.LockedNoWaiters();
      if (_mutex._state.compare_exchange_strong(old_state, _mutex.NotLocked(), std::memory_order_release,
                                                std::memory_order_relaxed)) {
        return true;
      }

      detail::BaseCore* head = static_cast<detail::BaseCore*>(_mutex._state.exchange(_mutex.LockedNoWaiters()));

      if (FIFO) {
        detail::BaseCore* prev = nullptr;
        do {
          detail::BaseCore* tmp = static_cast<detail::BaseCore*>(head->next);
          head->next = prev;
          prev = head;
          head = tmp;
        } while (head != nullptr);
      }

      _mutex._waiters = head;
      _mutex._need_next_cs_here = !old_need_next_cs_here;
      return false;
    }

    template <typename V, typename E>
    yaclib_std::coroutine_handle<> await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
      detail::BaseCore* next = _mutex._waiters;
      _mutex._waiters = static_cast<detail::BaseCore*>(next->next);

      if constexpr (Type == UnlockType::AlwaysReschedule) {
        _executor.Submit(handle.promise());
        return next->GetHandle();
      } else {
        if (_mutex._need_next_cs_here) {
          _executor.Submit(handle.promise());
          return next->GetHandle();
        } else {
          _mutex._need_next_cs_here = true;
          _executor.Submit(*next);
          return handle;
        }
      }
    }

    YACLIB_INLINE void await_resume() noexcept {
    }

   private:
    AsyncMutex& _mutex;
    IExecutor& _executor;
  };

  class [[nodiscard]] LockAwaiter {
   public:
    explicit LockAwaiter(AsyncMutex& mutex) : _mutex{mutex} {
    }

    YACLIB_INLINE bool await_ready() noexcept {
      void* old_state = _mutex._state.load(std::memory_order_acquire);
      if (old_state != _mutex.NotLocked()) {
        return false;  // mutex is locked by another execution thread
      }
      return _mutex._state.compare_exchange_strong(old_state, _mutex.LockedNoWaiters(), std::memory_order_acquire,
                                                   std::memory_order_relaxed);
    }

    template <class V, class E>
    bool await_suspend(yaclib_std::coroutine_handle<detail::PromiseType<V, E>> handle) {
      detail::BaseCore* promise_ptr = &handle.promise();
      void* old_state = _mutex._state.load(std::memory_order_acquire);
      while (true) {
        if (old_state == _mutex.NotLocked()) {
          if (_mutex._state.compare_exchange_weak(old_state, _mutex.LockedNoWaiters(), std::memory_order_acquire,
                                                  std::memory_order_relaxed)) {
            return false;
          }
        } else {
          promise_ptr->next = reinterpret_cast<detail::BaseCore*>(old_state);
          if (_mutex._state.compare_exchange_weak(old_state, reinterpret_cast<void*>(promise_ptr),
                                                  std::memory_order_release, std::memory_order_relaxed)) {
            return true;
          }
        }
      }
    }

    YACLIB_INLINE void await_resume() noexcept {
    }

   protected:
    AsyncMutex& _mutex;
  };

  class [[nodiscard]] GuardAwaiter : public LockAwaiter {
   public:
    explicit GuardAwaiter(AsyncMutex& mutex) : LockAwaiter{mutex} {
    }

    YACLIB_INLINE LockGuard await_resume() noexcept {
      return LockGuard{LockAwaiter::_mutex, true};
    }
  };

  class [[nodiscard]] LockGuard {
   public:
    explicit LockGuard(AsyncMutex& mutex, bool owns) noexcept : _mutex(&mutex), _owns(owns) {
    }

    YACLIB_INLINE LockAwaiter Lock() noexcept {
      _owns = true;
      return _mutex->Lock();
    }

    template <UnlockType Type = UnlockType::Batching>
    UnlockAwaiter<Type> Unlock(IExecutor& executor = CurrentThreadPool()) {
      _owns = false;
      return _mutex->Unlock<Type>(executor);
    }

    void UnlockHere(IExecutor& executor = CurrentThreadPool()) {
      _mutex->UnlockHere(executor);
    }

    ~LockGuard() {
      if (_owns) {
        YACLIB_INFO(true, "Better use co_await Guard::Unlock<UnlockType>(executor)");
        _mutex->UnlockHere();
      }
    }

   private:
    AsyncMutex* _mutex;
    bool _owns;  // TODO add as _mutex bit
  };

  YACLIB_INLINE void* NotLocked() noexcept {
    return this;
  }

  constexpr void* LockedNoWaiters() noexcept {
    return nullptr;
  }

  yaclib_std::atomic<void*> _state;  // locked without waiters, not locked, otherwise - head of the awaiters list
  detail::BaseCore* _waiters;
  bool _need_next_cs_here;  // TODO add as _waiters bit (suppose it won't give significant effect)
};

}  // namespace yaclib
