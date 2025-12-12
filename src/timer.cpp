#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <functional>

#include "timer.h"

timer::timer() : is_running(false) {}

void timer::start(std::chrono::milliseconds total_duration, std::chrono::milliseconds interval, std::function<void()> task) {
    stop(); // 确保之前的计时器停止
    is_running = true;
    timer_future = std::async(std::launch::async, [=]() {
        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time + total_duration;
        auto next_check = start_time;

        while (is_running.load() && std::chrono::steady_clock::now() < end_time) {
            std::this_thread::sleep_until(next_check);
            next_check += interval;
            if (is_running.load()) {
                task();
            }
        }
        is_running = false; // 确保在结束时将 is_running 设置为 false
    });
}

void timer::stop() {
    is_running = false;
    if (timer_future.valid()) {
        timer_future.wait();
    }
}

timer::~timer() {
    stop();
}