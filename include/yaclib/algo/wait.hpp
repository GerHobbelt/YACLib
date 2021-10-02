#pragma once

#include <yaclib/algo/detail/wait_impl.hpp>
#include <yaclib/async/future.hpp>

namespace yaclib {

/**
 * Wait until \ref Ready becomes true
 *
 * \param fs one or more futures to wait
 */
template <typename... T>
void Wait(Future<T>&... fs) {
  detail::Wait(detail::NoTimeoutTag{}, static_cast<detail::BaseCore&>(*fs.GetCore())...);
}

/**
 * Wait until \ref Ready becomes true
 *
 * \param begin Iterator to futures to wait
 * \param end Iterator to futures to wait
 */
template <typename Iterator>
std::enable_if_t<!util::IsFutureV<Iterator>, void> Wait(Iterator begin, Iterator end) {
  detail::Wait(detail::NoTimeoutTag{}, begin, begin, end);
}

/**
 * Wait until \ref Ready becomes true
 *
 * \param begin Iterator to futures to wait
 * \param size count of futures to wait
 */
template <typename Iterator>
void Wait(Iterator begin, size_t size) {
  detail::Wait(detail::NoTimeoutTag{}, begin, 0, size);
}

}  // namespace yaclib
