#pragma once

#include <yaclib/coroutine/detail/on_awaiter.hpp>
#include <yaclib/exe/executor.hpp>

namespace yaclib {

/**
 * TODO(mkornaukhov03) Add doxygen docs
 */
detail::OnAwaiter On(IExecutor& e);

}  // namespace yaclib
