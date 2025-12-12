#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <functional>

class timer {
public:
    timer();
    ~timer();
    void start(std::chrono::milliseconds total_duration, std::chrono::milliseconds interval, std::function<void()> task);
    void stop();

private:
    std::atomic<bool> is_running;
    std::future<void> timer_future;
};