#ifndef _SREP_STATE_HH_
#define _SREP_STATE_HH_

#include <memory>

namespace srep {

template <typename _state_type>
class state_update {
public:
  typedef _state_type state_type;
  typedef std::unique_ptr<state_update<_state_type> > ptr;

  virtual ~state_update() {}
  virtual void apply(state_type &state) = 0;
};

template <typename state_type>
struct state_update_apply_traits {
  typedef state_update<state_type> update_type;

  static constexpr void apply_update(state_type &state, update_type &update);
  template <typename state_update_type>
  static constexpr void apply_update(state_type &state, std::unique_ptr<state_update_type> &update);
};

}

#include "state_impl.hh"

#endif
