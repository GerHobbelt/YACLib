#pragma once

#include <yaclib/fault/detail/fiber/default_allocator.hpp>
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/stack.hpp>
#include <yaclib/fault/detail/fiber/stack_allocator.hpp>
#include <yaclib/fault/inject.hpp>
#include <yaclib/log.hpp>
#include <yaclib/util/func.hpp>

#include <chrono>
#include <map>

namespace yaclib::detail::fiber {
class FiberQueue;
}  // namespace yaclib::detail::fiber

namespace yaclib::fault {

class Scheduler {
 public:
  friend class detail::fiber::FiberQueue;

  Scheduler();

  void Schedule(detail::fiber::Fiber* fiber);

  [[nodiscard]] bool IsRunning() const;

  uint64_t Sleep(uint64_t ns);

  [[nodiscard]] uint64_t GetTimeNs() const;

  static detail::fiber::Fiber* Current();

  static detail::fiber::Fiber::Id GetId();

  static void RescheduleCurrent();

  static void SetTickLength(uint32_t tick);

  static void SetRandomListPick(uint32_t k);

  void Stop();

  static Scheduler* GetScheduler();

  static void Set(Scheduler* scheduler);

 private:
  static void Suspend();

  void AdvanceTime();

  void TickTime();

  detail::fiber::Fiber* GetNext();

  void RunLoop();

  void WakeUpNeeded();

  uint64_t _time;
  detail::fiber::BiList _queue;
  std::map<uint64_t, detail::fiber::BiList> _sleep_list;
  bool _running;
};

}  // namespace yaclib::fault

namespace yaclib::detail::fiber {
BiNode* PollRandomElementFromList(BiList& list);
}  // namespace yaclib::detail::fiber
