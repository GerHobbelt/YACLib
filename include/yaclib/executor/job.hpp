#pragma once

#include <yaclib/config.hpp>
#include <yaclib/util/detail/node.hpp>
#include <yaclib/util/func.hpp>

#if defined(YACLIB_CORO)
#  include <yaclib/coroutine/coroutine.hpp>
#endif

namespace yaclib {

/**
 * Callable that can be executed in an IExecutor \see IExecutor
 */
class Job : public detail::Node, public IFunc {
 public:
  virtual void Cancel() noexcept = 0;
#if defined(YACLIB_CORO)
  virtual yaclib_std::coroutine_handle<> GetHandle() noexcept {
    return yaclib_std::coroutine_handle<>{};  // plug, see coroutine/detail/promise_type.hpp
  }
#endif
};

}  // namespace yaclib
