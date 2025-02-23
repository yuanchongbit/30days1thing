#include "shared_condition.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    // 创建共享条件变量
    SharedCondition cond("/my_condition", true);
    
    std::cout << "Producer started" << std::endl;
    
    while (true) {
        sleep(2);  // 每2秒产生一个信号
        std::cout << "Producing signal..." << std::endl;
        cond.notify_one();  // 通知一个消费者
    }
    
    return 0;
}