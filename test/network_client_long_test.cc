#include <filesystem>
#include <gtest/gtest.h>
#include <fstream>
#include <random>
#include "../src/transmitter.hh"

TEST(HttpsTransmitter, CanSendLongDataInSSL) {
  std::filesystem::path ssl_dir = std::filesystem::path(__FILE__).parent_path() / "ssl";

  srep::https_transmitter transmitter("localhost", 8080, ssl_dir / "server.crt");

  {
    std::ofstream data("data.bin");
    std::random_device rd;
    for (size_t i = 0; i < 2000000; ++i) {
      data.put(rd());
    }
  }

  transmitter.transmit([](std::ostream &output) {
                         std::string input;
                         {
                           std::ifstream is("data.bin");
                           while (is.good()) {
                             char c = is.get();
                             if(is.good()) {
                               output.put(c);
                             }
                           }
                         }
                       });
}
