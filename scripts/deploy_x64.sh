#!/bin/bash

# --- VSS: Host (x64) Deployment ---
# Initializes the mock environment and builds the system for local testing.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
BIN_DIR="$PROJECT_ROOT/bin"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}[*] Starting Host (x64) Deployment Flow...${NC}"

# 1. Initialize Mock Sysfs
echo -e "${BLUE}[*] Initializing Host Mock Environment...${NC}"
"$SCRIPT_DIR/virt_env/env_init_x64.sh"

# 2. Build for Host
echo -e "${BLUE}[*] Building for Host (x86_64)...${NC}"
mkdir -p "$BIN_DIR"

g++ -Wall -Wextra -std=c++14 -DMOCK_HARDWARE -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/i2c/i2c_bus.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/sensor/proximity_sensor.cpp" \
    "$PROJECT_ROOT/app/collector/proximity_collector.cpp" \
    -o "$BIN_DIR/proximity_collector_host" -lpthread -lrt

g++ -Wall -Wextra -std=c++14 -DMOCK_HARDWARE -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/gpio/gpio_pin.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/actuator/led.cpp" \
    "$PROJECT_ROOT/drivers/actuator/buzzer.cpp" \
    "$PROJECT_ROOT/app/engine/security_engine.cpp" \
    "$PROJECT_ROOT/app/controller/security_controller.cpp" \
    -o "$BIN_DIR/security_controller_host" -lpthread -lrt

if [ $? -eq 0 ]; then
    echo -e "${GREEN}------------------------------------------------${NC}"
    echo -e "${GREEN}SUCCESS: Environment ready and binaries built.${NC}"
    echo -e "Run the collector and controller from the bin/ directory."
    echo -e "${GREEN}------------------------------------------------${NC}"
else
    echo -e "${RED}[ERROR] Host build failed.${NC}"
    exit 1
fi
