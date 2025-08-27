#pragma once

#include <memory>
#include <vector>
#include <functional>

using Task = std::function<void()>;

class CircularArray{
    private:
        CircularArray() = default;
        int log_size;
        int mask;

        std::unique_ptr<std::vector<Task>> array;
    public:
        CircularArray(int log_size);
        CircularArray(CircularArray&&) = default;
        CircularArray& operator=(CircularArray&&) = default;

        int size();

        Task& operator[](std::size_t index);
        const Task& operator[](std::size_t index) const;

        void grow(int b, int t);
};