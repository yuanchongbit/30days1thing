#include <atomic>
#include <thread>
#include <functional>
#include <iostream>

class Timer {
public:
    Timer() = default;
    // 禁止拷贝构造和赋值操作
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    
    void start(int interval, std::function<void()> callback) {
        activate_ = true;
        // 这里=表示以值捕获外部作用域的所有变量
        // 与之相对应的是&，以引用的方式捕获变量
        // [=, &x] - 除 x 以引用捕获外，其他都以值捕获
        // [&, x] - 除 x 以值捕获外，其他都以引用捕获
        timer_thread_ = std::thread([=](){
            while(activate_) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(interval)
                );
                if (activate_) {
                    callback();
                }
            }
        });
    }

    void stop() {
        activate_ = false;
        if (timer_thread_.joinable()) {
            timer_thread_.join();
        }
    }

    ~Timer() {
        stop();
    }


private:
    std::atomic<bool> activate_{false};
    std::thread timer_thread_;
};

int main() {
    Timer timer;

    timer.start(100, []() {
        std::cout << "timer triggered" << std::endl;
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(10));

    timer.stop();

    return 0;
}
