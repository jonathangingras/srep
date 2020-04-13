#ifndef _SREP_TYPE_ASSERTIONS_HH_
#define _SREP_TYPE_ASSERTIONS_HH_

#include <type_traits>
#include <string>

namespace srep {

template <typename state_type>
struct has_increase_operator {
  enum { value = false };
};

template <>
struct has_increase_operator<std::string> {
  enum { value = true };
};

template <typename state_type>
struct has_decrease_operator {
  enum { value = false };
};

template <typename state_type>
struct is_replace_by_supported : std::integral_constant<bool,
                                                        std::is_assignable<state_type&, state_type&&>::value ||
                                                        std::is_assignable<state_type&, state_type>::value> {};

template <typename state_type>
struct is_addition_supported : std::integral_constant<bool,
                                                      std::is_arithmetic<state_type>::value ||
                                                      has_increase_operator<state_type>::value> {};

template <typename state_type>
struct is_substraction_supported : std::integral_constant<bool,
                                                          std::is_arithmetic<state_type>::value ||
                                                          has_decrease_operator<state_type>::value> {};

}

#endif
