#pragma once

#if YACLIB_FAULT_ATOMIC_FLAG == 1 || YACLIB_FAULT_ATOMIC_FLAG == 2
#  include <yaclib/fault/detail/atomic_flag.hpp>

#  include <atomic>

namespace yaclib_std {

using atomic_flag = yaclib::detail::AtomicFlag<std::atomic_flag>;

}  // namespace yaclib_std
#else
#  include <atomic>

namespace yaclib_std {

using std::atomic_flag;

}  // namespace yaclib_std
#endif
