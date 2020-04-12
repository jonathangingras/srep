#include "../src/map_state.hh"

namespace srep {

void addition_map_state_update::apply(nlohmann::json &state) {
  auto it = state.find(key);
  if (it != state.end()) { state.erase(it); };
  state.emplace(key, std::move(data));
}

std::ostream &addition_map_state_update::serialize_to(std::ostream &output) const {
  output << nlohmann::json({{"operation", "addition"}, {"key", key}, {"data", data}});
  return output;
}

void suppression_map_state_update::apply(nlohmann::json &state) {
  auto it = state.find(key);
  if (it != state.end()) { state.erase(it); };
}

std::ostream &suppression_map_state_update::serialize_to(std::ostream &output) const {
  output << nlohmann::json({{"operation", "suppression"}, {"key", key}});
  return output;
}

typename serializable_state_update<nlohmann::json, nlohmann::json>::ptr
jsonable_json_state_update_deserializer::deserialize_from(std::istream &input) {
  nlohmann::json intermediate;
  input >> intermediate;

  const std::string &operation = intermediate["operation"];

  if (operation == "addition") {
    return std::make_unique<addition_map_state_update>(
      std::string(intermediate["key"]), std::move(intermediate["data"]));
  } else if (operation == "suppression") {
    return std::make_unique<suppression_map_state_update>(
      std::string(intermediate["key"]));
  }

  return typename serializable_state_update<nlohmann::json, nlohmann::json>::ptr();
}

}
