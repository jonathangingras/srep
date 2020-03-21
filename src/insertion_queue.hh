#ifndef _SREP_INSERTION_QUEUE_HH_
#define _SREP_INSERTION_QUEUE_HH_

#include <type_traits>
#include <functional>
#include <memory>
#include "io_exchange.hh"

namespace srep {

template <typename value_type_,
          typename comparison_trait = std::less<value_type_>,
          typename allocator_type_ = std::allocator<value_type_> >
class insertion_queue {
public:
  typedef value_type_ value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef const value_type *const_pointer;
  typedef const value_type &const_reference;
  typedef value_type &&rvalue_reference;
  typedef allocator_type_ allocator_type;

  static_assert(std::is_move_constructible<value_type>(),
                "value_type of srep::insertion_queue must be move-constructible");

private:
  struct bucket {
    bucket *previous;
    bucket *next;
    value_type element;

    inline bucket(rvalue_reference rhs):
      previous(nullptr), next(nullptr), element(std::forward<value_type>(rhs)) {}
  };

  typedef typename std::allocator_traits<allocator_type>::template
  rebind_alloc<bucket> bucket_allocator_type;

  size_t size_;
  bucket *first;
  bucket *last;
  bucket_allocator_type allocator;

  void insert_front(bucket *new_bucket);
  void insert_back(bucket *new_bucket);
  void insert_in_between(bucket *previous, bucket *new_bucket, bucket *next);

public:
  inline insertion_queue():
    size_(0), first(nullptr), last(nullptr), allocator() {}

  inline insertion_queue(const insertion_queue &) = delete;

  inline insertion_queue(insertion_queue &&rhs):
    size_(rhs.size_), first(rhs.first), last(rhs.last), allocator(rhs.allocator) {
    rhs.size_ = 0;
  }

  template <typename ...Args>
  inline insertion_queue(Args &&...args):
    size_(0), first(nullptr), last(nullptr), allocator(args...) {}

  constexpr insertion_queue &operator = (insertion_queue &&rhs);

  ~insertion_queue();

  inline size_t size() const;
  inline const_reference top() const;
  inline reference top();

  void insert(rvalue_reference rhs);
  void pop();
};

template <typename value_type>
struct forward_output_traits<insertion_queue<value_type> > {
  static constexpr void read_in(insertion_queue<value_type> &output, value_type &&input) {
    output.insert(std::forward<value_type>(input));
  }
};

}

#include "insertion_queue_impl.hh"

#endif
