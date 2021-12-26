#include <yaclib/fault/detail/mutex/mutex.hpp>

// TODO(myannyax) avoid copypaste from timed_mutex
namespace yaclib::detail {

void Mutex::lock() {
  assert(_owner != yaclib_std::this_thread::get_id());

  YACLIB_INJECT_FAULT(_m.lock();)

  _owner = yaclib_std::this_thread::get_id();
}

bool Mutex::try_lock() noexcept {
  assert(_owner != yaclib_std::this_thread::get_id());

  YACLIB_INJECT_FAULT(auto res = _m.try_lock();)

  if (res) {
    _owner = yaclib_std::this_thread::get_id();
  }
  return res;
}

void Mutex::unlock() noexcept {
  assert(_owner != yaclib::detail::kInvalidThreadId);
  assert(_owner == yaclib_std::this_thread::get_id());

  _owner = yaclib::detail::kInvalidThreadId;

  YACLIB_INJECT_FAULT(_m.unlock();)
}

Mutex::native_handle_type Mutex::native_handle() {
  return _m.native_handle();
}

}  // namespace yaclib::detail