#ifndef _SREP_RECEIVER_IMPL_HH_
#define _SREP_RECEIVER_IMPL_HH_

#ifndef _SREP_RECEIVER_HH_
#include "receiver.hh"
#endif

#include <iostream>

#include "http.hh"

namespace srep {

template <typename function_type>
void https_receiver::run(function_type &&function) {
  barrier barrier;
  barrier.push([this]{
    acceptor_.async_accept(get_accept_callback());
    io_context_.run();
  });
  while(!io_context_.stopped()) {
    std::shared_ptr<boost::asio::streambuf> buffer(nullptr);
    buffer_channel.read_preemptable(buffer);

    if (buffer.get() != nullptr) {
      std::istream input_stream(buffer.get());
      function(input_stream);
    }
  }
}

}

#endif
