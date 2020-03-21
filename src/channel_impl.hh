#ifndef _SREP_CHANNEL_IMPL_HH_
#define _SREP_CHANNEL_IMPL_HH_

#ifndef _SREP_CHANNEL_HH_
#include "channel.hh"
#endif

#include <utility>

namespace srep {

template <typename value_type>
size_t channel<value_type>::size() const {
  return queue.size();
}

template <typename value_type>
void channel<value_type>::write(rvalue_reference rhs) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    queue.insert(std::forward<value_type>(rhs));
  }
  condition_variable.notify_all();
}

template <typename value_type>
template <typename input_stream_type>
void channel<value_type>::write_from(input_stream_type &input, size_t n) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < n; ++i) { input >> queue; }
  }
  condition_variable.notify_all();
}

template <typename value_type>
value_type channel<value_type>::read() {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&] { return queue.size() >= 1; });
  value_type output(std::move(queue.top()));
  queue.pop();
  return output;
}

template <typename value_type>
template <typename output_stream_type>
void channel<value_type>::read_in(output_stream_type &output, size_t n) {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&] { return queue.size() >= n; });
  for (size_t i = 0; i < n; ++i) {
    output << std::move(queue.top());
    queue.pop();
  }
}

}

#endif
