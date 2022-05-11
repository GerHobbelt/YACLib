#pragma once
#include <yaclib/fault/detail/fiber/fiber.hpp>
#include <yaclib/fault/detail/fiber/scheduler.hpp>

#include <string_view>
#include <unordered_map>

namespace yaclib::detail::fiber {

static std::unordered_map<uint64_t, void*> defaults{};
static uint64_t nextFreeIndex{0};

template <typename Type>
class ThreadLocalPtrProxy {
 public:
  ThreadLocalPtrProxy(Type* value) : _i(nextFreeIndex++) {
    defaults[nextFreeIndex] = value;
  }

  ThreadLocalPtrProxy& operator=(Type* value) {
    auto* fiber = fault::Scheduler::Current();
    fiber->SetTls(nextFreeIndex, value);
    return *this;
  }

  Type& operator*() {
    auto* fiber = fault::Scheduler::Current();
    auto& val = *(static_cast<Type*>(fiber->GetTls(nextFreeIndex, defaults[nextFreeIndex])));
    return val;
  }

  Type* operator->() {
    auto* fiber = fault::Scheduler::Current();
    return static_cast<Type*>(fiber->GetTls(nextFreeIndex, defaults[nextFreeIndex]));
  }

 private:
  uint64_t _i;
};
}  // namespace yaclib::detail::fiber
