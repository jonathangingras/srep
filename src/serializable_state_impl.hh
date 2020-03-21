#ifndef _SREP_SERIALIZABLE_STATE_IMPL_HH_
#define _SREP_SERIALIZABLE_STATE_IMPL_HH_

#ifndef _SREP_SERIALIZABLE_STATE_HH_
#include "serializable_state.hh"
#endif

#include <type_traits>

namespace srep {

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
struct state_update_deserialization_traits {
  static constexpr typename serializable_state_update<
    state_type, serialization_format_type, CharT, Traits>::ptr
  deserialize(state_update_deserializer<state_type, serialization_format_type, CharT, Traits> &deserializer,
              std::basic_istream<CharT, Traits> &input) {
    return deserializer.deserialize_from(input);
  }
};

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
struct state_update_deserialization_type_traits {
  typedef std::nullptr_t deserializer_type;
};

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits,
          typename ...Args>
constexpr std::basic_istream<CharT, Traits>
&deserialize_from(std::basic_istream<CharT, Traits> &input,
                  std::unique_ptr<serializable_state_update<
                    state_type, serialization_format_type, CharT, Traits> > &deserialized,
                  Args &&...args) {
  static_assert(!std::is_same<std::nullptr_t,
                typename srep::state_update_deserialization_type_traits<
                state_type, serialization_format_type, CharT, Traits>::deserializer_type>(),
                "the deserializer_type is not defined in any specialization of srep::state_update_deserialization_type_traits");

  typename srep::state_update_deserialization_type_traits<
    state_type, serialization_format_type, CharT, Traits>::deserializer_type
    deserializer(std::forward<Args>(args)...);

  deserialized = srep::state_update_deserialization_traits<
    state_type, serialization_format_type, CharT, Traits>::deserialize(
      deserializer, input);

  return input;
}

}

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
constexpr std::basic_ostream<CharT, Traits>
&operator << (std::basic_ostream<CharT, Traits> &output,
              srep::serializable_state_update<
                state_type, serialization_format_type, CharT, Traits> &serialized) {
  return serialized.serialize_to(output);
}

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
constexpr std::basic_istream<CharT, Traits>
&operator >> (std::basic_istream<CharT, Traits> &input,
              std::unique_ptr<srep::serializable_state_update<
                state_type, serialization_format_type, CharT, Traits> > &deserialized) {
  return srep::deserialize_from(input, deserialized);
}

#endif
