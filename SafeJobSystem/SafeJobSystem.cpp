#include "SafeJobSystem.h"
#include <iostream> // Ensure this is included
#include <stdexcept>

namespace SafeJobSystem {

    template <QueueType Type>
    ThreadManager<Type>::ThreadManager(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back(&ThreadManager::workerThread, this);
        }
    }

    template <QueueType Type>
    ThreadManager<Type>::~ThreadManager() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template <QueueType Type>
    void ThreadManager<Type>::addTask(const std::function<void()>& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) {
                throw std::runtime_error("Adding tasks to stopped ThreadManager");
            }
            taskQueue.push(task);
        }
        condition.notify_one();
    }

    template <QueueType Type>
    void ThreadManager<Type>::workerThread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return stop || !taskQueue.empty(); });
                if (stop && taskQueue.empty()) {
                    return;
                }
                task = taskQueue.front();
                taskQueue.pop();
            }
            try {
                task();
            }
            catch (const std::exception& e) {
                // Handle exception (log, terminate, etc.)
                std::cerr << "Task threw an exception: " << e.what() << std::endl;
            }
        }
    }

    template class ThreadManager<QueueType::StdQueue>;
    template class ThreadManager<QueueType::WaitFreeQueue>;
    template class ThreadManager<QueueType::LockFreeQueue>;

} // namespace SafeJobSystem
