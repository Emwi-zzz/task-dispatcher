#pragma once

#include "ITaskDispatcher.hpp"
#include "WSDeque.hpp"
#include "random_generator.cpp"
#include <thread>

class Worker : public ITaskDispatcher{
    private:
        Worker() = default;

        int log_size = 5;

        std::atomic<int> working_deqs{0};

        uint64_t seed = 0;
        int threads_number = 1;

        std::vector<WSDeque> deques;
        std::vector<char> empty_statuses;
        std::vector<std::thread> threads;

        void thread_instructions(int thread_id);
    public:
        void push(Task&& task) override;
        void push(Task&& task, int thread_id) override;

        inline bool is_executing() override;

        void execute() override;

        void join() override;

        void set_log_size(int log_size) override;

        Worker(int threads_number);
        
    #ifdef TESTING
        friend void runWorkerFunctionTest(int threads, const std::vector<Task>& tasks);
    #endif

};