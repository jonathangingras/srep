#include <chrono>
#include <random>
#include <atomic>

#include <gtest/gtest.h>

#include "../src/barrier.hh"

TEST(Barrier, DestroyJoinsAllThreads) {
  std::uniform_int_distribution<int> d(0, 100);
  std::random_device rd;
  size_t n_threads = 1000;
  std::atomic<size_t> count { 0 };

  {
    srep::barrier barrier;
    for (size_t i = 0; i < n_threads; ++i) {
      barrier.push([&] {
                     std::this_thread::sleep_for(std::chrono::nanoseconds(d(rd)));
                     ++count;
                   });
    }
  }

  EXPECT_EQ(count, n_threads);
}
