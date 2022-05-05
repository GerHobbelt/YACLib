#include "util.hpp"

#include <yaclib/fault/injector.hpp>

#include <yaclib_std/thread>

namespace yaclib::detail {

inline constexpr int kFreq = 16;
inline constexpr int kSleepTimeNs = 200;

std::atomic_uint32_t Injector::yield_frequency = kFreq;
std::atomic_uint32_t Injector::sleep_time = kSleepTimeNs;

// TODO(myannyax) maybe scheduler-wide random engine?
Injector::Injector() : _count{0} {
}

void Injector::MaybeInject() {
  if (NeedInject()) {
    yaclib_std::this_thread::sleep_for(std::chrono::nanoseconds(1 + GetRandNumber(sleep_time - 1)));
  }
}

bool Injector::NeedInject() {
  if (_pause) {
    return false;
  }
  if (++_count >= yield_frequency) {
    Reset();
    return true;
  }
  return false;
}

void Injector::Reset() {
  _count = 1 + GetRandNumber(yield_frequency - 1);
}

void Injector::SetFrequency(uint32_t freq) {
  yield_frequency.store(freq);
}

void Injector::SetSleepTime(uint32_t ns) {
  sleep_time.store(ns);
}

uint32_t Injector::GetState() const {
  return _count;
}

void Injector::SetState(uint32_t state) {
  _count = state;
}

void Injector::SetPauseInject(bool pause) {
  _pause = pause;
}

}  // namespace yaclib::detail
