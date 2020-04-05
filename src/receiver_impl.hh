#ifndef _SREP_RECEIVER_IMPL_HH_
#define _SREP_RECEIVER_IMPL_HH_

#ifndef _SREP_RECEIVER_HH_
#include "receiver.hh"
#endif

#include <iostream>

#include "http.hh"

namespace srep {

// TODO: deal with all !error non-happy paths

template <typename function_type>
void https_receiver::forward_body_and_close_connection(
  function_type &&function,
  std::shared_ptr<boost::asio::streambuf> received_data,
  std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
  thread_pool &pool) {

  pool.push([&function, received_data] {
              std::istream input_stream(received_data.get());
              function(input_stream);
            });

  std::string response = http::server_header(202, "Accepted", {{"Connection:", "close"}});
  boost::asio::async_write(*client, boost::asio::buffer(response, response.size()), noop);
}

template <typename function_type>
std::function<void(const boost::system::error_code &, std::size_t)>
https_receiver::get_read_callback(
  function_type &&function,
  std::shared_ptr<boost::asio::streambuf> received_data,
  std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> client,
  thread_pool &pool) {
  return
    [&function, received_data, client, &pool](const boost::system::error_code &error, std::size_t read_size) {
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
                                   [&function, received_data, client, expected_length, &pool]
                                   (const boost::system::error_code &error, std::size_t) {
                                     if (!error) {
                                       boost::asio::async_read(
                                         *client,
                                         *received_data,
                                         boost::asio::transfer_at_least(expected_length),
                                         [&function, received_data, client, &pool]
                                         (const boost::system::error_code &error, std::size_t) {
                                           if (!error) {
                                             forward_body_and_close_connection(function, received_data, client, pool);
                                           }
                                         });
                                     }
                                   });
        } else if (read_size >= client_header.size() + client_header.content_length()) {
          forward_body_and_close_connection(function, received_data, client, pool);
        } else if (client_header.content_length() < 2048) {
          // TODO: adjust limit of 2048

          boost::asio::async_read(*client,
                                  *received_data,
                                  boost::asio::transfer_at_least(client_header.content_length()),
                                  [&function, received_data, client, &pool]
                                  (const boost::system::error_code &error, std::size_t) {
                                    if (!error) {
                                      forward_body_and_close_connection(function, received_data, client, pool);
                                    }
                                  });
        } else {
          std::string response = http::server_header(413, "Payload Too Large");
          boost::asio::async_write(*client, boost::asio::buffer(response, response.size()), noop);
        }
      }
    };
}

template <typename function_type>
std::function<void(const boost::system::error_code &, boost::asio::ip::tcp::socket)>
https_receiver::get_accept_callback(function_type &&function) {
  return
    [this, &function](const boost::system::error_code &error,
                      boost::asio::ip::tcp::socket client_socket) {
      if (!error) {
        auto client = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(
          std::move(client_socket), context_);

        thread_pool &pool = this->pool;
        client->async_handshake(boost::asio::ssl::stream_base::server,
                                [&function, client, &pool] (const boost::system::error_code &error) {
                                  if (!error) {
                                    auto received_data = std::make_shared<boost::asio::streambuf>();
                                    boost::asio::async_read(*client,
                                                            *received_data,
                                                            boost::asio::transfer_at_least(1),
                                                            get_read_callback(function, received_data, client, pool));
                                  }
                                });

      }
      acceptor_.async_accept(get_accept_callback(function));
    };
}

template <typename function_type>
void https_receiver::run(function_type &&function) {
  acceptor_.async_accept(get_accept_callback(function));
  io_context_.run();
}

}

#endif
