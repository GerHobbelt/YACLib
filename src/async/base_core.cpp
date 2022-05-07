#include <yaclib/async/detail/base_core.hpp>
#include <yaclib/async/detail/inline_core.hpp>
#include <yaclib/exe/executor.hpp>
#include <yaclib/exe/job.hpp>
#include <yaclib/log.hpp>
#include <yaclib/util/detail/nope_counter.hpp>

namespace yaclib::detail {

void PCore::Call() noexcept {
  YACLIB_DEBUG(true, "Pure virtual call");
}

void PCore::Drop() noexcept {
  YACLIB_DEBUG(true, "Pure virtual call");
}

void PCore::Here(PCore&, PCore::State) noexcept {
  YACLIB_DEBUG(true, "Pure virtual call");
}

static constexpr NopeCounter<IRef> kEmptyRef;

CCore::CCore(State state) noexcept : _state{state}, _caller{const_cast<NopeCounter<IRef>*>(&kEmptyRef)} {
}

void CCore::SetExecutor(IExecutor* executor) noexcept {
  // FIXME
  //  if (_executor) {
  //    _executor->DecRef();
  //  }
  //  if (executor) {
  //    executor->IncRef();
  //  }
  _executor = executor;
}

IExecutor* CCore::GetExecutor() const noexcept {
  return _executor;
}

void CCore::SetCall(CCore& callback) noexcept {
  callback._caller = this;  // move ownership
  if (!SetCallback(callback, kCall)) {
    Submit(callback);
  }
}

void CCore::SetHere(PCore& callback, State state) noexcept {
  YACLIB_DEBUG(state != kHereCall && state != kHereWrap, "");
  if (!SetCallback(callback, state)) {
    Submit(callback, state);
  }
}

bool CCore::SetWait(IRef& callback, State state) noexcept {
  YACLIB_DEBUG(state != kWait, "");
  return SetCallback(callback, state);
}

bool CCore::ResetWait() noexcept {
  auto expected = kWait;
  if (_state.load(std::memory_order_relaxed) != expected ||
      !_state.compare_exchange_strong(expected, kEmpty, std::memory_order_acquire, std::memory_order_relaxed)) {
    return false;  // want relaxed and need wait
  }
  YACLIB_DEBUG(_callback == nullptr, "callback empty, but should be our wait function");
  _callback = nullptr;  // want acquire and don't need wait
  return true;
}

void CCore::SetDrop(State state) noexcept {
  YACLIB_DEBUG(state != kWaitDetach && state != kWaitStop, "");
  if (!SetCallback(const_cast<NopeCounter<IRef>&>(kEmptyRef), state)) {
    _executor = nullptr;
    DecRef();
  }
}

bool CCore::Empty() const noexcept {
  YACLIB_DEBUG(_callback != nullptr, "callback not empty, so future already used");
  return _state.load(std::memory_order_acquire) == kEmpty;
}

bool CCore::Alive() const noexcept {
  return _state.load(std::memory_order_acquire) != kWaitStop;
}

void CCore::SetResult() noexcept {
  const auto state = _state.exchange(kResult, std::memory_order_acq_rel);
  auto* caller = std::exchange(_caller, const_cast<NopeCounter<IRef>*>(&kEmptyRef));
  YACLIB_DEBUG(caller == nullptr, "");
  caller->DecRef();
  auto* callback = std::exchange(_callback, nullptr);
  switch (state) {
    case kCall: {
      YACLIB_DEBUG(callback == nullptr, "");
      Submit(static_cast<CCore&>(*callback));
    } break;
    case kHereCall:
      [[fallthrough]];
    case kHereWrap: {
      YACLIB_DEBUG(callback == nullptr, "");
      YACLIB_DEBUG(caller != &kEmptyRef, "");
      Submit(static_cast<PCore&>(*callback), state);
    } break;
    case kWaitDetach:
      [[fallthrough]];
    case kWaitStop:
      _executor = nullptr;
      DecRef();
      [[fallthrough]];
    case kWait:
      YACLIB_DEBUG(callback == nullptr, "");
      callback->DecRef();
      [[fallthrough]];
    default:
      break;
  }
}

bool CCore::SetCallback(IRef& callback, State state) noexcept {
  YACLIB_DEBUG(_callback != nullptr, "callback not empty, so future already used");
  auto expected = kEmpty;
  if (_state.load(std::memory_order_acquire) != expected) {
    return false;  // want acquire here
  }
  _callback = &callback;
  if (!_state.compare_exchange_strong(expected, state, std::memory_order_release, std::memory_order_acquire)) {
    YACLIB_DEBUG(expected != kResult, "");
    _callback = nullptr;
    return false;  // want acquire here
  }
  return true;  // want release here
}

void CCore::Submit(CCore& callback) noexcept {
  YACLIB_DEBUG(_executor == nullptr, "we try to submit callback to executor, but don't see valid executor");
  auto* executor = std::exchange(_executor, nullptr);
  executor->Submit(callback);
  // FIXME executor.DecRef();
}

void CCore::Submit(PCore& callback, State state) noexcept {
  YACLIB_DEBUG(_caller != &kEmptyRef, "");
  YACLIB_DEBUG(_callback != nullptr, "");
  _executor = nullptr;
  callback.Here(*this, state);
  DecRef();
}

#ifdef YACLIB_LOG_DEBUG
CCore::~CCore() noexcept {
  auto state = _state.load(std::memory_order_acquire);
  YACLIB_DEBUG(state != kResult && state != kWaitStop, "Invalid state");
  YACLIB_DEBUG(_caller != &kEmptyRef, "Invalid state");
  YACLIB_DEBUG(_callback != nullptr, "Invalid state");
  YACLIB_DEBUG(_executor != nullptr, "Invalid state");
}
#endif

}  // namespace yaclib::detail
