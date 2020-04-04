#include <atomic>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include "../src/thread_pool.hh"

TEST(ThreadPool, CanRunThreads) {
  size_t n_threads = 100;
  std::atomic<size_t> i = 0;

  {
    srep::thread_pool pool;

    for(size_t j = 0; j < n_threads; ++j) {
      pool.push([&] { ++i; });
    }
  }

  EXPECT_EQ(n_threads, i);
}

TEST(ThreadPool, CanRunNoThreadsAtAll) {
  {
    srep::thread_pool pool;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
