#include <filesystem>
#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>
#include <gtest/gtest.h>

TEST(NetworkTransmitter, CanConnectInSSL) {
  std::filesystem::path ssl_dir = std::filesystem::path(__FILE__).parent_path() / "ssl";
  using boost::asio::ip::tcp;
  namespace ssl = boost::asio::ssl;
  typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

// Create a context that uses the default paths for
// finding CA certificates.
  ssl::context ctx(ssl::context::sslv23);
  ctx.set_default_verify_paths();
  ctx.load_verify_file(ssl_dir / "server.crt");

// Open a socket and connect it to the remote host.
  boost::asio::io_context io_context;
  ssl_socket sock(io_context, ctx);
  tcp::resolver resolver(io_context);
  //tcp::resolver::query query("www.boost.org", "https");
  boost::asio::connect(sock.lowest_layer(), resolver.resolve("localhost", "8080"));//resolver.resolve(query));
  sock.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
  sock.set_verify_mode(ssl::verify_peer);
  sock.set_verify_callback(ssl::rfc2818_verification("localhost"));
  sock.handshake(ssl_socket::client);

  std::string to_send_raw = "some data from the client";

// ... read and write as normal ...
  std::ostringstream oss;
  oss << "HTTP/1.1 PUT /update\r\n";
  oss << "Host: localhost\r\n";
  oss << "Content-Type: text/plain; charset=UTF-8\r\n";
  oss << "Content-Length: " << to_send_raw.size() << "\r\n";
  oss << "Accept: */*\r\n";
  oss << "Connection: close\r\n\r\n";

  oss << to_send_raw;
  oss.flush();

  std::string sent_data = oss.str();

  boost::asio::write(sock, boost::asio::buffer(sent_data.c_str(), sent_data.size()));

  char received_data[2048];
  sock.read_some(boost::asio::buffer(received_data, 2048));

  std::cout << received_data << std::endl;
}
