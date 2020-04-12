#ifndef _SREP_CHANNEL_IMPL_HH_
#define _SREP_CHANNEL_IMPL_HH_

#ifndef _SREP_CHANNEL_HH_
#include "channel.hh"
#endif

#include <utility>

namespace srep {

template <typename queue_type>
size_t basic_channel<queue_type>::size() const {
  return queue.size();
}

template <typename queue_type>
void basic_channel<queue_type>::write(rvalue_reference rhs) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::forward<value_type>(rhs));
  }
  condition_variable.notify_all();
}

template <typename queue_type>
template <typename input_stream_type>
void basic_channel<queue_type>::write_from(input_stream_type &input, size_t n) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    for (size_t i = 0; i < n; ++i) { input >> queue; }
  }
  condition_variable.notify_all();
}

template <typename queue_type>
typename queue_type::value_type basic_channel<queue_type>::read() {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&] { return queue.size() >= 1; });
  value_type output(std::move(queue.front()));
  queue.pop();
  return output;
}

template <typename queue_type>
template <typename output_stream_type>
void basic_channel<queue_type>::read_in(output_stream_type &output, size_t n) {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&] { return queue.size() >= n; });
  for (size_t i = 0; i < n; ++i) {
    output << std::move(queue.front());
    queue.pop();
  }
}

template <typename queue_type>
void basic_channel<queue_type>::preempt() {
  preempted = true;
  condition_variable.notify_all();
}

template <typename queue_type>
void basic_channel<queue_type>::read_preemptable(value_type &read) {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&] { return queue.size() >= 1 || preempted; });
  preempted = false;
  if (queue.size() >= 1) {
    read = std::move(queue.front());
    queue.pop();
  }
}

}

#endif
