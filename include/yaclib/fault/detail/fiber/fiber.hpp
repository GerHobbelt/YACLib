#pragma once

#include <yaclib/fault/detail/fiber/bidirectional_intrusive_list.hpp>
#include <yaclib/fault/detail/fiber/default_allocator.hpp>
#include <yaclib/fault/detail/fiber/execution_context.hpp>
#include <yaclib/fault/detail/fiber/stack.hpp>
#include <yaclib/fault/detail/fiber/stack_allocator.hpp>
#include <yaclib/util/detail/shared_func.hpp>

#include <unordered_map>

namespace yaclib::detail::fiber {

using Routine = yaclib::IFuncPtr;

class BiNodeSleep : public BiNode {};

class BiNodeWaitQueue : public BiNode {};

class BiNodeScheduleQueue : public BiNode {};

enum FiberState {
  Running,
  Suspended,
  Completed,
};

class Fiber : public BiNodeSleep, public BiNodeWaitQueue, public BiNodeScheduleQueue {
 public:
  using Id = uint64_t;

  Fiber(Fiber&& other) noexcept = default;

  Fiber& operator=(Fiber&& other) noexcept = default;

  Fiber(IStackAllocator& allocator, Routine routine);

  explicit Fiber(Routine routine);

  void SetCompleteCallback(Routine routine);

  [[nodiscard]] Id GetId() const;

  void Resume();

  void Yield();

  FiberState GetState();

  void SetThreadlikeInstanceDead();

  [[nodiscard]] bool IsThreadlikeInstanceAlive() const;

  void* GetTls(uint64_t name, void* _default);

  void SetTls(uint64_t name, void* value);

 private:
  [[noreturn]] static void Trampoline(void* arg);

  void Complete();

  Stack _stack;
  ExecutionContext _context{};
  ExecutionContext _caller_context{};
  Routine _routine;
  Routine _complete_callback{nullptr};
  std::exception_ptr _exception;
  Id _id;
  FiberState _state{Suspended};
  bool _threadlike_instance_alive{true};
  std::unordered_map<uint64_t, void*> _tls;
};

}  // namespace yaclib::detail::fiber
