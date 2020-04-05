#ifndef _SREP_HTTP_HH_
#define _SREP_HTTP_HH_

#include <istream>
#include <ostream>
#include <map>
#include <string>
#include <sstream>
#include <stdexcept>

namespace srep {
namespace http {

constexpr const char *endl() { return "\r\n"; }

class header_base;

}
}

std::istream &operator >> (std::istream &input, srep::http::header_base &header);
std::ostream &operator << (std::ostream &output, const srep::http::header_base &header);

namespace srep {
namespace http {

class header_base {
  friend std::istream &::operator >> (std::istream &input, header_base &header);
  friend std::ostream &::operator << (std::ostream &output, const header_base &header);

protected:
  std::map<std::string, std::string> fields;

  inline header_base(std::initializer_list<std::pair<const std::string, std::string> > init):
    fields(init)
    {}

  void write_fields(std::ostream &output) const;
  void parse_fields(std::istream &input);
  size_t fields_size() const;

  struct optional_field {
    const header_base &header;
    std::map<std::string, std::string>::const_iterator it;

    inline bool exist() const { return it != header.fields.end(); }

    template <typename output_type>
    output_type get() const;
  };

  inline optional_field access(const std::string &key) const {
    return {*this, fields.find(key)};
  }

  template <typename output_type, output_type default_value>
  output_type get_or_default(const std::string &key) const;

  std::string get_or_default_str(const std::string &key, const std::string &default_value="") const;

  virtual size_t size() const = 0;
  virtual void parse_in(std::istream &input) = 0;
  virtual void write_specific(std::ostream &output) const = 0;

public:
  operator std::string () const;

  inline void emplace(std::string &&key, std::string &&value) { fields.emplace(std::forward<std::string>(key), std::forward<std::string>(value)); }

  inline std::size_t content_length() const { return get_or_default<std::size_t, 0>("Content-Length:"); }
  inline std::string content_type() const { return get_or_default_str("Content-Type:"); }
};

class client_header: public header_base {
  std::string method_;
  std::string route_;

  void parse_in(std::istream &input);
  void write_specific(std::ostream &output) const;

public:
  inline client_header(
    const std::string &method="GET",
    const std::string &route="/",
    std::initializer_list<std::pair<const std::string, std::string> > init_fields={}):
    header_base(init_fields),
    method_(method),
    route_(route)
    {}

  static constexpr const char *default_user_agent() { return "srep"; }
  static constexpr const char *default_accept() { return "*/*"; }

  bool expect_continue() const;

  // TODO: test this method
  size_t size() const;

  inline const std::string &method() const { return method_; }
  inline const std::string &route() const { return route_; }

  // TODO: remove the following 3 methods, those shortcuts are out of scope for this class
  inline std::string host() const { return get_or_default_str("Host:"); }
  inline std::string user_agent() const { return get_or_default_str("User-Agent:", default_user_agent()); }
  inline std::string accept() const { return get_or_default_str("Accept:", default_accept()); }
};

class server_header: public header_base {
  unsigned short status_code_;
  std::string status_message_;

  void parse_in(std::istream &input);
  void write_specific(std::ostream &output) const;

public:
  inline server_header(
    unsigned short status_code=200,
    const std::string &status_message="OK",
    std::initializer_list<std::pair<const std::string, std::string> > init_fields={}):
    header_base(init_fields),
    status_code_(status_code),
    status_message_(status_message)
    {}

  // TODO: test this method
  size_t size() const;

  inline const unsigned short &status_code() const { return status_code_; }
  inline const std::string &status_message() const { return status_message_; }
};

}
}

#include "http_impl.hh"

#endif
