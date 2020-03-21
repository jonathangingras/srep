#ifndef _SREP_IO_EXCHANGE_HH_
#define _SREP_IO_EXCHANGE_HH_

#include <utility>

namespace srep {

template <typename output_container_type>
struct forward_output_traits {
  static constexpr void read_in(output_container_type &output,
                                typename output_container_type::value_type &&input);
};

template <typename output_container_type>
struct forward_output {
  output_container_type &container;

  forward_output &operator << (typename output_container_type::value_type &&input);
};

template <typename input_container_type>
struct forward_input_traits {
  static constexpr typename input_container_type::value_type &get_moveable(input_container_type &input);
  static constexpr void erase_moveable(input_container_type &input);
};

template <typename input_container_type>
struct forward_input {
  input_container_type &input;

  template <typename output_container_type>
  forward_input &operator >> (output_container_type &output);
};

}

#include "io_exchange_impl.hh"

#endif
