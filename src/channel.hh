#ifndef _SREP_CHANNEL_HH_
#define _SREP_CHANNEL_HH_

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "insertion_queue.hh"

namespace srep {

template <typename queue_type>
class basic_channel {
public:
  typedef typename queue_type::value_type value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type &&rvalue_reference;

private:
  queue_type queue;
  std::mutex mutex;
  std::condition_variable condition_variable;
  std::atomic<bool> preempted = false;

public:
  inline size_t size() const;

  void write(rvalue_reference rhs);
  template <typename input_stream_type>
  void write_from(input_stream_type &input, size_t n = 1);

  value_type read();
  template <typename output_stream_type>
  void read_in(output_stream_type &output, size_t n = 1);

  void preempt();
  void read_preemptable(value_type &read);
};

}

#include "channel_impl.hh"
#include <queue>

namespace srep {

template <typename value_type>
using channel = basic_channel<std::queue<value_type> >;

};

#endif
