#!/bin/bash

# --- VSS: Target (ARM64) Deployment ---
# Builds the system for ARM64 and deploys it to the QEMU target.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
BIN_DIR="$PROJECT_ROOT/bin"

# Configuration for QEMU
QEMU_PORT=2222
QEMU_USER="root"
QEMU_HOST="localhost"
QEMU_DEST="/root/vss"

# Default Cross-Compiler (can be overridden by ARM_CXX env var)
DEFAULT_ARM_CXX="/home/nam/Projects/buildroot/output/host/bin/aarch64-linux-g++"
ARM_CXX="${ARM_CXX:-$DEFAULT_ARM_CXX}"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}[*] Starting Target (ARM64) Deployment Flow...${NC}"

# 1. Build for Target
echo -e "${BLUE}[*] Building for Target (ARM64)...${NC}"
if [ ! -f "$ARM_CXX" ]; then
    echo -e "${RED}[ERROR] Cross-compiler not found at $ARM_CXX${NC}"
    echo "Please set ARM_CXX environment variable to the correct path."
    exit 1
fi
mkdir -p "$BIN_DIR"

$ARM_CXX -Wall -Wextra -std=c++14 -DMOCK_HARDWARE -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/i2c/i2c_bus.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/sensor/proximity_sensor.cpp" \
    "$PROJECT_ROOT/app/collector/proximity_collector.cpp" \
    -o "$BIN_DIR/proximity_collector" -lpthread -lrt

$ARM_CXX -Wall -Wextra -std=c++14 -DMOCK_HARDWARE -I"$PROJECT_ROOT" \
    "$PROJECT_ROOT/hal/gpio/gpio_pin.cpp" \
    "$PROJECT_ROOT/middlewares/ipc/shared_mem/shared_memory_manager.cpp" \
    "$PROJECT_ROOT/drivers/actuator/led.cpp" \
    "$PROJECT_ROOT/drivers/actuator/buzzer.cpp" \
    "$PROJECT_ROOT/app/engine/security_engine.cpp" \
    "$PROJECT_ROOT/app/controller/security_controller.cpp" \
    -o "$BIN_DIR/security_controller" -lpthread -lrt

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Target build failed.${NC}"
    exit 1
fi

# 2. Deploy to QEMU
echo -e "${BLUE}[*] Deploying to QEMU Target at $QEMU_HOST:$QEMU_PORT...${NC}"
scp -P $QEMU_PORT -r "$BIN_DIR" "$QEMU_USER@$QEMU_HOST:$QEMU_DEST/"
scp -P $QEMU_PORT -r "$SCRIPT_DIR/virt_env" "$QEMU_USER@$QEMU_HOST:$QEMU_DEST/"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}------------------------------------------------${NC}"
    echo -e "${GREEN}SUCCESS: Binaries deployed to QEMU.${NC}"
    echo -e "Connect to QEMU via SSH and run the system."
    echo -e "${GREEN}------------------------------------------------${NC}"
else
    echo -e "${RED}[ERROR] Deployment failed.${NC}"
    exit 1
fi
