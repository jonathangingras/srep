#include <type_traits>
#include <algorithm>
#include <random>
#include <gtest/gtest.h>
#include "../src/insertion_queue.hh"
#include "test_object.hh"

TEST(InsertionQueue, InstantiationLegal) {
  srep::insertion_queue<test_object> iq;
}

TEST(InsertionQueue, CanAddElement) {
  srep::insertion_queue<test_object> iq;

  iq.insert(42);
}

TEST(InsertionQueue, CanAddElementTwice) {
  srep::insertion_queue<test_object> iq;

  iq.insert(42);
  iq.insert(43);
}

TEST(InsertionQueue, CanPopElement) {
  srep::insertion_queue<test_object> iq;

  iq.insert(42);

  test_object output = std::move(iq.top());

  EXPECT_EQ(42, output.inside);
}

TEST(InsertionQueue, SortingIsConsistent) {
  srep::insertion_queue<test_object> iq;
  std::uniform_int_distribution<int> d(0, 1000);
  std::random_device rd;
  std::vector<int> output;
  size_t limit = 10000;

  for (size_t i = 0; i < limit; ++i) {
    iq.insert(d(rd));
  }

  for (size_t i = 0; i < limit; ++i) {
    output.push_back(iq.top().inside);
    iq.pop();
  }

  EXPECT_TRUE(std::is_sorted(output.begin(), output.end()));
}

TEST(InsertionQueue, MoveConstructorCallIsConsistent) {
  srep::insertion_queue<test_object> iq;

  iq.insert(42);

  test_object output = std::move(iq.top());

  EXPECT_EQ(0, iq.top().inside);
}

TEST(InsertionQueue, SizeIsConsistentWhenInserting) {
  srep::insertion_queue<test_object> iq;

  iq.insert(42);
  EXPECT_EQ(1, iq.size());

  iq.insert(43);
  EXPECT_EQ(2, iq.size());

  iq.insert(44);
  EXPECT_EQ(3, iq.size());
}

TEST(InsertionQueue, SizeIsConsistentWhenPopping) {
  srep::insertion_queue<test_object> iq;

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);
  EXPECT_EQ(5, iq.size());

  iq.pop();
  EXPECT_EQ(4, iq.size());

  iq.pop();
  EXPECT_EQ(3, iq.size());

  iq.pop();
  EXPECT_EQ(2, iq.size());

  iq.pop();
  EXPECT_EQ(1, iq.size());

  iq.pop();
  EXPECT_EQ(0, iq.size());
}

template <typename value_type>
struct test_comparison_trait {
  constexpr bool operator() (const value_type& lhs, const value_type& rhs) const {
    return !(lhs < rhs);
  }
};

TEST(InsertionQueue, InsertionIsSortedUsingComparisonTrait) {
  srep::insertion_queue<test_object, test_comparison_trait<test_object> > iq;

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  int expected[] = {5, 4, 3, 3, 1};

  for (size_t i = 0; i < sizeof(expected)/sizeof(int); ++i) {
    EXPECT_EQ(expected[i], iq.top().inside);
    iq.pop();
  }
}

template <typename value_type_>
struct test_allocator {
  size_t *alloc_count, *dealloc_count, *construct_count, *destroy_count;

  typedef value_type_ value_type;

  inline test_allocator(size_t *a, size_t *d, size_t *c, size_t *k):
    alloc_count(a), dealloc_count(d), construct_count(c), destroy_count(k) {}

  value_type* allocate(std::size_t n) {
    if (alloc_count != nullptr) { ++(*alloc_count); }
    return std::allocator<value_type>().allocate(n);
  }

  void deallocate(value_type* p, std::size_t n) {
    if (dealloc_count != nullptr) { ++(*dealloc_count); }
    std::allocator<value_type>().deallocate(p, n);
  }

  template<typename _value_type, typename... Args>
  void construct(_value_type* p, Args &&...args) {
    if (construct_count != nullptr) { ++(*construct_count); }
    ::new((void *)p) _value_type(std::forward<Args>(args)...);
  }

  template<typename _value_type>
  void destroy(_value_type *p) {
    if (destroy_count != nullptr) { ++(*destroy_count); }
    p->~_value_type();
  }
};

TEST(InsertionQueue, AllocCalledOnEachInsert) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  srep::insertion_queue<test_object,
                        std::less<test_object>,
                        test_allocator<test_object> >
    iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  EXPECT_EQ(5, alloc_count);
}

TEST(InsertionQueue, DeallocCalledOnEachPop) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  srep::insertion_queue<test_object,
                        std::less<test_object>,
                        test_allocator<test_object> >
    iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  while (iq.size() > 0) {
    iq.pop();
  }

  EXPECT_EQ(5, dealloc_count);
}

TEST(InsertionQueue, ConstructCalledOnEachInsert) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  srep::insertion_queue<test_object,
                        std::less<test_object>,
                        test_allocator<test_object> >
    iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  EXPECT_EQ(5, construct_count);
}

TEST(InsertionQueue, DestroyCalledOnEachPop) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  srep::insertion_queue<test_object,
                        std::less<test_object>,
                        test_allocator<test_object> >
    iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  while (iq.size() > 0) {
    iq.pop();
  }

  EXPECT_EQ(5, destroy_count);
}

TEST(InsertionQueue, DestroyCalledOnEachAtDestruction) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  {
    srep::insertion_queue<test_object,
                          std::less<test_object>,
                          test_allocator<test_object> >
      iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

    iq.insert(1);
    iq.insert(3);
    iq.insert(5);
    iq.insert(3);
    iq.insert(4);
  }

  EXPECT_EQ(5, destroy_count);
}

TEST(InsertionQueue, DeallocCalledOnEachAtDestruction) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  {
    srep::insertion_queue<test_object,
                          std::less<test_object>,
                          test_allocator<test_object> >
      iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

    iq.insert(1);
    iq.insert(3);
    iq.insert(5);
    iq.insert(3);
    iq.insert(4);
  }

  EXPECT_EQ(5, dealloc_count);
}

TEST(InsertionQueue, MoveConstructionIsConsistent) {
  srep::insertion_queue<test_object> iq;

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  srep::insertion_queue<test_object> iq2 = std::move(iq);

  EXPECT_EQ(5, iq2.size());
  EXPECT_EQ(0, iq.size());
}

TEST(InsertionQueue, MoveAssignatorIsConsistent) {
  size_t alloc_count = 0, dealloc_count = 0, construct_count = 0, destroy_count = 0;
  srep::insertion_queue<test_object,
                        std::less<test_object>,
                        test_allocator<test_object> >
    iq(&alloc_count, &dealloc_count, &construct_count, &destroy_count),
    iq2(&alloc_count, &dealloc_count, &construct_count, &destroy_count);

  iq.insert(1);
  iq.insert(3);
  iq.insert(5);
  iq.insert(3);
  iq.insert(4);

  iq2.insert(1);
  iq2.insert(3);

  iq2 = std::move(iq);

  EXPECT_EQ(5, iq2.size());
  EXPECT_EQ(0, iq.size());
  EXPECT_EQ(2, destroy_count);
  EXPECT_EQ(2, dealloc_count);
  EXPECT_EQ(7, construct_count);
  EXPECT_EQ(7, alloc_count);
}

TEST(InsertionQueue, Conformance) {
  static_assert(!std::is_copy_constructible<srep::insertion_queue<test_object> >(), "must not be copy constructible");
  static_assert(std::is_move_constructible<srep::insertion_queue<test_object> >(), "must be move constructible");
  static_assert(std::is_move_assignable<srep::insertion_queue<test_object> >(), "must be move assignable");
}
