#ifndef _SREP_SERIALIZABLE_STATE_IMPL_HH_
#define _SREP_SERIALIZABLE_STATE_IMPL_HH_

#ifndef _SREP_SERIALIZABLE_STATE_HH_
#include "serializable_state.hh"
#endif

#include <type_traits>

namespace srep {

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type>
struct state_update_deserializer_type_traits {
  typedef std::nullptr_t deserializer_type;
};

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type,
          typename deserializer_type_traits,
          typename ...Args>
constexpr std::basic_istream<char_type, char_traits_type>
&deserialize_from(std::basic_istream<char_type, char_traits_type> &input,
                  std::unique_ptr<serializable_state_update<
                    state_type, serialization_format_type, char_type, char_traits_type> > &deserialized,
                  Args &&...args) {
  static_assert(!std::is_same<std::nullptr_t, typename deserializer_type_traits::deserializer_type>(),
                "the deserializer_type is not defined in any specialization of srep::state_update_deserialization_type_traits");

  typename deserializer_type_traits::deserializer_type deserializer(std::forward<Args>(args)...);
  deserialized = deserializer.deserialize_from(input);

  return input;
}

}

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type>
constexpr std::basic_ostream<char_type, char_traits_type>
&operator << (std::basic_ostream<char_type, char_traits_type> &output,
              const srep::serializable_state_update<
                state_type, serialization_format_type, char_type, char_traits_type> &serialized) {
  return serialized.serialize_to(output);
}

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type>
constexpr std::basic_istream<char_type, char_traits_type>
&operator >> (std::basic_istream<char_type, char_traits_type> &input,
              std::unique_ptr<srep::serializable_state_update<
                state_type, serialization_format_type, char_type, char_traits_type> > &deserialized) {
  typedef typename srep::state_update_deserializer_type_traits<
    state_type, serialization_format_type, char_type, char_traits_type> deserializer_type_traits;

  return srep::deserialize_from<state_type,
                                serialization_format_type,
                                char_type,
                                char_traits_type,
                                deserializer_type_traits>(input, deserialized);
}

#endif
