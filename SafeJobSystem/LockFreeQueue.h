#pragma once

#include <atomic>
#include <memory>

template <typename T>
class LockFreeQueue {
public:
    LockFreeQueue();
    ~LockFreeQueue();

    void enqueue(const T& item);
    bool dequeue(T& item);
    bool isEmpty() const;

private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
};

// Implementations

template <typename T>
LockFreeQueue<T>::LockFreeQueue() {
    Node* dummy = new Node();
    head.store(dummy);
    tail.store(dummy);
}

template <typename T>
LockFreeQueue<T>::~LockFreeQueue() {
    while (Node* oldHead = head.load()) {
        head.store(oldHead->next.load());
        delete oldHead;
    }
}

template <typename T>
void LockFreeQueue<T>::enqueue(const T& item) {
    Node* newNode = new Node(item);
    Node* oldTail = tail.load();
    oldTail->next.store(newNode);
    tail.store(newNode);
}

template <typename T>
bool LockFreeQueue<T>::dequeue(T& item) {
    Node* oldHead = head.load(std::memory_order_acquire);
    Node* newHead = oldHead->next.load(std::memory_order_acquire);
    if (newHead == nullptr) {
        return false; // Queue is empty
    }
    item = newHead->data;
    head.store(newHead, std::memory_order_release);
    delete oldHead;
    return true;
}

template <typename T>
bool LockFreeQueue<T>::isEmpty() const {
    return head.load()->next.load() == nullptr;
}
