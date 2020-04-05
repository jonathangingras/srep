#include <fstream>
#include <random>

int main(int argc, char **argv) {
  std::random_device rd;
  std::ofstream o("data.bin");
  for(size_t i = 0; i < 20000000; ++i) {
    char c = rd();
    o.write(&c, 1);
  }
  return 0;
}
