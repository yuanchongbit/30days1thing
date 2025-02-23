#include "shared_condition.hpp"
#include <iostream>

int main() {
    // 打开已存在的共享条件变量
    SharedCondition cond("/my_condition", false);
    
    std::cout << "Consumer started" << std::endl;
    
    while (true) {
        std::cout << "Waiting for signal..." << std::endl;
        cond.wait();  // 等待信号
        std::cout << "Signal received!" << std::endl;
    }
    
    return 0;
}