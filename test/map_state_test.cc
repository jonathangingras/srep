#include <sstream>
#include <gtest/gtest.h>
#include "../src/map_state.hh"

TEST(MapState, CanAddEntry) {
  nlohmann::json state({});

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);

  srep::addition_map_state_update update {"test_key", {{"inner", 42}}};
  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, update);

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);
}

TEST(MapState, EntryAdditionOverridesExistingOne) {
  nlohmann::json state({});
  state.emplace("test_key", nlohmann::json {{"inner", 42}});

  srep::addition_map_state_update update {"test_key", {{"another", 43}}};
  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, update);

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"another", 43}}}}), state);
}

TEST(MapState, CanSuppressEntry) {
  nlohmann::json state({});
  state.emplace("test_key", nlohmann::json {{"inner", 42}});

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);

  srep::suppression_map_state_update update {"test_key"};
  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, update);

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);
}

TEST(MapState, EntrySuppressionHasNoEffectIfKeyDoesntExist) {
  nlohmann::json state({});
  state.emplace("test_key", nlohmann::json {{"inner", 42}});

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);

  srep::suppression_map_state_update update {"no_such_key"};
  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, update);

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);
}

TEST(MapState, EntrySuppressionHasNoEffectIfStateEmpty) {
  nlohmann::json state({});

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);

  srep::suppression_map_state_update update {"no_such_key"};
  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, update);

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);
}

TEST(MapState, CanApplyDeserializedAdditionUpdate) {
  nlohmann::json state({});

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);

  srep::addition_map_state_update update {"test_key", {{"inner", 42}}};
  std::ostringstream os;
  os << update;

  std::istringstream is(os.str());
  srep::serializable_state_update<nlohmann::json, nlohmann::json>::ptr deserialized;
  is >> deserialized;

  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, deserialized);

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);
}

TEST(MapState, CanApplyDeserializedSuppressionUpdate) {
  nlohmann::json state({});
  state.emplace("test_key", nlohmann::json {{"inner", 42}});

  EXPECT_EQ(std::size_t(1), state.size());
  EXPECT_EQ((nlohmann::json {{"test_key", {{"inner", 42}}}}), state);

  srep::suppression_map_state_update update {"test_key"};
  std::ostringstream os;
  os << update;

  std::istringstream is(os.str());
  srep::serializable_state_update<nlohmann::json, nlohmann::json>::ptr deserialized;
  is >> deserialized;

  srep::state_update_apply_traits<nlohmann::json>::apply_update(state, deserialized);

  EXPECT_EQ(std::size_t(0), state.size());
  EXPECT_EQ((nlohmann::json({})), state);
}
