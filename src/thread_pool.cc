#include "thread_pool.hh"

namespace srep {

thread_pool::thread_pool():
  should_end(false),
  joiner([this] {
           size_t queue_size = 0;
           do {
             std::unique_lock<std::mutex> lock(mutex);
             condition_variable.wait(lock, [this] { return threads.size() > 0 || should_end; });
             queue_size = threads.size();
             if (queue_size > 0) {
               threads.front().join();
               threads.pop();
               --queue_size;
             }
           } while(queue_size > 0 || !should_end);
         })
{}

thread_pool::~thread_pool() {
  should_end = true;
  condition_variable.notify_one();
  joiner.join();
}

}
