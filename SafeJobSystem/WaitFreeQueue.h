#pragma once

#include <functional>
#include <atomic>
#include <memory>

template <typename T>
class WaitFreeQueue {
public:
    WaitFreeQueue();
    ~WaitFreeQueue();

    void enqueue(const T& item);
    bool dequeue(T& item);
    bool isEmpty() const;

private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node(const T& data) : data(data), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
    std::atomic<size_t> size;
};

template <typename T>
WaitFreeQueue<T>::WaitFreeQueue() : size(0) {
    Node* dummy = new Node(T());
    head.store(dummy);
    tail.store(dummy);
}

template <typename T>
WaitFreeQueue<T>::~WaitFreeQueue() {
    while (Node* oldHead = head.load()) {
        head.store(oldHead->next.load());
        delete oldHead;
    }
}

template <typename T>
void WaitFreeQueue<T>::enqueue(const T& item) {
    Node* newNode = new Node(item);
    Node* oldTail = tail.load();
    oldTail->next.store(newNode);
    tail.store(newNode);
    size.fetch_add(1, std::memory_order_relaxed);
}

template <typename T>
bool WaitFreeQueue<T>::dequeue(T& item) {
    Node* oldHead = head.load(std::memory_order_acquire);
    Node* newHead = oldHead->next.load(std::memory_order_acquire);

    if (newHead == nullptr) {
        return false; // Queue is empty
    }

    item = newHead->data;
    head.store(newHead, std::memory_order_release);
    delete oldHead;
    size.fetch_sub(1, std::memory_order_relaxed);
    return true;
}

template <typename T>
bool WaitFreeQueue<T>::isEmpty() const {
    return size.load(std::memory_order_acquire) == 0;
}
