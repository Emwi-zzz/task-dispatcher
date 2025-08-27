#pragma once

#include <functional>

using Task = std::function<void()>;

class ITaskDispatcher{
    private:
    public:
        virtual void push(Task&& task) = 0;
        virtual void push(Task&& task, int thread_id) = 0;

        virtual bool is_executing() = 0;

        virtual void execute() = 0;

        virtual void join() = 0;

        virtual void set_log_size(int log_size) = 0;

        virtual ~ITaskDispatcher() = default; 
};