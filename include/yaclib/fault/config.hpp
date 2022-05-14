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

/**
 * Sets the amount of time to be added to fiber's scheduler time after each schedule cycle.
 * Default is 10
 */
void SetFaultTickLength(std::uint32_t ns);

/**
 * Sets the length of scheduler queue prefix and suffix from which the next schedule candidate will be chosen.
 * Default 0 meaning the candidate will be chosen randomly from the whole queue.
 */
void SetFaultRandomListPick(std::uint32_t k);

}  // namespace yaclib
