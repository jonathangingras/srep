#ifndef _SREP_STATE_IMPL_HH_
#define _SREP_STATE_IMPL_HH_

#ifndef _SREP_STATE_HH_
#include "state.hh"
#endif

namespace srep {

template <typename state_type>
constexpr void state_update_apply_traits<state_type>::apply_update(state_type &state, update_type &update) {
  update.apply(state);
}

template <typename state_type>
template <typename state_update_type>
constexpr void state_update_apply_traits<state_type>::apply_update(state_type &state,
                                                                   std::unique_ptr<state_update_type> &update) {
  update->apply(state);
}

}

#endif
