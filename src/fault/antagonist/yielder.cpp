#include <yaclib/fault/detail/antagonist/yielder.hpp>

#define YACLIB_FAULT_SLEEP_TIME_NS 200

namespace yaclib::detail {

// TODO(myannyax) maybe scheduler-wide random engine?
Yielder::Yielder(uint32_t frequency) : _freq(frequency), _eng(1142) {
}

void Yielder::MaybeYield() {
  if (ShouldYield()) {
    yaclib_std::this_thread::sleep_for(std::chrono::nanoseconds(RandNumber(YACLIB_FAULT_SLEEP_TIME_NS)));
  }
}

bool Yielder::ShouldYield() {
  if (_count.fetch_add(1, std::memory_order_acq_rel) >= _freq) {
    Reset();
    return true;
  }
  return false;
}

void Yielder::Reset() {
  _count.exchange(RandNumber(_freq));
}

unsigned Yielder::RandNumber(uint32_t max) {
  return 1 + _eng() % (max - 1);
}

}  // namespace yaclib::detail