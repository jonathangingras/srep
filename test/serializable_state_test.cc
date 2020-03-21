#include <gtest/gtest.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../src/serializable_state.hh"

template <typename state_type>
class jsonable_replace_by_state_update: public srep::serializable_state_update<state_type, nlohmann::json> {
  state_type value;

public:
  inline jsonable_replace_by_state_update(const state_type &state):
    value(state) {}
  inline jsonable_replace_by_state_update(state_type &&state):
    value(std::forward<state_type>(state)) {}

  void apply(state_type &state) {
    state = value;
  }

  std::ostream &serialize_to(std::ostream &output) {
    nlohmann::json result {{"operation", "replace_by"}, {"value", value}};
    output << result;
    return output;
  }
};

TEST(SerializableTest, CanSerializeToStream) {
  std::ostringstream serializing_stream, expected_stream;
  jsonable_replace_by_state_update<int> update(42);

  expected_stream << nlohmann::json {{"operation", "replace_by"}, {"value", 42}};
  serializing_stream << update;

  EXPECT_EQ(expected_stream.str(), serializing_stream.str());
}

template <typename state_type>
class jsonable_addition_state_update: public srep::serializable_state_update<state_type, nlohmann::json> {
  state_type value;

public:
  inline jsonable_addition_state_update(const state_type &state):
    value(state) {}
  inline jsonable_addition_state_update(state_type &&state):
    value(std::forward<state_type>(state)) {}

  void apply(state_type &state) {
    state += value;
  }

  std::ostream &serialize_to(std::ostream &output) {
    nlohmann::json result {{"operation", "addition"}, {"value", value}};
    output << result;
    return output;
  }
};

template <typename state_type>
class json_state_update_deserializer: public srep::state_update_deserializer<state_type, nlohmann::json> {
public:
  typename srep::serializable_state_update<state_type, nlohmann::json>::ptr
  deserialize_from(std::istream &input) {
    nlohmann::json intermediate;
    input >> intermediate;
    const std::string &operation = intermediate["operation"];
    if(operation == "replace_by") {
      return std::make_unique<jsonable_replace_by_state_update<state_type> >(state_type(intermediate["value"]));
    } else if(operation == "addition") {
      return std::make_unique<jsonable_addition_state_update<state_type> >(state_type(intermediate["value"]));
    }
    return typename srep::serializable_state_update<state_type, nlohmann::json>::ptr();
  }
};

template <typename state_type>
struct srep::state_update_deserialization_type_traits<state_type, nlohmann::json> {
  typedef json_state_update_deserializer<state_type> deserializer_type;
};

TEST(SerializableTest, CanDeserializeOnceFromStream) {
  int i = 0;
  srep::serializable_state_update<int, nlohmann::json>::ptr update;

  std::ostringstream intermediate;
  intermediate << nlohmann::json({{"operation", "replace_by"}, {"value", 42}});
  std::istringstream input(intermediate.str());

  input >> update;

  update->apply(i);

  EXPECT_EQ(42, i);
}

TEST(SerializableTest, CanDeserializeTwiceFromStream) {
  int i = 0;
  srep::serializable_state_update<int, nlohmann::json>::ptr update;

  std::ostringstream intermediate;
  intermediate << nlohmann::json({{"operation", "replace_by"}, {"value", 42}});
  intermediate << nlohmann::json({{"operation", "replace_by"}, {"value", 28}});
  std::istringstream input(intermediate.str());

  input >> update >> update;

  update->apply(i);

  EXPECT_EQ(28, i);
}

TEST(SerializableTest, DeserializationIsSequentiallyConsistentFromStream) {
  int i = 0;

  std::ostringstream intermediate;
  intermediate << nlohmann::json({{"operation", "replace_by"}, {"value", 42}});
  intermediate << nlohmann::json({{"operation", "addition"}, {"value", 28}});
  std::istringstream input(intermediate.str());

  for(size_t j = 0; j < 2; ++j){
    srep::serializable_state_update<int, nlohmann::json>::ptr update;
    input >> update;

    update->apply(i);
  }

  EXPECT_EQ(70, i);
}
