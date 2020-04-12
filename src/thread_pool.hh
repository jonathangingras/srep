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

  inline size_t size() const { return threads.size(); }

  template <typename function_type, typename ...Args>
  void push(function_type &&function, Args &&...args);
};

template <typename function_type,
          typename call_return_type,
          typename exception_type,
          typename handler_type>
class exception_catcher {
  function_type function_;
  handler_type handler_;

public:
  inline exception_catcher(function_type &&function,
                           handler_type &&handler):
    function_(std::forward<function_type>(function)),
    handler_(std::forward<handler_type>(handler))
    {}

  template <typename ...Args>
  call_return_type operator () (Args &&...args);
};

template <typename exception_type, typename call_return_type, typename function_type, typename handler_type>
exception_catcher<function_type, call_return_type, exception_type, handler_type>
catch_exceptions(function_type &&function, handler_type &&handler);

}

#include "thread_pool_impl.hh"

#endif
