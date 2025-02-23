#include <iostream>
#include <cstring>

class String {
public:
    String() : data_(new char[1]), size_(0) {
        data_[0] = '\0';
    }

    // 带参数的构造函数
    String(const char* str) {
        size_ = strlen(str);
        data_ = new char[size_ + 1];
        strcpy(data_, str);
    }
    
    // 拷贝构造
    // 在 C++ 中，类的私有成员虽然对类外部是不可访问的，但是对于同一个类的其他实例来说是可以访问的。这是 C++ 的一个重要特性：私有成员的访问限制是基于类而不是基于对象的。所以这里, str可以直接访问size_和data_
    String (const String& str) {
        size_ = str.size_;
        data_ = new char[size_+1];
        strcpy(data_, str.data_);
    }

    
    // 移动构造
    String(String&& str) {
        data_ = str.data_;
        size_ = str.size_;
        str.data_ = nullptr;
        str.size_ = 0;
    }

    // 析构函数
    ~String() {
        delete[] data_;
    }

    // 拷贝赋值运算符
    String& operator=(const String& str) noexcept{
        if (this != &str) {
            delete[] data_;
            size_ = str.size_;
            data_ = new char[size_ +1];
            strcpy(data_, str.data_);
        }
        
        return *this;
    }

    // 字符串连接
    // 第二个const 表示,本函数不会修改非mutable的成员变量,
    // 并且让const对象调用
    String operator+(const String& other) const {
        String result;
        result.size_ = other.size_ + size_;
        result.data_ = new char[result.size_ + 1];
        strcpy(result.data_, data_);
        strcpy(result.data_+size_, other.data_);

        return result;
    }

    char& operator[](size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // const版本
    // 提供给const对象使用,返回类型为const,
    // 这样就使得const对象不能修改内容
    const char& operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    size_t length() {
        return size_;
    }

    bool empty() {
        return size_ == 0;
    }

    void clear() {
        delete[] data_;
        data_ = new char[1];
        data_[0] = '\0';
        size_ = 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.data_;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, String& str) {
        char buffer[1024];
        is >> buffer;
        str = String(buffer);
        return is;
    }

private:
    char* data_;
    size_t size_;

};


int main () {
    String str("12345");

    std::cout << str << std::endl;
    std::cout << str[1] << std::endl;

    std::cout << String("111111") + String("22222") << std::endl;

    

    return 0;
}