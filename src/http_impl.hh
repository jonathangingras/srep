#ifndef _SREP_HTTP_IMPL_HH_
#define _SREP_HTTP_IMPL_HH_

#ifndef _SREP_HTTP_HH_
#include "http.hh"
#endif

namespace srep {
namespace http {

template <typename output_type>
output_type header_base::optional_field::get() const {
  if(!exist()) { throw std::logic_error("field does not exist"); }
  std::istringstream element_stream(it->second);
  output_type output;
  element_stream >> output;
  return output;
}

template <>
inline std::string header_base::optional_field::get() const {
  if(!exist()) { throw std::logic_error("field does not exist"); }
  return it->second;
}

template <typename output_type, output_type default_value>
output_type header_base::get_or_default(const std::string &key) const {
  optional_field accessor = access(key);
  if (accessor.exist()) {
    return accessor.get<output_type>();
  } else {
    return default_value;
  }
}

}
}

#endif
