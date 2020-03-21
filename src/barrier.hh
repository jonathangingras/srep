#ifndef _SREP_BARRIER_HH_
#define _SREP_BARRIER_HH_

#include <thread>
#include <forward_list>

namespace srep {

class barrier {
  std::forward_list<std::thread> threads;

public:
  ~barrier();

  void push(std::thread &&thread);

  template<typename function_type, typename ...Args>
  inline void push(function_type &&function, Args &&...args);
};

}

#include "barrier_impl.hh"

#endif
