#include <yaclib/fault/detail/fiber/queue.hpp>

namespace yaclib::detail::fiber {

void FiberQueue::Wait() {
  auto* fiber = Scheduler::Current();
  _queue.PushBack(static_cast<BiNodeWaitQueue*>(fiber));
  Scheduler::Suspend();
}

void FiberQueue::NotifyAll() {
  std::vector<Fiber*> all;
  while (!_queue.Empty()) {
    all.push_back(static_cast<Fiber*>(static_cast<BiNodeWaitQueue*>(_queue.PopBack())));
  }
  for (const auto& elem : all) {
    Scheduler::GetScheduler()->Schedule(elem);
  }
}

void FiberQueue::NotifyOne() {
  if (_queue.Empty()) {
    return;
  }
  auto* fiber = PollRandomElementFromList(_queue);
  Scheduler::GetScheduler()->Schedule(static_cast<Fiber*>(static_cast<BiNodeWaitQueue*>(fiber)));
}
}  // namespace yaclib::detail::fiber
