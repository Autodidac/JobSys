#include "SafeJobSystem.h"

// ThreadSafeQueue method implementations
void ThreadSafeQueue::push(const std::string& text) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(text);
    condition_.notify_one();
}

std::string ThreadSafeQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return !queue_.empty(); });
    std::string text = queue_.front();
    queue_.pop();
    return text;
}

// JobSystem::ThreadManager method implementations
namespace SafeJobSystem {

    ThreadManager::ThreadManager(size_t minThreads, size_t maxThreads)
        : minThreads_(minThreads), maxThreads_(maxThreads), running_(true)
    {
        if (minThreads_ > maxThreads_) {
            throw std::invalid_argument("Minimum threads cannot be greater than maximum threads.");
        }

        size_t numThreads = std::max<size_t>(minThreads_, std::thread::hardware_concurrency());
        numThreads = std::min<size_t>(numThreads, maxThreads_);

        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back(&ThreadManager::workerThread, this);
        }
    }

    ThreadManager::~ThreadManager() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            running_ = false;
            condition_.notify_all();
        }

        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void ThreadManager::AddTask(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (!running_) {
                throw std::runtime_error("Cannot add tasks to a stopped system.");
            }
            jobQueue_.emplace_back(std::move(task));
        }
        condition_.notify_one();
    }

    void ThreadManager::setThreadCount(size_t minThreads, size_t maxThreads) {
        if (minThreads > maxThreads) {
            throw std::invalid_argument("Minimum threads cannot be greater than maximum threads.");
        }

        std::unique_lock<std::mutex> lock(queueMutex_);
        minThreads_ = minThreads;
        maxThreads_ = maxThreads;

        size_t numThreads = std::max<size_t>(minThreads_, std::thread::hardware_concurrency());
        numThreads = std::min<size_t>(numThreads, maxThreads_);

        if (workers_.size() < numThreads) {
            // Start more worker threads
            for (size_t i = workers_.size(); i < numThreads; ++i) {
                workers_.emplace_back(&ThreadManager::workerThread, this);
            }
        }
        else if (workers_.size() > numThreads) {
            // Safely stop excess threads
            running_ = false;
            condition_.notify_all();

            while (workers_.size() > numThreads) {
                workers_.back().join();
                workers_.pop_back();
            }

            running_ = true;
            condition_.notify_all();
        }
    }


    void ThreadManager::workerThread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                condition_.wait(lock, [this] { return !running_ || !jobQueue_.empty(); });

                // If the system is no longer running and the queue is empty, exit the thread
                if (!running_ && jobQueue_.empty()) {
                    return;
                }

                if (!jobQueue_.empty()) {
                    task = std::move(jobQueue_.front());
                    jobQueue_.pop_front();
                }
            }

            // If a task was fetched, execute it outside the lock
            if (task) {
                task();
            }
        }
    }


} // namespace JobSystem

// Global instance of threadManager
SafeJobSystem::ThreadManager threadManager;
