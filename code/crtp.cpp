#include <iostream>
#include <chrono>

// 传统虚函数实现
class VirtualBase {
public:
    virtual ~VirtualBase() = default;
    virtual void process() = 0;
};

class VirtualDerived : public VirtualBase {
public:
    void process() override {
        // 防止被优化掉
        volatile int dummy = 0;
        (void)dummy;
    }
};

// CRTP实现
template <typename Derived>
class CRTPBase {
public:
    void process() {
        static_cast<Derived*>(this)->process_impl();
    }
};

class CRTPDerived : public CRTPBase<CRTPDerived> {
public:
    void process_impl() {
        // 同样防止优化
        volatile int dummy = 0;
        (void)dummy;
    }
};

// 性能测试函数
template <typename T>
void benchmark(const char* name) {
    // 千万次调用测试
    const int iterations = 10'000'000;
    T obj;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        obj.process();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << name << " time: " << duration.count() << " μs ("
              << duration.count() / double(iterations) << " μs per call)\n";
}

int main() {

    // 注意这里编译时应采用O2或O3优化，否则没有区别，
    // 因为CRTP的优化是在编译期，函数调用可被内联优化，减少了虚函数查表的过程。
    benchmark<VirtualDerived>("Virtual call");
    benchmark<CRTPDerived>("CRTP call");
    return 0;
}
