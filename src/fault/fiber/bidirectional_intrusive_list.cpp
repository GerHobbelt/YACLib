#include <yaclib/fault/detail/fiber/bidirectional_intrusive_list.hpp>

#include <utility>

namespace yaclib::detail::fiber {

void BiList::PushBack(BiNode* node) noexcept {
  if (node->prev != nullptr && node->prev != node) {
    this->Erase(node);
    _size++;
  }
  YACLIB_DEBUG(node->prev != nullptr && node->prev != node, "pushed and not popped");
  _size++;
  node->next = &_head;
  _head.prev->next = node;
  node->prev = _head.prev;
  _head.prev = node;
}

bool BiList::Empty() const noexcept {
  return &_head == _head.prev;
}

bool BiList::Erase(BiNode* node) noexcept {
  YACLIB_DEBUG(node == &_head, "trying to erase head");
  if (node->next == nullptr || node->prev == nullptr) {
    return false;
  }
  _size--;
  BiNode* prev = node->prev;
  BiNode* next = node->next;
  prev->next = next;
  next->prev = prev;
  node->next = nullptr;
  node->prev = nullptr;
  return true;
}

BiList::BiList(BiList&& other) noexcept {
  if (this == &other || other.Empty()) {
    return;
  }
  *this = std::move(other);
}

BiNode* BiList::PopBack() {
  _size--;
  auto* elem = _head.prev;
  Erase(_head.prev);
  return elem;
}

std::size_t BiList::GetSize() const noexcept {
  return _size;
}

BiNode* BiList::GetNth(std::size_t ind) const noexcept {
  YACLIB_DEBUG(ind >= _size, "ind for BiList::GetNth is out of bounds");
  std::size_t indd = 0;
  BiNode* node = _head.next;
  while (indd < ind) {
    node = node->next;
    indd++;
  }
  return node;
}

BiList& BiList::operator=(BiList&& other) noexcept {
  if (this == &other || other.Empty()) {
    return *this;
  }
  _head.next = std::exchange(other._head.next, &other._head);
  _head.prev = std::exchange(other._head.prev, &other._head);
  _head.next->prev = &_head;
  _head.prev->next = &_head;
  _size = std::exchange(other._size, 0);
  return *this;
}

void BiList::PushAll(BiList& other) noexcept {
  if (this == &other || other.Empty()) {
    return;
  }
  _head.prev->next = std::exchange(other._head.next, &other._head);
  _head.prev->next->prev = _head.prev;
  _head.prev = std::exchange(other._head.prev, &other._head);
  _head.prev->next = &_head;
  _size += other._size;
  other._size = 0;
}

void BiList::DecSize() noexcept {
  --_size;
}
}  // namespace yaclib::detail::fiber
