#undef YACLIB_LOG_ERROR
#define YACLIB_LOG_ERROR

#include <yaclib/config.hpp>
#include <yaclib/log.hpp>

#include <utility>

namespace yaclib {

static LogCallback g_error_callback = nullptr;

void SetErrorCallback(LogCallback callback) {
  g_error_callback = std::move(callback);
}

void detail::LogError(std::string_view file, std::size_t line, std::string_view function, std::string_view condition,
                      std::string_view message) {
  // GCOV_EXCL_START
  if (g_error_callback == nullptr) {
    return;
  }
  g_error_callback(file, line, function, condition, message);
  // GCOV_EXCL_STOP
}
}  // namespace yaclib
