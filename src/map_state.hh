#ifndef _SREP_MAP_STATE_HH_
#define _SREP_MAP_STATE_HH_

#include <string>
#include <nlohmann/json.hpp>
#include "serializable_state.hh"

namespace srep {

class addition_map_state_update: public serializable_state_update<nlohmann::json, nlohmann::json> {
  std::string key;
  nlohmann::json data;

public:
  inline addition_map_state_update(const std::string &_key, const nlohmann::json &_data):
    key(_key), data(_data) {}

  void apply(nlohmann::json &state);
  std::ostream &serialize_to(std::ostream &output) const;
};

class suppression_map_state_update: public serializable_state_update<nlohmann::json, nlohmann::json> {
  std::string key;

public:
  inline suppression_map_state_update(const std::string &_key):
    key(_key) {}

  void apply(nlohmann::json &state);
  std::ostream &serialize_to(std::ostream &output) const;
};

class jsonable_json_state_update_deserializer: public state_update_deserializer<nlohmann::json, nlohmann::json> {
public:
  typename serializable_state_update<nlohmann::json, nlohmann::json>::ptr
  deserialize_from(std::istream &input);
};

template <>
struct state_update_deserializer_type_traits<nlohmann::json, nlohmann::json> {
  typedef jsonable_json_state_update_deserializer deserializer_type;
};

}

#endif
