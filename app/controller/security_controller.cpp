#include "../../middlewares/ipc/shared_mem/shared_memory_manager.hpp"
#include "../../hal/gpio/gpio_pin.hpp"
#include "../../drivers/actuator/led.hpp"
#include "../../drivers/actuator/buzzer.hpp"
#include "../engine/security_engine.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <csignal>
#include <atomic>

// --- Global State & Synchronization ---
std::atomic<bool> running(true);
std::mutex alarm_mtx;
std::condition_variable alarm_cv;
bool alarm_trigger_pending = false;

void signal_handler(int) {
    running = false;
}

/**
 * @brief Thread 2: Alarm Handler (High Priority)
 * Wakes up instantly via std::condition_variable to drive actuators.
 */
void alarm_handler_thread(std::shared_ptr<drivers::Led> led, std::shared_ptr<drivers::Buzzer> buzzer) {
    std::cout << "[Alarm Handler] Thread started." << std::endl;
    
    while (running) {
        std::unique_lock<std::mutex> lock(alarm_mtx);
        alarm_cv.wait(lock, []{ return alarm_trigger_pending || !running; });

        if (!running) break;

        if (alarm_trigger_pending) {
            std::cout << "[Alarm Handler] !!! ALERT SIGNAL RECEIVED !!!" << std::endl;
            
            // Pattern: Flash LED and sound buzzer
            for (int i = 0; i < 5 && alarm_trigger_pending; ++i) {
                led->on();
                buzzer->on();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                led->off();
                buzzer->off();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            alarm_trigger_pending = false;
        }
    }
    std::cout << "[Alarm Handler] Thread shutting down." << std::endl;
}

/**
 * @brief Thread 1: Proximity Processor (Consumer)
 * Wakes up via POSIX Condition Variable when 5 samples are ready.
 */
int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        std::cout << "[Security Controller] Initializing..." << std::endl;

        // 1. Initialize Actuators (GPIO 17 for LED, 18 for Buzzer - Example)
        auto led_pin = std::make_shared<hal::GpioPin>(17);
        auto buzzer_pin = std::make_shared<hal::GpioPin>(18);
        auto led = std::make_shared<drivers::Led>(led_pin);
        auto buzzer = std::make_shared<drivers::Buzzer>(buzzer_pin);
        
        led->initialize();
        buzzer->initialize();

        // 2. Initialize Logic & IPC
        app::SecurityEngine engine;
        middleware::ipc::SharedMemoryManager shm(false); // Consumer
        auto* telemetry = shm.get_data();

        // Synchronize initial state
        shm.lock();
        telemetry->is_armed = engine.is_armed();
        telemetry->alarm_active = false;
        shm.unlock();

        // 3. Start Alarm Handler Thread
        std::thread alarm_thread(alarm_handler_thread, led, buzzer);

        std::cout << "[Security Controller] Monitoring via Event-Driven IPC..." << std::endl;

        int timeout_count = 0;
        const int MAX_TIMEOUTS = 5; // 500ms total without heartbeat = Fail-Safe

        while (running) {
            shm.lock();
            
            // Wait for 5 samples (100ms timeout for heartbeat check)
            if (shm.wait(100)) {
                timeout_count = 0;
                
                // Data is ready! Copy batch to local vector
                std::vector<uint16_t> batch(telemetry->samples, telemetry->samples + 5);
                shm.unlock();

                // Process the batch (Proximity Processor Logic)
                if (engine.process_batch(batch)) {
                    {
                        std::lock_guard<std::mutex> lock(alarm_mtx);
                        alarm_trigger_pending = true;
                    }
                    alarm_cv.notify_one(); // Signal Stage 2
                    
                    // Update SHM status
                    shm.lock();
                    telemetry->alarm_active = true;
                    shm.unlock();
                } else {
                    shm.lock();
                    telemetry->alarm_active = false;
                    shm.unlock();
                }

            } else {
                // Timeout - check for process health
                timeout_count++;
                
                if (timeout_count >= MAX_TIMEOUTS) {
                    std::cerr << "[Security Controller] CRITICAL: Producer heartbeat lost! Entering Fail-Safe mode." << std::endl;
                    {
                        std::lock_guard<std::mutex> lock(alarm_mtx);
                        alarm_trigger_pending = true;
                    }
                    alarm_cv.notify_one();
                    
                    // Reset timeout count to prevent spamming, but keep alarm active
                    timeout_count = 0; 
                }
                shm.unlock();
            }
        }

        // Cleanup
        {
            std::lock_guard<std::mutex> lock(alarm_mtx);
            alarm_trigger_pending = false;
            alarm_cv.notify_all();
        }
        if (alarm_thread.joinable()) alarm_thread.join();

        std::cout << "[Security Controller] Shutdown complete." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Security Controller] Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
