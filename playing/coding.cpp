/*
 * CODING INTERVIEW QUESTION:
 * 
 * Implement a thread-safe queue class that can be safely used by multiple threads.
 * The queue should support the following operations:
 * 1. push: Add an element to the queue
 * 2. try_pop: Try to pop an element from the queue, return false if queue is empty
 * 3. wait_and_pop: Wait until an element is available and then pop it
 * 
 * Your implementation should:
 * - Be thread-safe (multiple threads can call any method concurrently)
 * - Use proper synchronization mechanisms
 * - Handle edge cases (empty queue, etc.)
 * - Avoid common concurrency pitfalls like race conditions and deadlocks
 */

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

template<typename T>
class ThreadSafeQueue {
private:
    // TODO: Add your private member variables here
    
public:
    ThreadSafeQueue() {
        // TODO: Initialize your member variables if needed
    }
    
    // Add an element to the queue
    void push(T value) {
        // TODO: Implement this method
    }
    
    // Try to pop an element from the queue
    // Returns true if successful, false if queue was empty
    bool try_pop(T& value) {
        // TODO: Implement this method
        return false;
    }
    
    // Wait until an element is available and then pop it
    void wait_and_pop(T& value) {
        // TODO: Implement this method
    }
    
    // Check if the queue is empty
    bool empty() const {
        // TODO: Implement this method
        return true;
    }
};

// Test function for producers
void producer(ThreadSafeQueue<int>& queue, int id, int num_items) {
    for (int i = 0; i < num_items; i++) {
        int value = id * 1000 + i;
        queue.push(value);
        std::cout << "Producer " << id << " pushed: " << value << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50 + (std::rand() % 100)));
    }
}

// Test function for consumers
void consumer(ThreadSafeQueue<int>& queue, int id, std::atomic<bool>& done, std::atomic<int>& items_processed) {
    while (!done || !queue.empty()) {
        int value;
        if (queue.try_pop(value)) {
            std::cout << "Consumer " << id << " popped: " << value << std::endl;
            items_processed++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + (std::rand() % 150)));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    ThreadSafeQueue<int> queue;
    std::atomic<bool> done(false);
    std::atomic<int> items_processed(0);
    
    const int num_producers = 3;
    const int num_consumers = 2;
    const int items_per_producer = 5;
    const int total_items = num_producers * items_per_producer;
    
    std::vector<std::thread> threads;
    
    // Start producer threads
    for (int i = 0; i < num_producers; i++) {
        threads.push_back(std::thread(producer, std::ref(queue), i, items_per_producer));
    }
    
    // Start consumer threads
    for (int i = 0; i < num_consumers; i++) {
        threads.push_back(std::thread(consumer, std::ref(queue), i, std::ref(done), std::ref(items_processed)));
    }
    
    // Wait for producers to finish
    for (int i = 0; i < num_producers; i++) {
        threads[i].join();
    }
    
    // Wait until all items have been processed
    while (items_processed < total_items) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Signal consumers to finish and wait for them
    done = true;
    for (int i = num_producers; i < num_producers + num_consumers; i++) {
        threads[i].join();
    }
    
    std::cout << "All items processed successfully!" << std::endl;
    return 0;
}
