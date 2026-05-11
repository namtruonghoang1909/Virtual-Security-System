#!/bin/bash

# Configuration for VL53L0X Mock
I2C_ADDR="0x29"
DEBUG_FS_PATH="/sys/kernel/debug/i2c-stub"
# Find the specific bus folder (usually 0-0029)
BUS_DIR=$(ls -d $DEBUG_FS_PATH/*$I2C_ADDR 2>/dev/null)

echo "--- I2C Virtual Data Creator ---"

# 1. Load the i2c-stub module if not present
if ! lsmod | grep -q "i2c_stub"; then
    echo "[*] Loading i2c-stub for address $I2C_ADDR..."
    modprobe i2c-stub chip_addr=$I2C_ADDR
    sleep 1
    BUS_DIR=$(ls -d $DEBUG_FS_PATH/*$I2C_ADDR 2>/dev/null)
fi

# 2. Ensure debugfs is mounted
if [ ! -d "/sys/kernel/debug/i2c-stub" ]; then
    echo "[*] Mounting debugfs..."
    mount -t debugfs none /sys/kernel/debug
fi

if [ -z "$BUS_DIR" ]; then
    echo "[!] Error: i2c-stub directory not found for address $I2C_ADDR."
    exit 1
fi

echo "[+] I2C Virtual Sensor ready at $BUS_DIR"
echo "[+] Starting data emulation (Distance: 500mm -> 50mm)..."

# 3. Continuous Data Emulation Loop
while true; do
    for dist in {500..50..-50}; do
        # Split 16-bit distance into High and Low bytes
        # Using registers 0x1E (MSB) and 0x1F (LSB)
        high_byte=$(printf "0x%02x" $((dist >> 8)))
        low_byte=$(printf "0x%02x" $((dist & 0xFF)))
        
        # Write to the virtual registers in debugfs
        echo $high_byte > "$BUS_DIR/1e"
        echo $low_byte > "$BUS_DIR/1f"
        
        sleep 0.5
    done
    
    # Simulate a pause where no one is near
    echo "0x01" > "$BUS_DIR/1e" # 500mm
    echo "0xf4" > "$BUS_DIR/1f"
    sleep 2
done
