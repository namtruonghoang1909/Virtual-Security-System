#!/bin/bash

# Cross-Compilation Script for Virtual Security System
# Target: ARM64 (QEMU)

# Set the project root to the directory where this script is located's parent
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
OUTPUT_DIR="$PROJECT_ROOT/bin"

# Compiler path (Update this for your environment)
COMPILER="/home/nam/buildroot/output/host/bin/aarch64-linux-g++"

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

echo "--- Building Virtual Security System for ARM64 ---"

# 1. Build sensor_provider
echo "[*] Compiling sensor_provider..."
$COMPILER -Wall -Wextra -std=c++14 \
    -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/i2c/i2c_bus.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/sensor/proximity_sensor.cpp" \
    "$PROJECT_ROOT/app/proximity_collector.cpp" \
    -o "$OUTPUT_DIR/proximity_collector" -lpthread -lrt

if [ $? -eq 0 ]; then
    echo "[SUCCESS] sensor_provider created at: $OUTPUT_DIR/sensor_provider"
else
    echo "[FAILED] sensor_provider build failed."
    exit 1
fi

# 2. Build security_controller
echo "[*] Compiling security_controller..."
$COMPILER -Wall -Wextra -std=c++14 \
    -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/gpio/gpio_pin.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/actuator/led.cpp" \
    "$PROJECT_ROOT/drivers/actuator/buzzer.cpp" \
    "$PROJECT_ROOT/app/security_engine.cpp" \
    "$PROJECT_ROOT/app/security_controller.cpp" \
    -o "$OUTPUT_DIR/security_controller" -lpthread -lrt

if [ $? -eq 0 ]; then
    echo "[SUCCESS] security_controller created at: $OUTPUT_DIR/security_controller"
else
    echo "[FAILED] security_controller build failed."
    exit 1
fi

echo "--- Build Process Complete ---"
