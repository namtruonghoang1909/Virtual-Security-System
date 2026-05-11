#ifndef SHARED_MEMORY_MANAGER_HPP
#define SHARED_MEMORY_MANAGER_HPP

#include "telemetry_types.hpp"
#include <string>

namespace middleware {
namespace ipc {

class SharedMemoryManager {
public:
    /**
     * @brief Construct a new Shared Memory Manager object.
     * @param is_creator If true, it will create and initialize the SHM.
     */
    explicit SharedMemoryManager(bool is_creator = false);
    ~SharedMemoryManager();

    /**
     * @brief Access the telemetry data.
     * @return TelemetryData* Pointer to the mapped shared memory.
     */
    TelemetryData* get_data() const { return _data; }

    /**
     * @brief Lock the shared mutex.
     */
    void lock();

    /**
     * @brief Unlock the shared mutex.
     */
    void unlock();

    /**
     * @brief Signal the process-shared condition variable.
     */
    void signal();

    /**
     * @brief Wait for the process-shared condition variable.
     * @param timeout_ms Optional timeout in milliseconds.
     * @return true if signaled, false if timeout.
     */
    bool wait(uint32_t timeout_ms = 0);

private:
    bool _is_creator;
    int _shm_fd;
    TelemetryData* _data;

    void initialize_shm();
    void open_shm();
    void cleanup();
};

} // namespace ipc
} // namespace middleware

#endif // SHARED_MEMORY_MANAGER_HPP
