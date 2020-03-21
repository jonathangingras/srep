#include <vector>
#include <gtest/gtest.h>
#include "../src/state.hh"

template <typename _state_type>
class replace_by_state_update: public srep::state_update<_state_type> {
public:
  typedef _state_type state_type;

private:
  state_type value_;

public:
  inline replace_by_state_update(const state_type &value):
    value_(value) {}

  inline replace_by_state_update(state_type &&value):
    value_(std::forward<state_type>(value)) {}

  inline void apply(state_type &state) {
    state = value_;
  }
};

TEST(State, CanReplacePODValue) {
  int i = 0;

  replace_by_state_update<int> update { 42 };

  srep::state_update_apply_traits<int>::apply_update(i, update);

  EXPECT_EQ(42, i);
}

template <typename _state_type>
class addition_state_update: public srep::state_update<_state_type> {
public:
  typedef _state_type state_type;

private:
  state_type value_;

public:
  inline addition_state_update(const state_type &value):
    value_(value) {}

  inline addition_state_update(state_type &&value):
    value_(std::forward<state_type>(value)) {}

  inline void apply(state_type &state) {
    state += value_;
  }
};

TEST(State, CanIncreasePODValue) {
  int i = 1;

  addition_state_update<int> update { 42 };

  srep::state_update_apply_traits<int>::apply_update(i, update);

  EXPECT_EQ(43, i);
}

TEST(State, CanVectorizeUpdates) {
  int i = 1;

  std::vector<srep::state_update<int>::ptr> updates;
  updates.push_back(std::make_unique<replace_by_state_update<int>>(28));
  updates.push_back(std::make_unique<addition_state_update<int>>(42));

  for (auto &update: updates) {
    srep::state_update_apply_traits<int>::apply_update(i, update);
  }

  EXPECT_EQ(70, i);
}
