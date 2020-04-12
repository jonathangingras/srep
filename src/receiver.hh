#ifndef _SREP_RECEIVER_HH_
#define _SREP_RECEIVER_HH_

#include <istream>
#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "thread_pool.hh"
#include "channel.hh"
#include "barrier.hh"

namespace srep {

class https_receiver {
  boost::asio::io_context io_context_;
  boost::asio::ssl::context context_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::string passphrase_;
  channel<std::shared_ptr<boost::asio::streambuf> > buffer_channel;

  static constexpr void noop(const boost::system::error_code&, std::size_t) {}

  static void forward_body_and_close_connection(
    std::shared_ptr<boost::asio::streambuf> received_data,
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
    channel<std::shared_ptr<boost::asio::streambuf>> &buffer_channel);

  static std::function<void(const boost::system::error_code &, std::size_t)>
  get_read_callback(
    std::shared_ptr<boost::asio::streambuf> received_data,
    std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
    channel<std::shared_ptr<boost::asio::streambuf>> &buffer_channel);

  std::function<void(const boost::system::error_code &, boost::asio::ip::tcp::socket)>
  get_accept_callback();

public:
  https_receiver(const std::string &certificate_chain_file,
               const std::string &private_key_file,
               const std::string &tmp_dh_file,
               const std::string &passphrase,
               const unsigned short &port);

  void suspend();

  template <typename function_type>
  void run(function_type &&function);
};

}

#include "receiver_impl.hh"

#endif
