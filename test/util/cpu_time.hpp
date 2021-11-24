#pragma once

#include <chrono>
#include <cstdlib>
#include <ctime>

namespace test::util {

/* TODO(Ri7ay): Does not work on Windows, check unit/executor/thread_pool.cpp tests */
#if __linux
class ProcessCPUTimer {
 public:
  ProcessCPUTimer() {
    Reset();
  }

  std::chrono::microseconds Elapsed() const {
    return std::chrono::microseconds(ElapsedMicros());
  }

  void Reset() {
    _start_ts = std::clock();
  }

 private:
  size_t ElapsedMicros() const {
    const size_t clocks = std::clock() - _start_ts;
    return ClocksToMicros(clocks);
  }

  static size_t ClocksToMicros(const size_t clocks) {
    return (clocks * 1'000'000) / CLOCKS_PER_SEC;
  }

 private:
  std::clock_t _start_ts;
};

class ThreadCPUTimer {
 public:
  ThreadCPUTimer() {
    Reset();
  }

  std::chrono::nanoseconds Elapsed() const {
    return std::chrono::nanoseconds(ElapsedNanos());
  }

  void Reset() {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &_start);
  }

 private:
  uint64_t ElapsedNanos() const {
    struct timespec now;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);

    return ToNanos(now) - ToNanos(_start);
  }

  static uint64_t ToNanos(const struct timespec& tp) {
    return tp.tv_sec * 1'000'000'000 + tp.tv_nsec;
  }

 private:
  struct timespec _start;
};
#endif  // linux

}  // namespace test::util
