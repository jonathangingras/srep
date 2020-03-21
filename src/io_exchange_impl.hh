#ifndef _SREP_IO_EXCHANGE_IMPL_HH_
#define _SREP_IO_EXCHANGE_IMPL_HH_

#ifndef _SREP_IO_EXCHANGE_HH_
#include "io_exchange.hh"
#endif

namespace srep {

template <typename output_container_type>
constexpr void
forward_output_traits<output_container_type>::read_in(output_container_type &output,
                                                      typename output_container_type::value_type &&input) {
  output.push(std::forward<typename output_container_type::value_type>(input));
}

template <typename output_container_type>
forward_output<output_container_type>
&forward_output<output_container_type>::operator << (typename output_container_type::value_type &&input) {
  forward_output_traits<output_container_type>::read_in(
    container,
    std::forward<typename output_container_type::value_type>(input));
  return *this;
}

template <typename input_container_type>
constexpr typename input_container_type::value_type
&forward_input_traits<input_container_type>::get_moveable(input_container_type &input) {
    return input.front();
}

template <typename input_container_type>
constexpr void forward_input_traits<input_container_type>::erase_moveable(input_container_type &input) {
  input.pop_front();
}

template <typename input_container_type>
template <typename output_container_type>
forward_input<input_container_type>
&forward_input<input_container_type>::operator >> (output_container_type &output) {
  forward_output_traits<output_container_type>::read_in(
    output,
    std::move(forward_input_traits<input_container_type>::get_moveable(input)));
  forward_input_traits<input_container_type>::erase_moveable(input);
  return *this;
}

}

#endif
