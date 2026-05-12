#include "shared_memory_manager.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <stdexcept>


namespace middleware {
namespace ipc {

SharedMemoryManager::SharedMemoryManager(bool is_creator) 
    : _is_creator(is_creator), _shm_fd(-1), _data(nullptr) 
{
    if (_is_creator) {
        initialize_shm();
    } else {
        open_shm();
    }
}

SharedMemoryManager::~SharedMemoryManager() {
    cleanup();
}

void SharedMemoryManager::initialize_shm() {
    // Unlink if already exists to ensure fresh start
    shm_unlink(SHM_NAME);

    _shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (_shm_fd < 0) {
        throw std::runtime_error("Failed to create shared memory: " + std::string(strerror(errno)));
    }

    if (ftruncate(_shm_fd, SHM_SIZE) == -1) {
        throw std::runtime_error("Failed to set SHM size: " + std::string(strerror(errno)));
    }

    _data = static_cast<TelemetryData*>(mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _shm_fd, 0));
    if (_data == MAP_FAILED) {
        throw std::runtime_error("Failed to map shared memory: " + std::string(strerror(errno)));
    }

    // Initialize the structure
    std::memset(_data, 0, SHM_SIZE);

    // 1. Initialize process-shared mutex
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    // Add robustness: if a process dies while holding the mutex, the next lock() will return EOWNERDEAD
    pthread_mutexattr_setrobust(&m_attr, PTHREAD_MUTEX_ROBUST);
    
    if (pthread_mutex_init(&(_data->mutex), &m_attr) != 0) {
        pthread_mutexattr_destroy(&m_attr);
        throw std::runtime_error("Failed to initialize process-shared mutex");
    }
    pthread_mutexattr_destroy(&m_attr);

    // 2. Initialize process-shared condition variable
    pthread_condattr_t c_attr;
    pthread_condattr_init(&c_attr);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);
    
    if (pthread_cond_init(&(_data->data_ready_cond), &c_attr) != 0) {
        pthread_condattr_destroy(&c_attr);
        throw std::runtime_error("Failed to initialize process-shared condition variable");
    }
    pthread_condattr_destroy(&c_attr);
}

void SharedMemoryManager::open_shm() {
    // In a real system, we might retry or wait for the creator
    _shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (_shm_fd < 0) {
        throw std::runtime_error("Failed to open shared memory: " + std::string(strerror(errno)));
    }

    _data = static_cast<TelemetryData*>(mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _shm_fd, 0));
    if (_data == MAP_FAILED) {
        throw std::runtime_error("Failed to map shared memory: " + std::string(strerror(errno)));
    }
}

void SharedMemoryManager::lock() {
    if (_data) {
        int rc = pthread_mutex_lock(&(_data->mutex));
        if (rc == EOWNERDEAD) {
            // Previous owner died. Make the mutex consistent again.
            pthread_mutex_consistent(&(_data->mutex));
        }
    }
}

void SharedMemoryManager::unlock() {
    if (_data) {
        pthread_mutex_unlock(&(_data->mutex));
    }
}

void SharedMemoryManager::signal() {
    if (_data) {
        pthread_cond_signal(&(_data->data_ready_cond));
    }
}

bool SharedMemoryManager::wait(uint32_t timeout_ms) {
    if (!_data) return false;

    if (timeout_ms == 0) {
        return (pthread_cond_wait(&(_data->data_ready_cond), &(_data->mutex)) == 0);
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        return (pthread_cond_timedwait(&(_data->data_ready_cond), &(_data->mutex), &ts) == 0);
    }
}

void SharedMemoryManager::cleanup() {
    if (_data && _data != MAP_FAILED) {
        munmap(_data, SHM_SIZE);
    }
    if (_shm_fd >= 0) {
        close(_shm_fd);
    }
    if (_is_creator) {
        shm_unlink(SHM_NAME);
    }
}

} // namespace ipc
} // namespace middleware
