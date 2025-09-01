#include "Worker.hpp"
#include <iostream>

void Worker::push(Task&& task) {
    push(std::move(task), random(seed) % threads_number);
}


void Worker::push(Task&& task, int thread_id) {
    deques[thread_id].push(std::move(task));
}



inline bool Worker::is_executing() {
    return working_deqs.load(std::memory_order_relaxed) != 0;
}

void Worker::thread_instructions(int thread_id){
    uint64_t thread_seed = thread_id;
    
    deques[thread_id].set_EMPTY([&is_empty = empty_statuses[thread_id], &working_deqs_ref = working_deqs](){
        is_empty = true;
        working_deqs_ref.fetch_sub(1, std::memory_order_acq_rel);
    });
    while(!empty_statuses[thread_id]){
        Task task = deques[thread_id].pop();
        task();
    }
    #ifdef STEALING
    while(is_executing()){
        Task task = deques[random(thread_seed) % threads_number].steal();
        task();
    }
    #endif
}

void Worker::execute() {
    empty_statuses.assign(threads_number, false);
    threads.clear();
    threads.reserve(threads_number);
    working_deqs = threads_number;
    for(int i = 0; i < threads_number; i++){
        threads.emplace_back([this, i](){
            thread_instructions(i);
        });
    }
}

void Worker::join() {
    for(int i = 0; i < threads.size(); i++){
        if (threads[i].joinable()){
            threads[i].join();
        }
    }
}

void Worker::set_log_size(int log_size) {
    this->log_size = log_size;
}

Worker::Worker(int threads_number) {
    this->threads_number = threads_number;
    deques.reserve(threads_number);
    empty_statuses.resize(threads_number, false);
    for(int i = 0; i < threads_number; i++){
        deques.emplace_back(log_size);
    }
}