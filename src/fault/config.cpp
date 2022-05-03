#include "util.hpp"

#include <yaclib/fault/config.hpp>
#include <yaclib/fault/injector.hpp>

namespace yaclib {

void SetFaultFrequency(std::uint32_t freq) {
  yaclib::detail::Injector::SetFrequency(freq);
}

void SetFaultSleepTime(std::uint32_t ns) {
  yaclib::detail::Injector::SetSleepTime(ns);
}

void SetSeed(std::uint32_t seed) {
#ifdef YACLIB_FAULT
  detail::SetSeed(seed);
#endif
}

}  // namespace yaclib
