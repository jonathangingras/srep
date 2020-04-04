#include "receiver.hh"

namespace srep {

https_receiver::https_receiver(const std::string &certificate_chain_file,
                               const std::string &private_key_file,
                               const std::string &tmp_dh_file,
                               const std::string &passphrase,
                               const unsigned short &port):
  io_context_(),
  context_(boost::asio::ssl::context::sslv23),
  acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
  passphrase_(passphrase) {
  context_.set_options(
    boost::asio::ssl::context::default_workarounds
    | boost::asio::ssl::context::no_sslv2
    | boost::asio::ssl::context::single_dh_use);
  context_.set_password_callback([&] (auto, auto) -> std::string { return passphrase_; });
  context_.use_certificate_chain_file(certificate_chain_file);
  context_.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
  context_.use_tmp_dh_file(tmp_dh_file);
}

void https_receiver::suspend() {
  if (!io_context_.stopped()) {
    io_context_.stop();
  }
}

}
