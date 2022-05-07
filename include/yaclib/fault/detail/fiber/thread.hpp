#pragma once

#include <yaclib/fault/detail/fiber/queue.hpp>
#include <yaclib/fault/detail/fiber/scheduler.hpp>
#include <yaclib/log.hpp>
#include <yaclib/util/func.hpp>

#include <functional>
#include <thread>

namespace yaclib::detail::fiber {

class Thread {
 public:
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  using id = Fiber::Id;
  using native_handle_type = std::thread::native_handle_type;

  template <typename Fp, typename... Args>
  inline explicit Thread(Fp&& f, Args&&... args) {
    yaclib::IFuncPtr func = yaclib::MakeFunc([&, f = std::forward<Fp>(f)]() mutable {
      f(std::forward(args)...);
    });
    _impl = new Fiber(func);
    _impl->SetCompleteCallback(yaclib::MakeFunc([&]() mutable {
      _join_queue.NotifyAll();
    }));
    Scheduler::GetScheduler()->Schedule(_impl);
  }

  Thread() noexcept;
  Thread(Thread&& t) noexcept;
  Thread& operator=(Thread&& t) noexcept;
  ~Thread();

  void swap(Thread& t) noexcept;
  [[nodiscard]] bool joinable() const noexcept;
  void join();
  void detach();

  [[nodiscard]] id get_id() const noexcept;

  native_handle_type native_handle() noexcept;

  static unsigned int hardware_concurrency() noexcept;

 private:
  void AfterJoinOrDetach();

  Fiber* _impl{nullptr};
  FiberQueue _join_queue;
  bool _joined_or_detached{false};
};

}  // namespace yaclib::detail::fiber
