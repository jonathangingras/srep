#ifndef _SREP_JSONABLE_STATE_UPDATE_HH_
#define _SREP_JSONABLE_STATE_UPDATE_HH_

#include <nlohmann/json.hpp>
#include "serializable_state.hh"

namespace srep {

template <typename state_type>
using jsonable_state_update = serializable_state_update<state_type, nlohmann::json>;

template <typename state_type>
class jsonable_replace_by_state_update: public jsonable_state_update<state_type> {
  state_type value;

public:
  inline jsonable_replace_by_state_update(const state_type &state):
    value(state) {}
  inline jsonable_replace_by_state_update(state_type &&state):
    value(std::forward<state_type>(state)) {}

  void apply(state_type &state) {
    state = value;
  }

  std::ostream &serialize_to(std::ostream &output) const {
    nlohmann::json result {{"operation", "replace_by"}, {"value", value}};
    output << result;
    return output;
  }
};

template <typename state_type>
class jsonable_addition_state_update: public jsonable_state_update<state_type> {
  state_type value;

public:
  inline jsonable_addition_state_update(const state_type &state):
    value(state) {}
  inline jsonable_addition_state_update(state_type &&state):
    value(std::forward<state_type>(state)) {}

  void apply(state_type &state) {
    state += value;
  }

  std::ostream &serialize_to(std::ostream &output) const {
    nlohmann::json result {{"operation", "addition"}, {"value", value}};
    output << result;
    return output;
  }
};

template <typename state_type>
class jsonable_substraction_state_update: public jsonable_state_update<state_type> {
  state_type value;

public:
  inline jsonable_substraction_state_update(const state_type &state):
    value(state) {}
  inline jsonable_substraction_state_update(state_type &&state):
    value(std::forward<state_type>(state)) {}

  void apply(state_type &state) {
    state -= value;
  }

  std::ostream &serialize_to(std::ostream &output) const {
    nlohmann::json result {{"operation", "substraction"}, {"value", value}};
    output << result;
    return output;
  }
};

template <typename state_type>
class jsonable_state_update_deserializer: public state_update_deserializer<state_type, nlohmann::json> {
public:
  typename serializable_state_update<state_type, nlohmann::json>::ptr
  deserialize_from(std::istream &input) {
    nlohmann::json intermediate;
    input >> intermediate;
    const std::string &operation = intermediate["operation"];
    if(operation == "replace_by") {
      return std::make_unique<jsonable_replace_by_state_update<state_type> >(state_type(intermediate["value"]));
    } else if(operation == "addition") {
      return std::make_unique<jsonable_addition_state_update<state_type> >(state_type(intermediate["value"]));
    } else if(operation == "substraction") {
      return std::make_unique<jsonable_substraction_state_update<state_type> >(state_type(intermediate["value"]));
    }
    return typename serializable_state_update<state_type, nlohmann::json>::ptr();
  }
};

template <typename state_type>
struct state_update_deserializer_type_traits<state_type, nlohmann::json> {
  typedef jsonable_state_update_deserializer<state_type> deserializer_type;
};

}

#endif
