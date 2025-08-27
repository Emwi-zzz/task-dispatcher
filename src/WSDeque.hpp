#pragma once

#include <atomic>

#include "CircularArray.hpp"

class WSDeque{
    private:
        WSDeque() = delete;
        Task EMPTY = [](){return;};;
        inline static const Task ABORT = [](){return;};

        int bottom = 0;
        std::unique_ptr<std::atomic<int>> top;

        CircularArray array;

        bool casTop(int oldVal, int newVal);

    public:
        void reset();
        #ifdef TESTING
            // Test-only counters
            std::unique_ptr<std::atomic<int>>pop_count = std::make_unique<std::atomic<int>>(0);
            std::unique_ptr<std::atomic<int>>steal_count = std::make_unique<std::atomic<int>>(0);
            std::unique_ptr<std::atomic<int>>ABORT_count = std::make_unique<std::atomic<int>>(0);
            std::unique_ptr<std::atomic<int>>EMPTY_count = std::make_unique<std::atomic<int>>(0);
        #endif

        void push(Task&& task);
        Task pop();
        Task steal();
        void set_EMPTY(Task&& task);

        WSDeque(int log_size);
        WSDeque(WSDeque&&) = default;
        WSDeque& operator=(WSDeque&&) = default;

};