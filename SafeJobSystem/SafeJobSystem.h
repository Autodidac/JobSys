#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <deque>
#include <stdexcept>
#include <string>

// Thread-safe queue implementation
class ThreadSafeQueue {
public:
    void push(const std::string& text);
    std::string pop();

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

// Job system with a thread manager
namespace SafeJobSystem {

    class ThreadManager {
    public:
        // Maximum of 8 times the number of detected cores, in the case it fails to detect than it's set to 1 and 8 times that is the maximum threads
        // 50 1 second tests on a 3.0 6x core pc happens in perfect unison.. to give you an idea and still avoid thread lock.
        // Recommended to set to the number of cores in your system or less, there is a job queue as well that processes one at a time.
        ThreadManager(size_t minThreads = 1, size_t maxThreads = 8 * std::thread::hardware_concurrency());
        ~ThreadManager();

        void AddTask(std::function<void()> task);
        void setThreadCount(size_t minThreads, size_t maxThreads);

    private:
        void workerThread();

        std::vector<std::thread> workers_;
        std::deque<std::function<void()>> jobQueue_;
        std::mutex queueMutex_;
        std::condition_variable condition_;
        size_t minThreads_;
        size_t maxThreads_;
        std::atomic<bool> running_;
    };

} // namespace JobSystem

// Global declaration of threadManager
extern SafeJobSystem::ThreadManager threadManager;
