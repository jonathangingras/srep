#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "../src/distributor.hh"

template <typename shared_type>
struct test_peer {
  shared_type &kept;

  void share(const std::unique_ptr<shared_type> &shared) {
    std::ostringstream output;
    output << *shared;
    shared_type deserialized;
    std::istringstream(output.str()) >> deserialized;
    kept = std::move(deserialized);
  }
};

TEST(Distributor, CanSendToPeers) {
  int i1, i2, i3;
  std::vector<test_peer<int>> peers {{i1}, {i2}, {i3}};
  srep::peer_distributor<std::vector<test_peer<int>>> distributor(peers);

  distributor.share(std::make_unique<int>(42));

  EXPECT_EQ(42, i1);
  EXPECT_EQ(42, i2);
  EXPECT_EQ(42, i3);
}
