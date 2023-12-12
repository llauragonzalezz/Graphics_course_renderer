//  
// ThreadSafeCounter.hpp
// 
// Description:
//  Contador a compartir entre hilos
// 
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  12/2022.
//

#ifndef INFORMATICA_GRAFICA_THREADSAFECOUNTER_HPP
#define INFORMATICA_GRAFICA_THREADSAFECOUNTER_HPP

#include <shared_mutex>
#include <mutex>

// Adapted from https://en.cppreference.com/w/cpp/thread/shared_mutex
class ThreadSafeCounter {
public:
    ThreadSafeCounter() {};

    unsigned int get() const {
        std::shared_lock lock(mutex_);
        return value_;
    }

    void increment(unsigned int inc) {
        std::unique_lock lock(mutex_);
        value_ += inc;
    }

    void reset() {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    unsigned int value_ = 0;
};

#endif //INFORMATICA_GRAFICA_THREADSAFECOUNTER_HPP
