#pragma once

#include <yaclib/async/detail/base_core.hpp>
#include <yaclib/config.hpp>
#include <yaclib/fault/atomic.hpp>
#include <yaclib/util/detail/atomic_counter.hpp>
#include <yaclib/util/detail/event_deleter.hpp>
#include <yaclib/util/detail/mutex_event.hpp>
#include <yaclib/util/type_traits.hpp>
#ifdef YACLIB_ATOMIC_EVENT
#  include <yaclib/util/detail/atomic_event.hpp>
#endif

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace yaclib::detail {

struct NoTimeoutTag {};

template <typename Event, typename Timeout, typename Range>
bool WaitRange(const Timeout& timeout, const Range& range, std::size_t count) {
  AtomicCounter<Event, EventDeleter> event{count + 1};
  // event ref counter = n + 1, it is optimization: we don't want to notify when return true immediately
  auto const wait_count = range([&](detail::BaseCore& core) {
    return core.Empty() && core.SetWait(event);
  });
  if (wait_count == 0 || event.SubEqual(count - wait_count + 1)) {
    return true;
  }
  auto token = event.Make();
  std::size_t reset_count = 0;
  if constexpr (!std::is_same_v<Timeout, NoTimeoutTag>) {
    // If you have problem with TSAN here, check this link: https://github.com/google/sanitizers/issues/1259
    // TLDR: new pthread function is not supported by thread sanitizer yet.
    if (event.Wait(token, timeout)) {
      return true;
    }
    reset_count = range([](detail::BaseCore& core) {
      return core.ResetWait();
    });
    if (reset_count != 0 && (reset_count == wait_count || event.SubEqual(reset_count))) {
      return false;
    }
    // We know we have `wait_count - reset_count` Results, but we must wait until event was not used by cores
  }
  event.Wait(token);
  return reset_count == 0;
}

template <typename Event, typename Timeout, typename... Cores>
bool WaitCore(const Timeout& timeout, Cores&... cores) {
  static_assert(sizeof...(cores) >= 1, "Number of futures must be at least one");
  static_assert((... && std::is_same_v<detail::BaseCore, Cores>), "Futures must be Future in Wait function");
  auto range = [&](auto&& functor) {
    return (... + static_cast<std::size_t>(functor(cores)));
  };
  return WaitRange<Event>(timeout, range, sizeof...(cores));
}

template <typename Event, typename Timeout, typename Iterator>
bool WaitIterator(const Timeout& timeout, Iterator it, std::size_t size) {
  static_assert(is_future_v<typename std::iterator_traits<Iterator>::value_type>,
                "Wait function Iterator must be point to some Future");
  if (size == 0) {
    return true;
  }
  auto range = [&](auto&& functor) {
    auto temp_it = it;
    std::size_t count = 0;
    for (std::size_t i = 0; i != size; ++i) {
      count += static_cast<std::size_t>(functor(*temp_it->GetCore()));
      ++temp_it;
    }
    return count;
  };
  return WaitRange<Event>(timeout, range, size);
}

extern template bool WaitCore<MutexEvent, NoTimeoutTag, BaseCore>(const NoTimeoutTag&, BaseCore&);

#ifdef YACLIB_ATOMIC_EVENT
extern template bool WaitCore<AtomicEvent, NoTimeoutTag, BaseCore>(const NoTimeoutTag&, BaseCore&);
using DefaultEvent = AtomicEvent;
#else
using DefaultEvent = MutexEvent;
#endif

}  // namespace yaclib::detail
