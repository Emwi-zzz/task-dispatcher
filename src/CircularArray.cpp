#include "CircularArray.hpp"

Task& CircularArray::operator[](std::size_t index) {
    return (*array)[index & mask];
}

const Task& CircularArray::operator[](std::size_t index) const {
    return (*array)[index & mask];
}

int CircularArray::size(){
    return 1 << log_size;
}

void CircularArray::grow(int b, int t){
    int old_mask = mask;
    log_size++;
    int new_mask = (1 << (log_size + 1)) - 1;
    auto temp = std::make_unique<std::vector<Task>>(1 << (log_size));
    for(int i = t; i < b; i++){
        (*temp)[i & new_mask] = std::move((*array)[i & old_mask]);
    }
    mask = new_mask;
    array = std::move(temp);
}

CircularArray::CircularArray(int log_size){
    this->log_size = log_size;
    mask = (1 << (log_size + 1)) - 1;
    array = std::make_unique<std::vector<Task>>(1 << (log_size));
}