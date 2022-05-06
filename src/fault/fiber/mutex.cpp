#include <yaclib/fault/detail/fiber/mutex.hpp>

namespace yaclib::detail::fiber {

void Mutex::lock() {
  while (_occupied) {
    _queue.Wait();
  }
  _occupied = true;
}

bool Mutex::try_lock() noexcept {
  if (_occupied) {
    return false;
  }
  _occupied = true;
  return true;
}

void Mutex::unlock() noexcept {
  _occupied = false;
  _queue.NotifyOne();
}
}  // namespace yaclib::detail::fiber
