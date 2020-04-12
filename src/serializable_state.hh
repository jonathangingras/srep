#ifndef _SREP_SERIALIZABLE_STATE_HH_
#define _SREP_SERIALIZABLE_STATE_HH_

#include <ostream>
#include <istream>
#include "state.hh"

namespace srep {

template <typename state_type,
          typename _serialization_format_type,
          typename char_type = char,
          typename char_traits_type = std::char_traits<char_type> >
class serializable_state_update: public state_update<state_type> {
public:
  typedef _serialization_format_type serialization_format_type;
  typedef std::unique_ptr<serializable_state_update<state_type, serialization_format_type, char_type, char_traits_type> > ptr;

  virtual std::basic_ostream<char_type, char_traits_type> &serialize_to(std::basic_ostream<char_type, char_traits_type> &output) const = 0;
};

template <typename state_type,
          typename serialization_format_type,
          typename char_type = char,
          typename char_traits_type = std::char_traits<char_type> >
class state_update_deserializer {
public:
  virtual typename serializable_state_update<state_type, serialization_format_type, char_type, char_traits_type>::ptr
    deserialize_from(std::basic_istream<char_type, char_traits_type> &input) = 0;
};

template <typename state_type,
          typename serialization_format_type,
          typename char_type = char,
          typename char_traits_type = std::char_traits<char_type> >
struct state_update_deserializer_type_traits;

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type,
          typename deserializer_type_traits =
            state_update_deserializer_type_traits<state_type,
                                                  serialization_format_type,
                                                  char_type,
                                                  char_traits_type>,
          typename ...Args>
constexpr std::basic_istream<char_type, char_traits_type>
&deserialize_from(std::basic_istream<char_type, char_traits_type> &input,
                  std::unique_ptr<serializable_state_update<
                  state_type, serialization_format_type, char_type, char_traits_type> > &deserialized,
                  Args &&...args);

}

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type>
constexpr std::basic_ostream<char_type, char_traits_type>
&operator << (std::basic_ostream<char_type, char_traits_type> &output,
              const srep::serializable_state_update<
                state_type, serialization_format_type, char_type, char_traits_type> &serialized);

template <typename state_type,
          typename serialization_format_type,
          typename char_type,
          typename char_traits_type>
constexpr std::basic_istream<char_type, char_traits_type>
&operator >> (std::basic_istream<char_type, char_traits_type> &input,
              std::unique_ptr<srep::serializable_state_update<
                state_type, serialization_format_type, char_type, char_traits_type> > &deserialized);

#include "serializable_state_impl.hh"

#endif
