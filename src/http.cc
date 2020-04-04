#include <vector>
#include <array>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include "http.hh"

std::string srep::http::header_base::get_or_default_str(const std::string &key, const std::string &default_value) const {
  optional_field accessor = access(key);
  if (accessor.exist()) {
    return accessor.get<std::string>();
  } else {
    return default_value;
  }
}

static void parse_multi_word(std::string &output, std::istream &input) {
  std::ostringstream oss;
  int words = 0;
  while (input.good() && input.get() != '\r') {
    std::string next;
    input.unget();
    input >> next;

    if(words > 0) {
      oss << " ";
    }
    oss << next;
    ++words;
  }

  output = oss.str();
}

bool srep::http::client_header::expect_continue() const {
  auto e = access("Expect:");
  if(!e.exist()) {
    return false;
  } else {
    return e.get<std::string>() == "100-continue";
  }
}

void srep::http::header_base::parse_fields(std::istream &input) {
  while(input.good()) {
    std::string field, value;
    input >> field;
    if(field[field.size() - 1] == ':') {
      parse_multi_word(value, input);
      fields.insert_or_assign(std::move(field), std::move(value));
    }
  }
}

void srep::http::header_base::write_fields(std::ostream &output) const {
  if (!access("Content-Length:").exist()) {
    output << "Content-Length: 0" << http::endl();
  }
  for (const auto &pair: fields) {
    output << pair.first << " " << pair.second << http::endl();
  }
}

srep::http::header_base::operator std::string () const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

void srep::http::client_header::parse_in(std::istream &input) {
  if (input.good()) {
    input >> method_;
    input >> route_;
    std::string protocol;
    input >> protocol;

    if(protocol != "HTTP/1.1") {
      //TODO: deal with bad format
    }

    parse_fields(input);
  }
}

void srep::http::client_header::write_specific(std::ostream &output) const {
  output << method() << " " << route() << " " << "HTTP/1.1" << http::endl();
  write_fields(output);

  for (const auto &pair: std::initializer_list<std::pair<std::string, std::string> >
         {
           {"User-Agent:", default_user_agent()},
           {"Accept:", default_accept()},
         }) {
    if (!access(pair.first).exist()) {
      output << pair.first << " " << pair.second << http::endl();
    }
  }

  output << http::endl();
}

void srep::http::server_header::parse_in(std::istream &input) {
  std::string str;
  input >> str;
  if (str == "HTTP/1.1") {
    input >> status_code_;
    parse_multi_word(status_message_, input);

    parse_fields(input);
  } else {
    //TODO: deal bad format
  }
}

void srep::http::server_header::write_specific(std::ostream &output) const {
  output << "HTTP/1.1 " << status_code() << " " << status_message() << http::endl();
  write_fields(output);
  output << http::endl();
}

template <size_t ending_size>
bool valid_ending(const std::vector<char> &chars, const std::array<char, ending_size> &ending) {
  if (chars.size() < ending_size) {
    return false;
  }

  return std::memcmp(chars.data()+(chars.size()-ending_size), ending.data(), ending_size) == 0;
}

std::istream &operator >> (std::istream &input, srep::http::header_base &header) {
  std::vector<char> chars;
  std::array<char, 4> ending {'\r', '\n', '\r', '\n'};

  while(input.good() && !valid_ending(chars, ending)) {
    chars.push_back(input.get());
  }

  if (!valid_ending(chars, ending)) {
    throw std::runtime_error("invalid http header format");
  }

  chars.push_back(0);
  std::istringstream iss(chars.data());
  header.parse_in(iss);

  return input;
}

std::ostream &operator << (std::ostream &output, const srep::http::header_base &header) {
  header.write_specific(output);
  return output;
}
