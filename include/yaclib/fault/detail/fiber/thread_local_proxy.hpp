#pragma once
#include <yaclib/fault/detail/fiber/fiber_base.hpp>
#include <yaclib/fault/detail/fiber/scheduler.hpp>

#include <string_view>
#include <unordered_map>

namespace yaclib::detail::fiber {

static std::unordered_map<uint64_t, void*> defaults{};
static uint64_t nextFreeIndex{0};

template <typename Type>
class ThreadLocalPtrProxy {
 public:
  ThreadLocalPtrProxy() : _i(nextFreeIndex++) {
    defaults[_i] = nullptr;
  }

  ThreadLocalPtrProxy(Type* value) : _i(nextFreeIndex++) {
    defaults[_i] = value;
  }
  ThreadLocalPtrProxy(ThreadLocalPtrProxy&& other) : _i(other._i) {
  }
  ThreadLocalPtrProxy(const ThreadLocalPtrProxy& other) : _i(nextFreeIndex++) {
    defaults[_i] = other.operator->();
  }

  ThreadLocalPtrProxy& operator=(Type* value) {
    auto* fiber = fault::Scheduler::Current();
    fiber->SetTls(_i, value);
    return *this;
  }
  ThreadLocalPtrProxy& operator=(ThreadLocalPtrProxy&& other) noexcept {
    _i = other._i;
    return *this;
  }
  ThreadLocalPtrProxy& operator=(const ThreadLocalPtrProxy& other) {
    defaults[_i] = other.operator->();
    return *this;
  }

  template <typename U>
  ThreadLocalPtrProxy(ThreadLocalPtrProxy<U>&& other) noexcept : _i(other._i) {
  }
  template <typename U>
  ThreadLocalPtrProxy(const ThreadLocalPtrProxy<U>& other) noexcept : _i(nextFreeIndex++) {
    defaults[_i] = other.operator->();
  }

  template <typename U>
  ThreadLocalPtrProxy& operator=(ThreadLocalPtrProxy<U>&& other) noexcept {
    _i = other._i;
    return *this;
  }
  template <typename U>
  ThreadLocalPtrProxy& operator=(const ThreadLocalPtrProxy<U>& other) noexcept {
    defaults[_i] = other.operator->();
    return *this;
  }

  Type& operator*() const noexcept {
    auto* fiber = fault::Scheduler::Current();
    auto& val = *(static_cast<Type*>(fiber->GetTls(_i, defaults[_i])));
    return val;
  }

  Type* operator->() const noexcept {
    auto* fiber = fault::Scheduler::Current();
    return static_cast<Type*>(fiber->GetTls(_i, defaults[_i]));
  }

  explicit operator bool() const noexcept {
    auto* fiber = fault::Scheduler::Current();
    auto* val = static_cast<Type*>(fiber->GetTls(_i, defaults[_i]));
    return val != nullptr;
  }

  Type& operator[](std::size_t i) const {
    auto* fiber = fault::Scheduler::Current();
    auto* val = static_cast<Type*>(fiber->GetTls(_i, defaults[_i]));
    val += i;
    return *val;
  }

 private:
  uint64_t _i;
};
}  // namespace yaclib::detail::fiber
