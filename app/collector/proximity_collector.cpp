#include "../../hal/i2c/i2c_bus.hpp"
#include "../../middlewares/ipc/shared_mem/shared_memory_manager.hpp"
#include "../../drivers/sensor/proximity_sensor.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signal_handler(int) {
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        std::cout << "[Proximity Collector] Initializing..." << std::endl;

        // 1. Initialize HAL & Driver
        auto i2c_bus = std::make_shared<hal::I2CBus>("/dev/i2c-1");
        drivers::ProximitySensor sensor(i2c_bus);
        if (!sensor.initialize()) {
            std::cerr << "[Proximity Collector] Hardware failure." << std::endl;
            return 1;
        }

        // 2. Initialize Middleware (IPC Creator)
        middleware::ipc::SharedMemoryManager shm(true);
        auto* telemetry = shm.get_data();

        std::cout << "[Proximity Collector] Polling at 50Hz (Signaling every 5 samples)..." << std::endl;

        uint16_t distance = 0;
        uint32_t heartbeat_tick = 0;

        while (running) {
            // Check system state in SHM
            shm.lock();
            bool is_armed = telemetry->is_armed;
            shm.unlock();

            if (is_armed) {
                if (sensor.get_distance(distance)) {
                    shm.lock();
                    
                    // Write to the 5-sample buffer
                    telemetry->samples[telemetry->sample_index] = distance;
                    telemetry->sample_index++;

                    // Heartbeat update
                    telemetry->heartbeat = ++heartbeat_tick;

                    // Check if buffer is full
                    if (telemetry->sample_index >= 5) {
                        telemetry->sample_index = 0;
                        telemetry->batch_counter++;
                        
                        // Signal the Consumer (security_controller)
                        shm.signal();
                    }

                    shm.unlock();
                } else {
                    std::cerr << "[Proximity Collector] Sensor read error." << std::endl;
                }
            } else {
                // System disarmed: still update heartbeat so controller knows we are alive
                shm.lock();
                telemetry->heartbeat = ++heartbeat_tick;
                shm.unlock();
            }

            // High-frequency polling (50Hz = 20ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        std::cout << "[Proximity Collector] Shutdown complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Proximity Collector] Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
