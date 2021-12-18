#include <yaclib/fault/detail/mutex/recursive_timed_mutex.hpp>

namespace yaclib::detail {

void RecursiveTimedMutex::lock() {
  unsigned new_level = _lock_level;
  if (_owner == yaclib::std::this_thread::get_id()) {
    new_level++;
  } else {
    new_level = 1;
  }
  yaclib::detail::InjectFault();
  _m.lock();
  _lock_level = new_level;
  _owner = yaclib::std::this_thread::get_id();
  yaclib::detail::InjectFault();
}

bool RecursiveTimedMutex::try_lock() noexcept {
  unsigned new_level = _lock_level;
  if (_owner == yaclib::std::this_thread::get_id()) {
    new_level++;
  } else {
    new_level = 1;
  }
  yaclib::detail::InjectFault();
  auto res = _m.try_lock();
  if (res) {
    _lock_level = new_level;
    _owner = yaclib::std::this_thread::get_id();
  }
  yaclib::detail::InjectFault();
  return res;
}

void RecursiveTimedMutex::unlock() noexcept {
  assert(_owner != yaclib::detail::kInvalidThreadId);
  assert(_owner == yaclib::std::this_thread::get_id());
  yaclib::detail::InjectFault();
  _m.unlock();
  _lock_level--;
  if (_lock_level == 0) {
    _owner = yaclib::detail::kInvalidThreadId;
  }
  yaclib::detail::InjectFault();
}

template <typename _Clock, typename _Duration>
bool RecursiveTimedMutex::try_lock_until(const ::std::chrono::time_point<_Clock, _Duration>& duration) {
  unsigned new_level = _lock_level;
  if (_owner == yaclib::std::this_thread::get_id()) {
    new_level++;
  } else {
    new_level = 1;
  }
  yaclib::detail::InjectFault();
  auto res = _m.try_lock_until(duration);
  if (res) {
    _lock_level = new_level;
    _owner = yaclib::std::this_thread::get_id();
  }
  yaclib::detail::InjectFault();
  return res;
}

}  // namespace yaclib::detail
