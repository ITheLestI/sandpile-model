#pragma once

#include <cinttypes>

struct Point;

struct Node {
  Point* point;
  Node* next = nullptr;
  Node* prev = nullptr;
  Node(Point* point) : point(point) {}
};


class Queue {
  Node* back_ = nullptr;
  Node* front_ = nullptr;
  size_t size_ = 0;
public:  
  Queue() = default;
  void PushBack(Point* point);
  [[nodiscard]] Point* PopFront();
  bool IsEmpty() const;
  const Node* Front() const;
  const Node* Back() const;
  size_t Size() const;
  ~Queue();
  Queue(const Queue& src);
  Queue& operator=(const Queue& src);
};
