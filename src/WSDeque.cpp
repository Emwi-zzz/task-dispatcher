#include "WSDeque.hpp"

bool WSDeque::casTop(int oldVal, int newVal) {
    return top->compare_exchange_strong(oldVal, newVal);
}

void WSDeque::push(Task&& task) {
    int t = top->load(std::memory_order_relaxed);
    int size = bottom - t;
    if(size >= array.size()){
        array.grow(bottom, t);
    }
    array[bottom] = std::move(task);
    bottom++;
}

Task WSDeque::pop(){

    #ifdef TESTING  // Test-only counter
        pop_count->fetch_add(1, std::memory_order_relaxed);
    #endif

    bottom--;
    int t = top->load(std::memory_order_relaxed);
    int size = bottom - t;
    if(size < 0){
        bottom = t;

        #ifdef TESTING  // Test-only counter
            EMPTY_count->fetch_add(1, std::memory_order_relaxed);
        #endif

        return EMPTY;
    }
    if(size > 0){
        return std::move(array[bottom]);
    }
    bottom = t + 1;
    if(!casTop(t, t+1)){

        #ifdef TESTING  // Test-only counter
            EMPTY_count->fetch_add(1, std::memory_order_relaxed);
        #endif

        return EMPTY;
    }
    return std::move(array[bottom-1]);
}

Task WSDeque::steal(){

    #ifdef TESTING  // Test-only counter
        steal_count->fetch_add(1, std::memory_order_relaxed);
    #endif

    int t = top->load(std::memory_order_relaxed);
    int size = bottom - t;
    if(size <= 0){

        #ifdef TESTING  // Test-only counter
            ABORT_count->fetch_add(1, std::memory_order_relaxed);
        #endif

        return ABORT;
    }
    if(!casTop(t, t+1)){

        #ifdef TESTING  // Test-only counter
            ABORT_count->fetch_add(1, std::memory_order_relaxed);
        #endif

        return ABORT;
    }
    return std::move(array[t]);
}

void WSDeque::set_EMPTY(Task&& task){
    EMPTY = std::move(task);
}

WSDeque::WSDeque(int log_size)
    :array(log_size)
{
    top = std::make_unique<std::atomic<int>>(0);
}

void WSDeque::reset(){
    #ifdef TESTING  // Reset test-only counters
        pop_count->store(0, std::memory_order_relaxed);
        steal_count->store(0, std::memory_order_relaxed);
        ABORT_count->store(0, std::memory_order_relaxed);
        EMPTY_count->store(0, std::memory_order_relaxed);
    #endif
}