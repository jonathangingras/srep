#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>

#include "../src/jsonable_state_update.hh"
#include "../src/barrier.hh"
#include "../src/receiver.hh"
#include "../src/serializable_state.hh"
#include "../src/transmitter.hh"
#include "../src/distributor.hh"

namespace srep {

template <typename state_type,
          typename serialization_format_type,
          typename char_type = char,
          typename char_traits_type = std::char_traits<char_type>
          >
struct remote_https_peer {
  https_transmitter transmitter;

  typedef srep::serializable_state_update<state_type,
                                          serialization_format_type,
                                          char_type,
                                          char_traits_type>
      update_type;

  void share(const update_type &update) {
    transmitter.transmit([&update](std::ostream &output) { output << update; });
  }
};

template <typename state_type,
          typename serialization_format_type,
          typename char_type = char,
          typename char_traits_type = std::char_traits<char_type>,
          typename deserializer_type_traits =
            typename srep::state_update_deserializer_type_traits<
              state_type, serialization_format_type, char_type, char_traits_type>
          >
struct local_https_peer {
  state_type &state;
  https_receiver receiver;
  std::mutex mutex;

  inline state_type get_state_copy() {
    std::lock_guard<std::mutex> lock(mutex);
    return state;
  }

  void run() {
    receiver.run([this](std::istream &input) {
      typename serializable_state_update<state_type,
                                         serialization_format_type,
                                         char_type,
                                         char_traits_type>::ptr update;
      srep::deserialize_from<state_type,
                             serialization_format_type,
                             char_type,
                             char_traits_type,
                             deserializer_type_traits>(input, update);
      std::lock_guard<std::mutex> lock(mutex);
      state_update_apply_traits<state_type>::apply_update(state, update);
    });
  }
};

}


TEST(HttpsPeers, CanCommunicate) {
  int data1 = 0, data2 = 0;

  {
    std::filesystem::path ssl_dir =
        std::filesystem::path(__FILE__).parent_path() / "ssl";

    srep::local_https_peer<int, nlohmann::json> local1 {
        {data1},
        {ssl_dir / "server.crt", ssl_dir / "server.key", ssl_dir / "dh1024.pem",
         [&]() -> std::string {
           std::string result;
           std::ifstream f(ssl_dir / "passphrase.txt");
           f >> result;
           return result;
         }(),
         8080},
        {}};
    srep::local_https_peer<int, nlohmann::json> local2 {
      {data2},
      {ssl_dir / "server.crt", ssl_dir / "server.key", ssl_dir / "dh1024.pem",
       [&]() -> std::string {
         std::string result;
         std::ifstream f(ssl_dir / "passphrase.txt");
         f >> result;
         return result;
       }(),
       8081},
      {}};

    srep::barrier barrier;
    barrier.push([&local1] { local1.run(); });
    barrier.push([&local2] { local2.run(); });

    std::vector<srep::remote_https_peer<int, nlohmann::json>> peers {
      {{"localhost", 8080, ssl_dir / "server.crt"}},
      {{"localhost", 8081, ssl_dir / "server.crt"}},
    };
    srep::peer_distributor<std::vector<srep::remote_https_peer<int, nlohmann::json>>> distributor(peers);

    distributor.share(srep::jsonable_addition_state_update(42));
    distributor.share(srep::jsonable_addition_state_update(8));
    distributor.share(srep::jsonable_substraction_state_update(10));

    local1.receiver.suspend();
    local2.receiver.suspend();
  }

  EXPECT_EQ(40, data1);
  EXPECT_EQ(40, data2);
}
