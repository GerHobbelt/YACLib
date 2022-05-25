#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/inject.hpp>
#include <yaclib/fault/injector.hpp>

#include <utility>

namespace yaclib::detail::fiber {

static Fiber::Id next_id{1L};

DefaultAllocator Fiber::allocator{};

Fiber::Fiber(Routine routine) : _id(next_id++), _stack(allocator), _routine(std::move(routine)) {
  _context.Setup(_stack.GetAllocation(), Trampoline, this);
}

Fiber::Id Fiber::GetId() const {
  return _id;
}

void Fiber::Resume() {
  if (_state == Completed) {
    return;
  }

  _state = Running;

  _caller_context.SwitchTo(_context);

  if (_exception != nullptr) {
    rethrow_exception(_exception);
  }
}

void Fiber::Yield() {
  _state = Suspended;
  _context.SwitchTo(_caller_context);
}

void Fiber::Complete() {
  _state = Completed;
  if (_joining_fiber != nullptr && _threadlike_instance_alive) {
    GetInjector()->SetPauseInject(true);
    ScheduleFiber(_joining_fiber);
    GetInjector()->SetPauseInject(false);
  }
  _context.SwitchTo(_caller_context);
}

void Fiber::Trampoline(void* arg) {
  auto* coroutine = reinterpret_cast<Fiber*>(arg);

  try {
    coroutine->_routine->Call();
  } catch (...) {
    coroutine->_exception = std::current_exception();
  }

  coroutine->Complete();
}

FiberState Fiber::GetState() {
  return _state;
}

void Fiber::SetJoiningFiber(Fiber* joining_fiber) {
  _joining_fiber = joining_fiber;
}

void Fiber::SetThreadlikeInstanceDead() {
  _threadlike_instance_alive = false;
}

bool Fiber::IsThreadlikeInstanceAlive() const {
  return _threadlike_instance_alive;
}

void* Fiber::GetTls(uint64_t name, void* _default) {
  return _tls[name] == nullptr ? _default : _tls[name];
}

void Fiber::SetTls(uint64_t name, void* value) {
  _tls[name] = value;
}

IStackAllocator& Fiber::GetAllocator() {
  return allocator;
}

}  // namespace yaclib::detail::fiber
