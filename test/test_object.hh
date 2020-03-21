#ifndef _TEST_OBJECT_HH_
#define _TEST_OBJECT_HH_

#include <memory>

struct test_object {
  int inside;
  size_t *count;
  int *allocated;

  inline test_object(const int &i): inside(i), count(nullptr), allocated(new int {42}) {}
  inline test_object(const int &i, size_t *c): inside(i), count(c), allocated(new int {42}) {}

  test_object(const test_object &) = delete;

  inline test_object(test_object &&t): inside(t.inside), count(t.count), allocated(t.allocated) {
    t.inside = 0;
    t.count = nullptr;
    t.allocated = nullptr;
  }

  inline ~test_object() {
    if(count != nullptr) { ++(*count); }
    if(allocated != nullptr) { delete allocated; }
  }

  inline bool operator < (const test_object &other) const {
    return inside < other.inside;
  }
};

#endif
