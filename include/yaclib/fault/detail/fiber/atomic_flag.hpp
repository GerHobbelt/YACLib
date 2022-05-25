#pragma once

#include <yaclib/config.hpp>
#include <yaclib/fault/detail/fiber/atomic_wait.hpp>
#include <yaclib/fault/inject.hpp>

namespace yaclib::detail::fiber {

class AtomicFlag : public AtomicWait<bool> {
  using Base = AtomicWait<bool>;

 public:
  using Base::Base;

  void clear(std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    _value = false;
  }
  void clear(std::memory_order order = std::memory_order_seq_cst) noexcept {
    _value = false;
  }

  bool test_and_set(std::memory_order order = std::memory_order_seq_cst) volatile noexcept {
    auto val = _value;
    _value = true;
    return val;
  }
  bool test_and_set(std::memory_order order = std::memory_order_seq_cst) noexcept {
    auto val = _value;
    _value = true;
    return val;
  }

#ifdef YACLIB_ATOMIC_EVENT
  bool test(std::memory_order order = std::memory_order::seq_cst) const volatile noexcept {
    return _value;
  }
  bool test(std::memory_order order = std::memory_order::seq_cst) const noexcept {
    return _value;
  }
#endif
 private:
  using Base::_value;
};

}  // namespace yaclib::detail::fiber
