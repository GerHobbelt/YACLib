#include <yaclib/async/future.hpp>

namespace yaclib {

template class Future<void>;

Future<void> MakeFuture() {
  // TODO(MBkkt) Do we really need this?
  return Future<void>{util::MakeIntrusive<detail::ResultCore<void>>(util::Result<void>::Default())};
}

}  // namespace yaclib
