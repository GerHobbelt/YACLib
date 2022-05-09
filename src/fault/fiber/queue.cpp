#include <yaclib/fault/detail/fiber/queue.hpp>

namespace yaclib::detail::fiber {

void FiberQueue::Wait() {
  auto* fiber = fault::Scheduler::Current();
  _queue.PushBack(static_cast<BiNodeWaitQueue*>(fiber));
  fault::Scheduler::Suspend();
}

void FiberQueue::NotifyAll() {
  while (!_queue.Empty()) {
    fault::Scheduler::GetScheduler()->Schedule(static_cast<Fiber*>(static_cast<BiNodeWaitQueue*>(_queue.PopBack())));
  }
}

void FiberQueue::NotifyOne() {
  if (_queue.Empty()) {
    return;
  }
  auto* fiber = PollRandomElementFromList(_queue);
  fault::Scheduler::GetScheduler()->Schedule(static_cast<Fiber*>(static_cast<BiNodeWaitQueue*>(fiber)));
}

FiberQueue::~FiberQueue() {
  YACLIB_ERROR(!_queue.Empty(), "queue must be empty on destruction - potentially deadlock");
}

FiberQueue& FiberQueue::operator=(FiberQueue&& other) noexcept {
  _queue = std::move(other._queue);
  other._queue = BiList();
  return *this;
}

bool FiberQueue::Empty() {
  return _queue.Empty();
}
}  // namespace yaclib::detail::fiber
