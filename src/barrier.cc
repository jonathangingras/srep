#include <utility>
#include "barrier.hh"

namespace srep {

barrier::~barrier() {
  for (auto &thread: threads) {
    if (thread.joinable()) { thread.join(); }
  }
}

void barrier::push(std::thread &&thread) {
  threads.push_front(std::forward<std::thread>(thread));
}

}
