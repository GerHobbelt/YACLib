#pragma once

#include <yaclib/fault/inject.hpp>

namespace yaclib::detail {

template <typename Impl>
class Mutex : protected Impl {
 public:
  using Impl::Impl;

  void lock() {
    YACLIB_INJECT_FAULT(Impl::lock());
  }

  bool try_lock() {
    YACLIB_INJECT_FAULT(auto r = Impl::try_lock());
    return r;
  }

  void unlock() {
    YACLIB_INJECT_FAULT(Impl::unlock());
  }

  /// Internal
  using impl_t = Impl;
  impl_t& GetImpl() {
    return *this;
  }
};

}  // namespace yaclib::detail
