#ifndef _SREP_THREAD_POOL_HH_
#define _SREP_THREAD_POOL_HH_

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

namespace srep {

class thread_pool {
  std::atomic<bool> should_end;
  std::mutex mutex;
  std::condition_variable condition_variable;
  std::thread joiner;
  std::queue<std::thread> threads;

public:
  thread_pool();
  ~thread_pool();

  template <typename function_type, typename ...Args>
  void push(function_type &&function, Args &&...args);
};

}

#include "thread_pool_impl.hh"

#endif
