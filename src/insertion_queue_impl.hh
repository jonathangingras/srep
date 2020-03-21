#ifndef _SREP_INSERTION_QUEUE_IMPL_HH_
#define _SREP_INSERTION_QUEUE_IMPL_HH_

#ifndef _SREP_INSERTION_QUEUE_HH_
#include "insertion_queue.hh"
#endif

#include <utility>

namespace srep {

template <typename value_type, typename comparison_trait, typename allocator_type>
insertion_queue<value_type, comparison_trait, allocator_type>::~insertion_queue() {
  while(size_ > 0) {
    pop();
  }
}

template <typename value_type, typename comparison_trait, typename allocator_type>
constexpr insertion_queue<value_type, comparison_trait, allocator_type> &
insertion_queue<value_type, comparison_trait, allocator_type>::operator = (insertion_queue<
                                                                           value_type,
                                                                           comparison_trait,
                                                                           allocator_type> &&rhs) {
  this->~insertion_queue();
  ::new (this) insertion_queue<value_type,
                               comparison_trait,
                               allocator_type>(std::forward<insertion_queue<
                                               value_type,
                                               comparison_trait,
                                               allocator_type> >(rhs));
  return *this;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
size_t insertion_queue<value_type, comparison_trait, allocator_type>::size() const {
  return size_;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
void insertion_queue<value_type, comparison_trait, allocator_type>::insert_front(bucket *new_bucket) {
  new_bucket->next = first;

  if (new_bucket->next != nullptr) {
    new_bucket->next->previous = new_bucket;
  }

  first = new_bucket;

  if (size_ == 0) { last = first; }

  ++size_;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
void insertion_queue<value_type, comparison_trait, allocator_type>::insert_back(bucket *new_bucket) {
  new_bucket->previous = last;

  if (new_bucket->previous != nullptr) {
    new_bucket->previous->next = new_bucket;
  }

  last = new_bucket;

  if (size_ == 0) { first = last; }

  ++size_;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
void insertion_queue<value_type, comparison_trait, allocator_type>::insert_in_between(bucket *previous, bucket *new_bucket, bucket *next) {
  new_bucket->previous = previous;
  new_bucket->next = next;

  if (new_bucket->previous != nullptr) { new_bucket->previous->next = new_bucket; }
  if (new_bucket->next != nullptr) { new_bucket->next->previous = new_bucket; }

  ++size_;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
void insertion_queue<value_type, comparison_trait, allocator_type>::insert(rvalue_reference rhs) {
  bucket *new_bucket = std::allocator_traits<bucket_allocator_type>::allocate(allocator, 1);
  std::allocator_traits<bucket_allocator_type>::construct(allocator, new_bucket, std::forward<value_type>(rhs));

  if (size_ == 0) {
    insert_back(new_bucket);
  } else {
    bucket *it;
    for (it = last;
         it != nullptr && comparison_trait()(new_bucket->element, it->element);
         it = it->previous);

    if (it == nullptr) {
      insert_front(new_bucket);
    } else if (it == last) {
      insert_back(new_bucket);
    } else {
      insert_in_between(it, new_bucket, it->next);
    }
  }
}

template <typename value_type, typename comparison_trait, typename allocator_trait>
void insertion_queue<value_type, comparison_trait, allocator_trait>::pop() {
  bucket *output = first;

  if (size_ == 0) {
    return;
  } else if (size_ == 1) {
    first = nullptr;
    last = nullptr;
  } else if (size_ == 2) {
    last->previous = nullptr;
    first = last;
  } else {
    first = first->next;
    first->previous = nullptr;
  }

  std::allocator_traits<bucket_allocator_type>::destroy(allocator, output);
  std::allocator_traits<bucket_allocator_type>::deallocate(allocator, output, 1);
  size_--;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
typename insertion_queue<value_type, comparison_trait, allocator_type>::const_reference
insertion_queue<value_type, comparison_trait, allocator_type>::top() const {
  return first->element;
}

template <typename value_type, typename comparison_trait, typename allocator_type>
typename insertion_queue<value_type, comparison_trait, allocator_type>::reference
insertion_queue<value_type, comparison_trait, allocator_type>::top() {
  return first->element;
}

}

#endif
