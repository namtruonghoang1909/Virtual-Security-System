#include "proximity_sensor.hpp"
#include <iostream>

namespace drivers {

ProximitySensor::ProximitySensor(std::shared_ptr<hal::I2CBus> i2c_bus, uint8_t address)
    : _i2c_bus(i2c_bus), _address(address) {}

bool ProximitySensor::initialize() {
    if (!_i2c_bus) return false;
    
    // Set slave address
    if (!_i2c_bus->set_address(_address)) {
        std::cerr << "ProximitySensor: Failed to set I2C address 0x" << std::hex << (int)_address << std::dec << std::endl;
        return false;
    }

    // In a real VL53L0X, we would do complex initialization here.
    // For this mock-based system, we just ensure we can talk to the device.
    uint8_t dummy;
    if (!_i2c_bus->read_byte(REG_DISTANCE_MSB, dummy)) {
        std::cerr << "ProximitySensor: Failed to communicate with sensor at 0x" << std::hex << (int)_address << std::dec << std::endl;
        return false;
    }

    return true;
}

bool ProximitySensor::get_distance(uint16_t& distance_mm) {
    if (!_i2c_bus) return false;

    // Ensure we are talking to the right device
    if (!_i2c_bus->set_address(_address)) return false;

    // Read 16-bit distance from registers 0x1E and 0x1F
    if (!_i2c_bus->read_word(REG_DISTANCE_MSB, distance_mm)) {
        return false;
    }

    return true;
}

} // namespace drivers
