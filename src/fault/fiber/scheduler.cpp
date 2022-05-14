#include "fault/util.hpp"

#include <yaclib/fault/detail/fiber/scheduler.hpp>

#include <iostream>

namespace yaclib::fault {

Scheduler* current_scheduler = nullptr;

static thread_local detail::fiber::Fiber* current;

detail::fiber::Fiber* Scheduler::GetNext() {
  YACLIB_DEBUG(_queue.Empty(), "Queue can't be empty");
  auto* next = PollRandomElementFromList(_queue);
  return static_cast<detail::fiber::Fiber*>(static_cast<detail::fiber::BiNodeScheduleQueue*>(next));
}

bool Scheduler::IsRunning() const {
  return _running;
}

void Scheduler::Suspend() {
  YACLIB_DEBUG(current == nullptr, "Current can't be null");
  auto* fiber = current;
  fiber->Yield();
}

void Scheduler::Stop() {
  Scheduler::Set(nullptr);
  YACLIB_DEBUG(_running, "scheduler still running on stop");
}

Scheduler* Scheduler::GetScheduler() {
  return current_scheduler;
}

void Scheduler::Set(Scheduler* scheduler) {
  current_scheduler = scheduler;
}

void Scheduler::Schedule(detail::fiber::Fiber* fiber) {
  InjectFault();
  _queue.PushBack(static_cast<detail::fiber::BiNodeScheduleQueue*>(fiber));
  if (!IsRunning()) {
    _running = true;
    RunLoop();
    _running = false;
  }
}

detail::fiber::Fiber* Scheduler::Current() {
  return current;
}

detail::fiber::Fiber::Id Scheduler::GetId() {
  YACLIB_DEBUG(current == nullptr, "Current can't be null");
  return current->GetId();
}

void Scheduler::TickTime() {
  _time += 10;
}

void Scheduler::AdvanceTime() {
  auto min_sleep_time = _sleep_list.begin()->first - _time;
  if (min_sleep_time >= 0) {
    _time += min_sleep_time;
  }
}

uint64_t Scheduler::GetTimeNs() const {
  return _time;
}

void Scheduler::WakeUpNeeded() {
  auto iter_to_remove = _sleep_list.end();
  for (auto& elem : _sleep_list) {
    if (elem.first > _time) {
      iter_to_remove = _sleep_list.find(elem.first);
      break;
    }
    while (!elem.second.Empty()) {
      _queue.PushBack(static_cast<detail::fiber::BiNodeScheduleQueue*>(
        static_cast<detail::fiber::Fiber*>(static_cast<detail::fiber::BiNodeSleep*>(elem.second.PopBack()))));
    }
  }
  if (iter_to_remove != _sleep_list.begin()) {
    _sleep_list.erase(_sleep_list.begin(), iter_to_remove);
  }
}

void Scheduler::RunLoop() {
  while (!_queue.Empty() || !_sleep_list.empty()) {
    if (_queue.Empty()) {
      AdvanceTime();
    }
    WakeUpNeeded();
    YACLIB_INFO(_queue.Empty(), "Potentially deadlock");
    auto* next = GetNext();
    current = next;
    TickTime();
    next->Resume();
    if (next->GetState() == detail::fiber::Completed && !next->IsThreadlikeInstanceAlive()) {
      delete next;
    }
  }
  current = nullptr;
}

void Scheduler::RescheduleCurrent() {
  if (current == nullptr) {
    return;
  }
  auto* fiber = current;
  GetScheduler()->_queue.PushBack(static_cast<detail::fiber::BiNodeScheduleQueue*>(fiber));
  fiber->Yield();
}

Scheduler::Scheduler() : _running(false), _time(0) {
}
}  // namespace yaclib::fault

namespace yaclib::detail::fiber {

BiNode* PollRandomElementFromList(BiList& list) {
  auto rand_pos = detail::GetRandNumber(list.GetSize());
  auto* next = list.GetNth(rand_pos);
  list.Erase(next);
  return next;
}
}  // namespace yaclib::detail::fiber
