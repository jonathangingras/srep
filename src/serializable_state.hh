#ifndef _SREP_SERIALIZABLE_STATE_HH_
#define _SREP_SERIALIZABLE_STATE_HH_

#include <ostream>
#include <istream>
#include "state.hh"

namespace srep {

template <typename state_type, typename _serialization_format_type,
          typename CharT = char, typename Traits = std::char_traits<CharT> >
class serializable_state_update: public state_update<state_type> {
public:
  typedef _serialization_format_type serialization_format_type;
  typedef std::unique_ptr<
    serializable_state_update<
      state_type, serialization_format_type, CharT, Traits> > ptr;

  virtual std::basic_ostream<CharT, Traits> &serialize_to(std::basic_ostream<CharT, Traits> &output) = 0;
};

template <typename state_type, typename serialization_format_type,
          typename CharT = char, typename Traits = std::char_traits<CharT> >
class state_update_deserializer {
public:
  virtual typename serializable_state_update<
  state_type, serialization_format_type, CharT, Traits>::ptr
  deserialize_from(std::basic_istream<CharT, Traits> &input) = 0;
};

template <typename state_type, typename serialization_format_type,
          typename CharT = char, typename Traits = std::char_traits<CharT> >
struct state_update_deserialization_traits;

template <typename state_type, typename serialization_format_type,
          typename CharT = char, typename Traits = std::char_traits<CharT> >
struct state_update_deserialization_type_traits;

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits,
          typename ...Args>
constexpr std::basic_istream<CharT, Traits>
&deserialize_from(std::basic_istream<CharT, Traits> &input,
                  std::unique_ptr<serializable_state_update<
                    state_type, serialization_format_type, CharT, Traits> > &deserialized,
                  Args &&...args);

}

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
constexpr std::basic_ostream<CharT, Traits>
&operator << (std::basic_ostream<CharT, Traits> &output,
              srep::serializable_state_update<
                state_type, serialization_format_type, CharT, Traits> &serialized);

template <typename state_type, typename serialization_format_type,
          typename CharT, typename Traits>
constexpr std::basic_istream<CharT, Traits>
&operator >> (std::basic_istream<CharT, Traits> &input,
              std::unique_ptr<srep::serializable_state_update<
                state_type, serialization_format_type, CharT, Traits> > &deserialized);

#include "serializable_state_impl.hh"

#endif
