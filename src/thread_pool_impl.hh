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

template <typename function_type,
          typename call_return_type,
          typename exception_type,
          typename handler_type>
template <typename ...Args>
call_return_type
exception_catcher<function_type, call_return_type, exception_type, handler_type>::operator () (Args &&...args) {
  try {
    return function_(std::forward<Args>(args)...);
  } catch (exception_type &error) {
    return handler_(error);
  }
}

template <typename exception_type, typename call_return_type, typename function_type, typename handler_type>
exception_catcher<function_type, call_return_type, exception_type, handler_type>
catch_exceptions(function_type &&function, handler_type &&handler) {
  return exception_catcher<function_type, call_return_type, exception_type, handler_type>(
    std::forward<function_type>(function),
    std::forward<handler_type>(handler)
    );
}

}

#endif
