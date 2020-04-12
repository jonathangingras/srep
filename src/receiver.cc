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
  buffer_channel.preempt();
}

// TODO: deal with all !error non-happy paths

void https_receiver::forward_body_and_close_connection(
  std::shared_ptr<boost::asio::streambuf> received_data,
  std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
  channel<std::shared_ptr<boost::asio::streambuf>> &buffer_channel) {

  buffer_channel.write(std::move(received_data));

  std::string response = http::server_header(202, "Accepted", {{"Connection:", "close"}});
  boost::asio::async_write(*client, boost::asio::buffer(response, response.size()), noop);
}

std::function<void(const boost::system::error_code &, std::size_t)>
https_receiver::get_read_callback(
  std::shared_ptr<boost::asio::streambuf> received_data,
  std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
  channel<std::shared_ptr<boost::asio::streambuf>> &buffer_channel) {
  return
    [received_data, client, &buffer_channel](const boost::system::error_code &error, std::size_t read_size) {
      if (!error) {
        http::client_header client_header;
        {
          std::istream input_stream(received_data.get());
          input_stream >> client_header;
        }

        if (client_header.expect_continue()) {
          // TODO: set a limit (byte size) as parameter, so the server can return 417 Negotiation Failed

          std::string response = http::server_header(100, "Continue");
          size_t expected_length = client_header.content_length();
          boost::asio::async_write(*client,
                                   boost::asio::buffer(response, response.size()),
                                   [received_data, client, expected_length, &buffer_channel]
                                   (const boost::system::error_code &error, std::size_t) {
                                     if (!error) {
                                       boost::asio::async_read(
                                         *client,
                                         *received_data,
                                         boost::asio::transfer_at_least(expected_length),
                                         [received_data, client, &buffer_channel]
                                         (const boost::system::error_code &error, std::size_t) {
                                           if (!error) {
                                             forward_body_and_close_connection(received_data, client, buffer_channel);
                                           }
                                         });
                                     }
                                   });
        } else if (read_size >= client_header.size() + client_header.content_length()) {
          forward_body_and_close_connection(received_data, client, buffer_channel);
        } else if (client_header.content_length() < 2048) {
          // TODO: adjust limit of 2048

          boost::asio::async_read(*client,
                                  *received_data,
                                  boost::asio::transfer_at_least(client_header.content_length()),
                                  [received_data, client, &buffer_channel]
                                  (const boost::system::error_code &error, std::size_t) {
                                    if (!error) {
                                      forward_body_and_close_connection(received_data, client, buffer_channel);
                                    }
                                  });
        } else {
          std::string response = http::server_header(413, "Payload Too Large");
          boost::asio::async_write(*client, boost::asio::buffer(response, response.size()), noop);
        }
      }
    };
}

std::function<void(const boost::system::error_code &, boost::asio::ip::tcp::socket)>
https_receiver::get_accept_callback() {
  return
    [this](const boost::system::error_code &error,
           boost::asio::ip::tcp::socket client_socket) {
      if (!error) {
        auto client = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(
          std::move(client_socket), context_);

        channel<std::shared_ptr<boost::asio::streambuf>> &buffer_channel = this->buffer_channel;
        client->async_handshake(boost::asio::ssl::stream_base::server,
                                [client, &buffer_channel] (const boost::system::error_code &error) {
                                  if (!error) {
                                    auto received_data = std::make_shared<boost::asio::streambuf>();
                                    boost::asio::async_read(*client,
                                                            *received_data,
                                                            boost::asio::transfer_at_least(1),
                                                            get_read_callback(received_data, client, buffer_channel));
                                  }
                                });

      }
      acceptor_.async_accept(get_accept_callback());
    };
}

}
