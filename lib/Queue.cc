#include "Queue.h"

#include <algorithm>
#include <cinttypes>

bool Queue::IsEmpty() const {
  return size_ == 0;
}

void Queue::PushBack(Point* point) {
  Node* node = new Node(point);
  if (front_ == nullptr) {
    front_ = node;
    back_ = node;
  } else {
    back_->prev = node;
    node->next = back_;
    back_ = node;
  }
  ++size_;
}

Point* Queue::PopFront() {
  if (front_ == nullptr) {
    return nullptr;
  }
  Point* result = front_->point;
  Node* tmp = front_;
  front_ = front_->prev;
  delete tmp;
  --size_;
  if (front_ == nullptr) {
    back_ = nullptr;
    return result;
  }
  front_->next = nullptr;
  return result;
}

const Node* Queue::Front() const {
  return front_;
}

const Node* Queue::Back() const {
  return back_;
}

size_t Queue::Size() const {
  return size_;
}

Queue::~Queue() {
  while (back_ != nullptr) {
    Node* tmp = back_;
    back_ = back_->prev;
    delete tmp;
  }
  front_ = nullptr;
}

Queue::Queue(const Queue& src) {
  Node* iterator = src.back_;
  while (iterator != nullptr) {
    if (front_ == nullptr) {
      front_ = new Node(*iterator);
      back_ = front_;
    } else {
      Node* node = new Node(*iterator);
      node->next = front_;
      front_->prev = node;
      front_ = node;
    }
    iterator = iterator->prev;
  }
}

Queue& Queue::operator=(const Queue& src) {
  if (this != &src) {
    Queue tmp(src);
    std::swap(front_, tmp.front_);
    std::swap(back_, tmp.back_);
    std::swap(size_, tmp.size_);
  }
  return *this;
}