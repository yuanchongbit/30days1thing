#ifndef SHARED_CONDITION_HPP
#define SHARED_CONDITION_HPP

#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string>
#include <system_error>

class SharedCondition {
private:
    struct SharedData {
        sem_t mutex;      // 互斥锁
        sem_t waiters;    // 等待者计数信号量
        int waiter_count; // 等待线程数
        int signal_count; // 信号计数
    };

    SharedData* data_;
    std::string name_;
    int fd_;
    bool is_owner_;

public:
    // 创建共享条件变量
    SharedCondition(const std::string& name, bool create = false) 
        : name_(name), is_owner_(create) {
        
        if (create) {
            fd_ = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        } else {
            fd_ = shm_open(name.c_str(), O_RDWR, 0666);
        }

        if (fd_ == -1) {
            throw std::system_error(errno, std::system_category(), 
                "Failed to open shared memory");
        }

        if (create) {
            if (ftruncate(fd_, sizeof(SharedData)) == -1) {
                throw std::system_error(errno, std::system_category(), 
                    "Failed to set size of shared memory");
            }
        }

        data_ = static_cast<SharedData*>(mmap(nullptr, sizeof(SharedData), 
            PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0));

        if (data_ == MAP_FAILED) {
            throw std::system_error(errno, std::system_category(), 
                "Failed to map shared memory");
        }

        if (create) {
            // 初始化信号量
            if (sem_init(&data_->mutex, 1, 1) == -1 ||
                sem_init(&data_->waiters, 1, 0) == -1) {
                throw std::system_error(errno, std::system_category(), 
                    "Failed to initialize semaphores");
            }
            data_->waiter_count = 0;
            data_->signal_count = 0;
        }
    }

    // 等待条件
    void wait() {
        sem_wait(&data_->mutex);
        data_->waiter_count++;
        sem_post(&data_->mutex);

        sem_wait(&data_->waiters);  // 等待信号

        sem_wait(&data_->mutex);
        data_->waiter_count--;
        data_->signal_count--;
        bool last_waiter = (data_->signal_count == 0);
        sem_post(&data_->mutex);

        if (last_waiter) {
            // 最后一个等待者负责重置信号
            sem_post(&data_->mutex);
        }
    }

    // 通知一个等待者
    void notify_one() {
        sem_wait(&data_->mutex);
        if (data_->waiter_count > data_->signal_count) {
            data_->signal_count++;
            sem_post(&data_->waiters);
        }
        sem_post(&data_->mutex);
    }

    // 通知所有等待者
    void notify_all() {
        sem_wait(&data_->mutex);
        if (data_->waiter_count > 0) {
            data_->signal_count = data_->waiter_count;
            for (int i = 0; i < data_->waiter_count; i++) {
                sem_post(&data_->waiters);
            }
        }
        sem_post(&data_->mutex);
    }

    ~SharedCondition() {
        munmap(data_, sizeof(SharedData));
        close(fd_);
        
        if (is_owner_) {
            sem_destroy(&data_->mutex);
            sem_destroy(&data_->waiters);
            shm_unlink(name_.c_str());
        }
    }
};

#endif