#include <filesystem>
#include <gtest/gtest.h>
#include <fstream>
#include "../src/transmitter.hh"

TEST(HttpsTransmitter, CanConnectInSSL) {
  std::filesystem::path ssl_dir = std::filesystem::path(__FILE__).parent_path() / "ssl";

  srep::https_transmitter transmitter("localhost", 8080, ssl_dir / "server.crt");

  {
    std::ofstream data("data.bin");
    data << "some-data-from-the-client";
  }

  transmitter.transmit([](std::ostream &output) {
                         std::string input;
                         {
                           std::ifstream is("data.bin");
                           is >> input;
                         }
                         output << input;
                       });
}
