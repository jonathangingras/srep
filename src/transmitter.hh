#ifndef _SREP_TRANSMITTER_HH_
#define _SREP_TRANSMITTER_HH_

#include <ostream>
#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>

namespace srep {

class https_transmitter {
  std::string remote_host_;
  unsigned short port_;
  std::string certificate_chain_file_;

public:
  https_transmitter(const std::string &remote_host,
                    const unsigned short &port,
                    const std::string &certificate_chain_file = "");

  void transmit(std::function<void(std::ostream &input)> function);
};

}

#endif
