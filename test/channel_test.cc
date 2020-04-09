#include <gtest/gtest.h>
#include <random>
#include <thread>
#include <vector>
#include <set>
#include "../src/channel.hh"
#include "../src/io_exchange.hh"
#include "test_object.hh"

TEST(Channel, SendIsConsistent) {
  srep::channel<test_object> channel;

  channel.write(0);
  channel.write(2);
  channel.write(1);
  channel.write(3);
  channel.write(1);

  std::multiset<int> expected {0, 1, 1, 2, 3};
  std::vector<int> popped;

  EXPECT_EQ(std::size_t(5), channel.size());
  for (size_t i = 0; i < 5; ++i) {
    popped.push_back(channel.read().inside);
  }

  for (const auto &i: popped) {
    EXPECT_TRUE(expected.find(i) != expected.end());
  }
}

TEST(Channel, SingleThreadReadUnlockedWriteIsConsistent) {
  srep::channel<test_object> channel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    for (size_t j = 0; j < n_writes; ++j) {
                                      channel.write(d(rd));
                                    }
                                  }));
  }

  std::vector<int> output;

  for (size_t i = 0; i < n_threads * n_writes; ++i) {
    output.push_back(channel.read().inside);
  }

  for (auto &thread: threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, output.size());
}

TEST(Channel, LockedReadUnlockedWriteIsConsistent) {
  srep::channel<test_object> channel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads, read_threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    for (size_t j = 0; j < n_writes; ++j) {
                                      channel.write(d(rd));
                                    }
                                  }));
  }

  std::mutex vmtx;
  std::vector<int> output;

  for (size_t i = 0; i < n_threads; ++i) {
    read_threads.push_back(std::thread([&] {
                                         for (size_t j = 0; j < n_writes; ++j) {
                                           int r = channel.read().inside;
                                           {
                                             std::lock_guard<std::mutex> lock(vmtx);
                                             output.push_back(r);
                                           }
                                         }
                                       }));
  }

  for (auto &thread: threads) {
    thread.join();
  }
  for (auto &thread: read_threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, output.size());
}

template <typename value_type>
struct srep::forward_output_traits<std::vector<value_type> > {
  static constexpr void read_in(std::vector<value_type> &output, value_type &&input) {
    output.push_back(std::forward<value_type>(input));
  }
};

template <typename value_type>
struct srep::forward_input_traits<std::vector<value_type> > {
  static constexpr value_type &get_moveable(std::vector<value_type> &input) {
    return input.back();
  }

  static constexpr void erase_moveable(std::vector<value_type> &input) {
    input.pop_back();
  }
};

TEST(Channel, UnlockedBulkReadUnlockedWriteIsConsistent) { // Most Performant model
  srep::channel<test_object> channel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads, read_threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    for (size_t j = 0; j < n_writes; ++j) {
                                     channel.write(d(rd));
                                    }
                                  }));
  }

  std::vector<test_object> output;

  for (size_t i = 0; i < n_threads; ++i) {
    read_threads.push_back(std::thread([&] {
                                         srep::forward_output<std::vector<test_object> > reader { output };
                                         channel.read_in(reader, n_writes);
                                       }));
  }

  for (auto &thread: threads) {
    thread.join();
  }
  for (auto &thread: read_threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, output.size());
}

TEST(Channel, UnlockedBulkReadUnlockedBulkWriteIsConsistent) {
  srep::channel<test_object> channel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads, read_threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    std::vector<test_object> input;
                                    input.reserve(n_writes);
                                    for(size_t j = 0; j < n_writes; ++j) {
                                      input.push_back(d(rd));
                                    }
                                    srep::forward_input<std::vector<test_object> > writer { input };
                                    channel.write_from(writer, n_writes);
                                  }));
  }

  std::vector<test_object> output;

  for (size_t i = 0; i < n_threads; ++i) {
    read_threads.push_back(std::thread([&] {
                                         srep::forward_output<std::vector<test_object> > reader { output };
                                         channel.read_in(reader, n_writes);
                                       }));
  }

  for (auto &thread: threads) {
    thread.join();
  }
  for (auto &thread: read_threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, output.size());
}

TEST(Channel, UnlockedTwoChannelsIsConsistent) {
  srep::channel<test_object> wchannel, rchannel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads, read_threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    for (size_t j = 0; j < n_writes; ++j) {
                                     wchannel.write(d(rd));
                                    }
                                  }));
  }

  for (size_t i = 0; i < n_threads; ++i) {
    read_threads.push_back(std::thread([&] {
                                         for (size_t j = 0; j < n_writes; ++j) {
                                           rchannel.write(wchannel.read());
                                         }
                                       }));
  }

  for (auto &thread: threads) {
    thread.join();
  }
  for (auto &thread: read_threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, rchannel.size());
}

template <typename value_type>
struct srep::forward_output_traits<srep::channel<value_type> > {
  static constexpr void read_in(srep::channel<value_type> &output, value_type &&input) {
    output.write(std::forward<value_type>(input));
  }
};

TEST(Channel, UnlockedTwoChannelsReaderInBulkIsConsistent) {
  srep::channel<test_object> wchannel, rchannel;

  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<std::thread> threads, read_threads;

  size_t n_threads = 100, n_writes = 100;

  for (size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::thread([&] {
                                    for (size_t j = 0; j < n_writes; ++j) {
                                     wchannel.write(d(rd));
                                    }
                                  }));
  }

  for (size_t i = 0; i < n_threads; ++i) {
    read_threads.push_back(std::thread([&] {
                                         srep::forward_output<srep::channel<test_object> > reader { rchannel };
                                         wchannel.read_in(reader, n_writes);
                                       }));
  }

  for (auto &thread: threads) {
    thread.join();
  }
  for (auto &thread: read_threads) {
    thread.join();
  }

  EXPECT_EQ(n_threads * n_writes, rchannel.size());
}
