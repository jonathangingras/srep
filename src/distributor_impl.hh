#ifndef _SREP_DISTRIBUTOR_IMPL_HH_
#define _SREP_DISTRIBUTOR_IMPL_HH_

#ifndef _SREP_DISTRIBUTOR_HH_
#include "distributor.hh"
#endif

namespace srep {

template <typename peer_container_type>
template <typename shared_type>
void peer_distributor<peer_container_type>::share(const shared_type &shared) {
  barrier barrier;
  for (auto &peer : peers_) {
    barrier.push([&peer, &shared]() { peer.share(shared); });
  }
}

}

#endif
