#pragma once

#include <functional>

namespace SafeJobSystem {

    class IQueue {
    public:
        virtual ~IQueue() = default;
        virtual void enqueue(const std::function<void()>& task) = 0;
        virtual bool dequeue(std::function<void()>& task) = 0;
        virtual bool isEmpty() const = 0;
    };

}
