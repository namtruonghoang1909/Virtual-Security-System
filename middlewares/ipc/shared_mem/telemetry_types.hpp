#ifndef TELEMETRY_TYPES_HPP
#define TELEMETRY_TYPES_HPP

#include <cstdint>
#include <pthread.h>

namespace middleware {
namespace ipc {

/**
 * @brief Structure for sensor data and system state in shared memory.
 * Designed for process-shared synchronization.
 */
struct TelemetryData {
    // Synchronization
    pthread_mutex_t mutex;
    pthread_cond_t  data_ready_cond; // Process-shared condition variable

    // 5-Sample Buffer for Proximity Data
    uint16_t samples[5];
    uint8_t  sample_index;   // Current index (0-4)
    uint32_t batch_counter;  // Incremented every 5 samples

    // System State
    bool is_armed;
    bool alarm_active;

    // Heartbeat for Health Monitoring
    uint32_t heartbeat;
};

// SHM Configuration
const char* const SHM_NAME = "/vss_telemetry";
const size_t SHM_SIZE = sizeof(TelemetryData);

} // namespace ipc
} // namespace middleware

#endif // TELEMETRY_TYPES_HPP
