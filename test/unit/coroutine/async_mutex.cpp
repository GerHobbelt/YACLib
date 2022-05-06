#include <util/time.hpp>

#include <yaclib/async/run.hpp>
#include <yaclib/config.hpp>
#include <yaclib/coroutine/async_mutex.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/future_traits.hpp>
#include <yaclib/executor/strand.hpp>
#include <yaclib/executor/submit.hpp>
#include <yaclib/executor/thread_pool.hpp>
#include <yaclib/fault/thread.hpp>

#include <exception>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace {

yaclib::Future<void> coro2(int& sum) {
  sum += 1;
  co_return;
}

TEST(AsyncMutex, JustWorks) {
  using namespace std::chrono_literals;

  yaclib::AsyncMutex m;

  auto tp = yaclib::MakeThreadPool();

  int sum = 0;
  const int N = 100000;
  std::array<yaclib::Future<void>, N> f;
  yaclib_std::atomic<int> done = 0;
  int i = 0;

  auto coro1 = [&]() -> yaclib::Future<void> {
    co_await m.Lock();  // lock
    auto tmp = sum;
    sum = tmp + 1;
    co_await m.Unlock();
    co_return;
  };

  for (i = 0; i < N; ++i) {
    yaclib::Submit(*tp, [&, i]() {
      f[i] = coro1();
      done.fetch_add(1, std::memory_order_seq_cst);
    });
  }

  while (!(done.load(std::memory_order_seq_cst) == N)) {
  }

  for (auto&& future : f) {
    std::move(future).Get();
  }
  EXPECT_EQ(N, sum);
  tp->HardStop();
  tp->Wait();
}

TEST(AsyncMutex, CoroOfDifferentTypes) {
  using namespace std::chrono_literals;

  yaclib::AsyncMutex m;

  auto tp = yaclib::MakeThreadPool();

  int sum = 0;
  const int N = 4000;
  std::array<yaclib::Future<void>, N> f;
  std::array<yaclib::Future<int>, N> fi;

  yaclib_std::atomic<int> done = 0;
  int i = 0;

  auto coro1 = [&]() -> yaclib::Future<void> {
    co_await m.Lock();  // lock
    auto tmp = sum;
    sum = tmp + 1;
    co_await m.Unlock();
    // m.SimpleUnlock();  // automatic unlocking
    co_return;
  };
  auto coro2 = [&]() -> yaclib::Future<int> {
    co_await m.Lock();  // lock
    auto tmp = sum;
    sum = tmp + 1;
    co_await m.Unlock();
    // m.SimpleUnlock();  // automatic unlocking
    co_return 42;
  };

  for (i = 0; i < N; ++i) {
    yaclib::Submit(*tp, [&, i]() {
      if (i % 2) {
        fi[i] = coro2();
        f[i] = coro1();
      } else {
        f[i] = coro1();
        fi[i] = coro2();
      }

      done.fetch_add(1, std::memory_order_seq_cst);
    });
  }

  while (!(done.load(std::memory_order_seq_cst) == N)) {
  }

  for (int i = 0; i < N; ++i) {
    std::move(f[i]).Get();
    std::move(fi[i]).Get();
  }
  EXPECT_EQ(2 * N, sum);
  tp->HardStop();
  tp->Wait();
}

TEST(AsyncMutex, Guard) {
  using namespace std::chrono_literals;

  yaclib::AsyncMutex m;

  auto tp = yaclib::MakeThreadPool();

  int sum = 0;
  const int N = 2000;
  std::array<yaclib::Future<void>, N> f;

  yaclib_std::atomic<int> done = 0;
  int i = 0;

  auto coro1 = [&]() -> yaclib::Future<void> {
    auto g = co_await m.Guard();  // lock
    auto tmp = sum;
    sum = tmp + 1;
    co_await g.Unlock();
    co_await g.Lock();
    tmp = sum;
    sum = tmp + 1;
    co_await g.Unlock();
  };

  for (i = 0; i < N; ++i) {
    yaclib::Submit(*tp, [&, i]() {
      f[i] = coro1();
      done.fetch_add(1, std::memory_order_seq_cst);
    });
  }

  while (!(done.load(std::memory_order_seq_cst) == N)) {
  }

  for (int i = 0; i < N; ++i) {
    std::move(f[i]).Get();
  }
  EXPECT_EQ(2 * N, sum);
  tp->HardStop();
  tp->Wait();
}

}  // namespace
