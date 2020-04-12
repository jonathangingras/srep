#include <gtest/gtest.h>
#include <sstream>
#include "../src/jsonable_state_update.hh"

TEST(SerializableTest, CanSerializeToStream) {
  std::ostringstream serializing_stream, expected_stream;
  srep::jsonable_replace_by_state_update<int> update(42);

  expected_stream << nlohmann::json {{"operation", "replace_by"}, {"value", 42}};
  serializing_stream << update;

  EXPECT_EQ(expected_stream.str(), serializing_stream.str());
}

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
  intermediate << nlohmann::json({{"operation", "substraction"}, {"value", 10}});
  std::istringstream input(intermediate.str());

  for(size_t j = 0; j < 3; ++j){
    srep::serializable_state_update<int, nlohmann::json>::ptr update;
    input >> update;

    update->apply(i);
  }

  EXPECT_EQ(60, i);
}
