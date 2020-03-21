#ifndef _SREP_BARRIER_IMPL_HH_
#define _SREP_BARRIER_IMPL_HH_

#include <utility>

#ifndef _SREP_BARRIER_HH_
#include "barrier.hh"
#endif

namespace srep {

template<typename function_type, typename ...Args>
inline void barrier::push(function_type &&function, Args &&...args) {
  push(std::thread(function, std::forward<Args>(args)...));
}

}

#endif
