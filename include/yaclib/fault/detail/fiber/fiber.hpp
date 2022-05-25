#pragma once

#include <yaclib/fault/detail/fiber/bidirectional_intrusive_list.hpp>
#include <yaclib/fault/detail/fiber/default_allocator.hpp>
#include <yaclib/fault/detail/fiber/execution_context.hpp>
#include <yaclib/fault/detail/fiber/stack.hpp>
#include <yaclib/fault/detail/fiber/stack_allocator.hpp>
#include <yaclib/fault/detail/fiber/wakeup_helper.hpp>
#include <yaclib/util/func.hpp>
#include <yaclib/util/intrusive_ptr.hpp>

#include <unordered_map>

namespace yaclib::detail::fiber {

using Routine = IntrusivePtr<IFunc>;

class BiNodeScheduler : public BiNode {};

class BiNodeWaitQueue : public BiNode {};

enum FiberState {
  Running,
  Suspended,
  Completed,
};

class FiberBase : public BiNodeScheduler, public BiNodeWaitQueue {
 public:
  using Id = uint64_t;

  FiberBase();

  void SetJoiningFiber(FiberBase* joining_fiber);

  [[nodiscard]] Id GetId() const;

  void Resume();

  void Yield();

  FiberState GetState();

  void SetThreadlikeInstanceDead();

  [[nodiscard]] bool IsThreadlikeInstanceAlive() const;

  void* GetTls(uint64_t name, void* _default);

  void SetTls(uint64_t name, void* value);

  static IStackAllocator& GetAllocator();

 protected:
  void Complete();

  Stack _stack;
  ExecutionContext _context{};
  ExecutionContext _caller_context{};
  FiberBase* _joining_fiber{nullptr};
  std::exception_ptr _exception;
  Id _id;
  FiberState _state{Suspended};
  bool _threadlike_instance_alive{true};
  std::unordered_map<uint64_t, void*> _tls;
  static DefaultAllocator allocator;
};

template <typename Fp, typename... Args>
using FuncState = std::tuple<typename std::decay<Fp>::type, typename std::decay<Args>::type...>;

template <typename Fp, typename... Args>
class Fiber : public FiberBase {
 public:
  Fiber(FuncState<Fp, Args...>&& func_state) : _func_state(std::move(func_state)) {
    _context.Setup(_stack.GetAllocation(), Trampoline, this);
  }

  [[noreturn]] static void Trampoline(void* arg) {
    auto* coroutine = reinterpret_cast<Fiber*>(arg);
    try {
      Helper(coroutine->_func_state, std::index_sequence_for<FuncState<Fp, Args...>>{});
    } catch (...) {
      coroutine->_exception = std::current_exception();
    }

    coroutine->Complete();
  }

 private:
  template <typename Tuple, std::size_t... I>
  static auto Helper(Tuple& a, std::index_sequence<I...>) {
    std::invoke(std::move(std::get<I>(a))...);
  }

  FuncState<Fp, Args...> _func_state;
};

}  // namespace yaclib::detail::fiber
