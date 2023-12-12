//  
// JobQueue.hpp
// 
// Description:
//  Generic golang-like job queue
//  Adapted from https://bolu.dev/programming/2020/07/02/go-channels-part2.html
//
// Authors:
//  Samuel García
//  Laura González
//
// Date:
//  10/2022.
//

#ifndef INFORMATICA_GRAFICA_CHANNEL_HPP
#define INFORMATICA_GRAFICA_CHANNEL_HPP

#include <mutex>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <future>
#include <sstream>

template<class T>
class Channel {
protected:
    Channel& operator=(const Channel& other ) = delete;
    Channel(const Channel& other) = delete;

    std::deque<T> data;
    std::mutex mutex;
    std::condition_variable m_cv;

public:
    Channel() {};
    virtual ~Channel() {};

    T receive() {
        std::unique_lock<std::mutex> lock(mutex);

        m_cv.wait(lock, [this] {
            return !empty();
        });

        T val = std::move(data.front());
        data.pop_front();

        return val;
    };

    void send(T&& val) {
        std::unique_lock<std::mutex> the_lock(mutex);

        data.emplace_back(val);
        m_cv.notify_all();
    };

    [[nodiscard]] bool empty() const {
        return data.empty();
    }
};


#endif //INFORMATICA_GRAFICA_CHANNEL_HPP
