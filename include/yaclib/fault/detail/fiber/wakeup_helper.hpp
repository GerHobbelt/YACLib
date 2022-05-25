#pragma once

namespace yaclib::detail::fiber {

class Fiber;

void ScheduleFiber(Fiber* fiber);

}  // namespace yaclib::detail::fiber
