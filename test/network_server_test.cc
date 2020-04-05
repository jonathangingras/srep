#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include "../src/receiver.hh"

TEST(HttpsReceiver, CanServeSSL) {
  std::filesystem::path ssl_dir = std::filesystem::path(__FILE__).parent_path() / "ssl";
  srep::https_receiver receiver(ssl_dir / "server.crt",
                                ssl_dir / "server.key",
                                ssl_dir / "dh1024.pem",
                                [&] () -> std::string {
                                  std::string result;
                                  std::ifstream f(ssl_dir / "passphrase.txt");
                                  f >> result;
                                  return result;
                                }(),
                                8080);

  std::ofstream of("output.bin");

  receiver.run([&receiver, &of] (std::istream &input) {
                 while (input.good()) {
                   char c;
                   c = input.get();
                   if (input.good()) {of.write(&c, 1);}
                 }
                 receiver.suspend();
               });
}
