#ifndef _SREP_CHANNEL_HH_
#define _SREP_CHANNEL_HH_

#include <mutex>
#include <condition_variable>
#include "insertion_queue.hh"

namespace srep {

template <typename value_type_>
class channel {
public:
  typedef value_type_ value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type &&rvalue_reference;

private:
  insertion_queue<value_type> queue;
  std::mutex mutex;
  std::condition_variable condition_variable;

public:
  inline size_t size() const;

  void write(rvalue_reference rhs);
  template <typename input_stream_type>
  void write_from(input_stream_type &input, size_t n = 1);

  value_type read();
  template <typename output_stream_type>
  void read_in(output_stream_type &output, size_t n = 1);
};

}

#include "channel_impl.hh"

#endif
