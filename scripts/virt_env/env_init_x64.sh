#!/bin/bash

# --- Virtual Security System: Environment Initialization ---
# This script sets up the mock hardware structure in Sysfs for local testing.

MOCK_SYSFS="/tmp/mock_sysfs"
GPIO_PATH="$MOCK_SYSFS/class/gpio"
I2C_PATH="$MOCK_SYSFS/bus/i2c/devices"

echo "[*] Setting up Mock Sysfs at $MOCK_SYSFS..."

# 1. Create GPIO Mock Structure (Pins 17 and 18)
mkdir -p "$GPIO_PATH/gpio17"
mkdir -p "$GPIO_PATH/gpio18"

echo "out" > "$GPIO_PATH/gpio17/direction"
echo "0"   > "$GPIO_PATH/gpio17/value"
echo "out" > "$GPIO_PATH/gpio18/direction"
echo "0"   > "$GPIO_PATH/gpio18/value"

# 2. Create export/unexport files
touch "$GPIO_PATH/export"
touch "$GPIO_PATH/unexport"

echo "[SUCCESS] Mock environment initialized."
echo "[HINT] To use this mock, the application must be compiled with -DMOCK_HARDWARE"
echo "       and point to $MOCK_SYSFS instead of /sys."