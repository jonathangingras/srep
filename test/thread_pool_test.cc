#include "../src/thread_pool.hh"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>

TEST(ThreadPool, CanRunThreads) {
  size_t n_threads = 100;
  std::atomic<size_t> i = 0;

  {
    srep::thread_pool pool;

    for (size_t j = 0; j < n_threads; ++j) {
      pool.push([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ++i;
      });
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

TEST(ThreadPool, CanUseAnExceptionCatcher) {
  std::ostringstream oss;
  {
    srep::thread_pool pool;

    pool.push(srep::catch_exceptions<std::runtime_error, int>(
                  [](int i) -> int {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    throw std::runtime_error("TEST " + std::to_string(i));
                  },
                  [&oss](std::runtime_error &error) -> int {
                    oss << error.what();
                    return 0;
                  }),
              42);
  }

  EXPECT_EQ("TEST 42", oss.str());
}
