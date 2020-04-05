#include <string>
#include <istream>
#include <array>
#include "transmitter.hh"
#include "http.hh"

#include <iostream>

namespace srep {

https_transmitter::https_transmitter(const std::string &remote_host,
                                     const unsigned short &port,
                                     const std::string &certificate_chain_file):
  remote_host_(remote_host),
  port_(port),
  ssl_context_(boost::asio::ssl::context::sslv23) {
  ssl_context_.set_default_verify_paths();
  if (!certificate_chain_file.empty()) {
    ssl_context_.load_verify_file(certificate_chain_file);
  }
}

// TODO: deal with all !error non-happy paths

void https_transmitter::transmit(std::function<void(std::ostream &input)> function) {
  boost::asio::io_context io_context;
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(io_context, ssl_context_);
  boost::asio::ip::tcp::resolver resolver(io_context);

  boost::asio::connect(socket.lowest_layer(), resolver.resolve(remote_host_, std::to_string(port_)));
  socket.lowest_layer().set_option(boost::asio::ip::tcp::no_delay(true));

  socket.set_verify_mode(boost::asio::ssl::verify_peer);
  socket.set_verify_callback(boost::asio::ssl::rfc2818_verification(remote_host_));

  boost::asio::streambuf content_buf, header_buf, from_server;
  socket.async_handshake(
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::client,
    [&function, &socket, &header_buf, &content_buf, &from_server] (const boost::system::error_code &error) {
      if (!error) {
        bool needs_continue = false;
        {
          std::ostream os(&content_buf);
          function(os);

          http::client_header header("POST", "/",
                                     {{"Content-Length:", std::to_string(content_buf.size())}});

          if (header.content_length() > 2048) {
            needs_continue = true;
            header.emplace("Expect:", "100-continue");
          }

          std::ostream hos(&header_buf);
          hos << header;
        }

        if (!needs_continue) {
          std::array<boost::asio::streambuf::const_buffers_type, 2> buffers {header_buf.data(), content_buf.data()};

          boost::asio::async_write(
            socket,
            buffers,
            [&socket, &from_server] (const boost::system::error_code &error, std::size_t) {
              if (!error) {
                boost::asio::async_read(
                  socket,
                  from_server,
                  boost::asio::transfer_at_least(1),
                  [&from_server] (const boost::system::error_code &error, std::size_t) {
                    if (!error) {
                      http::server_header server_header;
                      std::istream is(&from_server);
                      is >> server_header;
                      if (server_header.status_code() != 202) {
                        throw std::runtime_error("could not send data to remote host: status code " +
                                                 std::to_string(server_header.status_code()));
                      }
                    }
                  });
              }
            });
        } else {
          boost::asio::async_write(
            socket,
            header_buf,
            [&socket, &content_buf, &from_server] (const boost::system::error_code &error, std::size_t) {
              if (!error) {

                boost::asio::async_read(
                  socket,
                  from_server,
                  boost::asio::transfer_at_least(1),
                  [&socket, &content_buf, &from_server] (const boost::system::error_code &error, std::size_t) {
                    if (!error) {

                      http::server_header server_header;
                      std::istream is(&from_server);
                      is >> server_header;

                      if (server_header.status_code() == 100) {

                        boost::asio::async_write(
                          socket,
                          content_buf,
                          [&socket, &from_server] (const boost::system::error_code &error, std::size_t) {
                            if (!error) {
                              boost::asio::async_read(
                                socket,
                                from_server,
                                boost::asio::transfer_at_least(1),
                                [&from_server] (const boost::system::error_code &error, std::size_t) {
                                  if (!error) {
                                    http::server_header server_header;
                                    std::istream is(&from_server);
                                    is >> server_header;
                                    if (server_header.status_code() != 202) {
                                      throw std::runtime_error("could not send data to remote host: status code " +
                                                               std::to_string(server_header.status_code()));
                                    }
                                  }
                                });
                            }
                          });

                      } else {
                        throw std::runtime_error("server did not accept the continue request: status code " +
                                                 std::to_string(server_header.status_code()));
                      }

                    }
                  });

              }
            });
        }
      }
    });

  io_context.run();
}

}
