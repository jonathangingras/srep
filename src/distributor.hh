#ifndef _SREP_DISTRIBUTOR_HH_
#define _SREP_DISTRIBUTOR_HH_

#include "barrier.hh"

namespace srep {

template <typename peer_container_type>
class peer_distributor {
  peer_container_type &peers_;

public:
  inline peer_distributor(peer_container_type &peers):
    peers_(peers)
    {}

  template <typename shared_type>
  void share(const shared_type &shared);
};

}

#include "distributor_impl.hh"

#endif
