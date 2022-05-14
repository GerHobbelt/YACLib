#include "util.hpp"

namespace yaclib::detail {

inline constexpr int kSeed = 1239;

static std::atomic_uint32_t seed = kSeed;

// TODO(myannyax): make not thread_static
thread_local static std::mt19937_64 eng(seed.load());

void SetSeed(uint32_t new_seed) {
  eng.seed(new_seed);
}

uint32_t GetSeed() {
  return seed;
}

uint64_t GetRandNumber(uint64_t max) {
  return eng() % max;
}

}  // namespace yaclib::detail
