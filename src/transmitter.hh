#ifndef _SREP_TRANSMITTER_HH_
#define _SREP_TRANSMITTER_HH_

#include <string>

namespace srep {

class https_transmitter {
public:
  https_transmitter(const std::string &remote_host,
                    const unsigned short &port,
                    const std::string &certificate_chain_file = "") {
    
  }

  void suspend();

  template <typename function_type>
  void run(function_type &&function);
};

}

#endif
