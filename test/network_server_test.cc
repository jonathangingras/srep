#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>
#include <gtest/gtest.h>

TEST(NetworkTransmitter, CanServeSSL) {
  std::filesystem::path ssl_dir = std::filesystem::path(__FILE__).parent_path() / "ssl";
  using boost::asio::ip::tcp;
  namespace ssl = boost::asio::ssl;

  boost::asio::io_context io_context;

  unsigned short port = 8080;
  tcp::acceptor acceptor_(io_context, tcp::endpoint(tcp::v4(), port));
  boost::asio::ssl::context context_(boost::asio::ssl::context::sslv23);

  context_.set_options(
    boost::asio::ssl::context::default_workarounds
    | boost::asio::ssl::context::no_sslv2
    | boost::asio::ssl::context::single_dh_use);
  context_.set_password_callback([&] (auto, auto) -> std::string {
                                   std::string result;
                                   std::ifstream f(ssl_dir / "passphrase.txt");
                                   f >> result;
                                   return result;
                                     });
  context_.use_certificate_chain_file(ssl_dir / "server.crt");
  context_.use_private_key_file(ssl_dir / "server.key", boost::asio::ssl::context::pem);
  context_.use_tmp_dh_file(ssl_dir / "dh1024.pem");

  boost::asio::ssl::stream<tcp::socket> socket_(acceptor_.accept(), context_);
  socket_.handshake(boost::asio::ssl::stream_base::server);

  char received_data[2048];
  memset(received_data, 0, 2048);
  socket_.read_some(boost::asio::buffer(received_data, 2048));

  std::string to_send_raw = "some data from the server";

  std::ostringstream oss;
  oss << "HTTP/1.1 200 OK\r\n";
  oss << "Content-Type: text/plain; charset=UTF-8\r\n";
  oss << "Content-Length: " << to_send_raw.size() << "\r\n";
  oss << "Connection: close\r\n\r\n";

  oss << to_send_raw;
  oss.flush();

  std::string to_send_back = oss.str();

  boost::asio::write(socket_, boost::asio::buffer(to_send_back.c_str(), to_send_back.size() + 1));

  std::cout << received_data << std::endl;
}
