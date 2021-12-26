#pragma once

#include <yaclib/fault/detail/antagonist/inject_fault.hpp>

#include <cassert>
#include <mutex>

namespace yaclib::detail {

class RecursiveMutex {
 public:
  RecursiveMutex() = default;
  ~RecursiveMutex() noexcept = default;

  RecursiveMutex(const RecursiveMutex&) = delete;
  RecursiveMutex& operator=(const RecursiveMutex&) = delete;

  void lock();
  bool try_lock() noexcept;
  void unlock() noexcept;

  using native_handle_type = std::recursive_mutex::native_handle_type;

  native_handle_type native_handle();

 private:
  std::recursive_mutex _m;
  // TODO(myannyax) yaclib wrapper
  std::atomic<yaclib_std::thread::id> _owner{yaclib::detail::kInvalidThreadId};
  std::atomic<unsigned> _lock_level{0};
  void UpdateOnLock();
};

}  // namespace yaclib::detail