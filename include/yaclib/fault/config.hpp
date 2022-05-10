#pragma once

#include <cstdint>

namespace yaclib {

/**
 * Sets frequency with which fault will be injected.
 * Default is 16.
 */
void SetFaultFrequency(std::uint32_t freq);

/**
 * Sets seed for random, which will be used when deciding when to yield, for fibers scheduler and random wrapper for
 * tests. Default is 1239.
 */
void SetSeed(std::uint32_t seed);

/**
 * Sets sleep time if sleep is used instead of yield for interrupting thread execution for fault injection.
 * Default is 200
 */
void SetFaultSleepTime(std::uint32_t ns);

}  // namespace yaclib
