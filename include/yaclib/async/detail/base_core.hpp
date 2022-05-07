#pragma once

#include <yaclib/async/detail/inline_core.hpp>
#include <yaclib/exe/executor.hpp>
#include <yaclib/util/intrusive_ptr.hpp>
#include <yaclib/util/ref.hpp>

#include <yaclib_std/atomic>

namespace yaclib::detail {

class CCore : public PCore {
 public:
  void SetExecutor(IExecutor* executor) noexcept;
  [[nodiscard]] IExecutor* GetExecutor() const noexcept;

  void SetCall(CCore& callback) noexcept;
  void SetHere(PCore& callback, State state) noexcept;

  [[nodiscard]] bool SetWait(IRef& callback, State state) noexcept;
  [[nodiscard]] bool ResetWait() noexcept;
  void SetDrop(State state) noexcept;

  [[nodiscard]] bool Empty() const noexcept;
  [[nodiscard]] bool Alive() const noexcept;

  void SetResult() noexcept;

 protected:
  explicit CCore(State s) noexcept;

#ifdef YACLIB_LOG_DEBUG
  ~CCore() noexcept override;
#endif

  yaclib_std::atomic<State> _state;  //  TODO(MBkkt) merge with callback
  IRef* _caller{nullptr};
  IRef* _callback{nullptr};
  IExecutor* _executor{nullptr};

 private:
  bool SetCallback(IRef& callback, State state) noexcept;

  void Submit(CCore& callback) noexcept;
  void Submit(PCore& callback, State state) noexcept;
};

}  // namespace yaclib::detail
