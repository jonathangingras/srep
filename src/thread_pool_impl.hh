#ifndef _SREP_THREAD_POOL_IMPL_HH_
#define _SREP_THREAD_POOL_IMPL_HH_

#ifndef _SREP_THREAD_POOL_HH_
#include "thread_pool.hh"
#endif

#include <stdexcept>

namespace srep {

template <typename function_type, typename ...Args>
void thread_pool::push(function_type &&function, Args &&...args) {
  if (should_end) { throw std::logic_error("thread pool is in ending state"); };
  {
    std::lock_guard<std::mutex> lock(mutex);
    threads.push(std::thread(std::forward<function_type>(function), std::forward<Args>(args)...));
  }
  condition_variable.notify_one();
}

}

#endif
