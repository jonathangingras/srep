#include <set>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <gtest/gtest.h>
#include "../src/http.hh"


static std::set<std::string> split_header_str(const std::string &header_str) {
  std::set<std::string> set;
  boost::split(set, header_str, boost::is_any_of("\r\n"));
  auto empty = set.find("");
  if (empty != set.end()) {
    set.erase(empty);
  }
  return set;
}

TEST(StringTester, CanSplitToSet) {
  std::string str = "hello\r\nhello2\r\n\r\n";
  auto set = split_header_str(str);

  EXPECT_EQ(std::set<std::string>({"hello", "hello2"}), set);
}

TEST(HTTPClient, CanParseHeader) {
  srep::http::client_header header;
  const std::string str_header[] {
                                "POST / HTTP/1.1",
                                  "Host: google.com",
                                  "User-Agent: curl/7.69.1",
                                  "Accept: */*",
                                  "Content-Length: 9",
                                  "Content-Type: application/x-www-form-urlencoded",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::istringstream iss(oss.str());

  iss >> header;

  EXPECT_EQ("POST", header.method());
  EXPECT_EQ("google.com", header.host());
  EXPECT_EQ("curl/7.69.1", header.user_agent());
  EXPECT_EQ(std::size_t(9), header.content_length());
  EXPECT_EQ("application/x-www-form-urlencoded", header.content_type());
}

TEST(HTTPClient, CanSerializeDefaultHeader) {
  const std::string str_header[] {
                                "GET / HTTP/1.1",
                                  "Content-Length: 0",
                                  "User-Agent: srep",
                                  "Accept: */*",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::client_header header;
  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPClient, CanSerializeFilledHeader) {
  const std::string str_header[] {
                                "POST / HTTP/1.1",
                                  "Accept: */*",
                                  "Content-Length: 10",
                                  "Content-Type: application/json",
                                  "Host: google.com",
                                  "User-Agent: srep",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::client_header header("POST", "/",
                                   {{"Host:", "google.com"},
                                    {"User-Agent:", "srep"},
                                    {"Accept:", "*/*"},
                                    {"Content-Length:", "10"},
                                    {"Content-Type:", "application/json"}});

  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPClient, CanSerializeFilledHeaderWithCustomFields) {
  const std::string str_header[] {
                                "POST / HTTP/1.1",
                                  "Accept: */*",
                                  "Content-Length: 10",
                                  "Content-Type: application/json",
                                  "Custom: arbitrary value",
                                  "Host: google.com",
                                  "User-Agent: srep",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::client_header header("POST", "/",
                                   {{"Host:", "google.com"},
                                    {"User-Agent:", "srep"},
                                    {"Accept:", "*/*"},
                                    {"Content-Length:", "10"},
                                    {"Content-Type:", "application/json"},
                                    {"Custom:", "arbitrary value"}});

  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPClient, CanSerializeFilledHeaderWithContinue) {
  const std::string str_header[] {
                                "POST / HTTP/1.1",
                                  "Content-Length: 0",
                                  "Accept: */*",
                                  "Expect: 100-continue",
                                  "Host: google.com",
                                  "User-Agent: srep",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::client_header header("POST", "/",
                                   {{"Host:", "google.com"},
                                    {"User-Agent:", "srep"},
                                    {"Accept:", "*/*"},
                                    {"Expect:", "100-continue"}});

  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPServer, CanParseHeader) {
  srep::http::server_header header;
  const std::string str_header[] {
                                "HTTP/1.1 301 Moved Permanently",
                                  "Location: http://www.google.com/",
                                  "Content-Type: text/html; charset=UTF-8",
                                  "Date: Sun, 29 Mar 2020 14:35:58 GMT",
                                  "Expires: Tue, 28 Apr 2020 14:35:58 GMT",
                                  "Cache-Control: public, max-age=2592000",
                                  "Server: gws",
                                  "Content-Length: 219",
                                  "X-XSS-Protection: 0",
                                  "X-Frame-Options: SAMEORIGIN",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::istringstream iss(oss.str());

  iss >> header;

  EXPECT_EQ(301, header.status_code());
  EXPECT_EQ("Moved Permanently", header.status_message());
  EXPECT_EQ(std::size_t(219), header.content_length());
  EXPECT_EQ("text/html; charset=UTF-8", header.content_type());
}

TEST(HTTPServer, CanSerializeDefaultHeader) {
  const std::string str_header[] {
                                "HTTP/1.1 200 OK",
                                  "Content-Length: 0",
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();
  srep::http::server_header header;
  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPServer, CanSerializeFilledHeader) {
  const std::string str_header[] {
                                "HTTP/1.1 100 Continue",
                                  "Content-Length: 10",
                                  "Content-Type: application/json"
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::server_header header(100, "Continue",
                                   {{"Content-Length:", "10"},
                                    {"Content-Type:", "application/json"}});
  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}

TEST(HTTPServer, CanSerializeFilledHeaderWithCustomField) {
  const std::string str_header[] {
                                "HTTP/1.1 100 Continue",
                                  "Content-Length: 10",
                                  "Content-Type: application/json",
                                  "Custom: arbitrary value"
                                  };
  std::ostringstream oss;
  for (const std::string &line: str_header) {
    oss << line << "\r\n";
  }
  oss << "\r\n";
  std::string expected = oss.str();

  srep::http::server_header header(100, "Continue",
                                   {{"Content-Length:", "10"},
                                    {"Content-Type:", "application/json"},
                                    {"Custom:", "arbitrary value"}});
  std::ostringstream output;
  output << header;

  EXPECT_EQ(split_header_str(expected), split_header_str(output.str()));
}
