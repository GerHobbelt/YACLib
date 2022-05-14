#pragma once

#include <atomic>
#include <yaclib_std/thread>

namespace yaclib::detail {

// TODO(myannyax) Add metrics, refactor this shit
class Injector {
 public:
  explicit Injector();
  void MaybeInject();

  uint32_t GetState() const;

  void SetState(uint32_t state);

  void SetPauseInject(bool pause);

  static void SetFrequency(uint32_t freq);
  static void SetSleepTime(uint32_t ns);

 private:
  bool NeedInject();
  void Reset();

  static std::atomic_uint32_t yield_frequency;
  static std::atomic_uint32_t sleep_time;

  std::atomic_uint32_t _count;
  bool _pause;
};

}  // namespace yaclib::detail
