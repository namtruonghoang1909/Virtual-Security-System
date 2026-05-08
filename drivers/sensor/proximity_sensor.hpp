#ifndef PROXIMITY_SENSOR_HPP
#define PROXIMITY_SENSOR_HPP

#include "../../hal/i2c/i2c_bus.hpp"
#include <memory>

namespace drivers {

/**
 * @brief Driver for the VL53L0X Proximity Sensor.
 */
class ProximitySensor {
public:
    /**
     * @brief Construct a new Proximity Sensor object.
     * @param i2c_bus Shared pointer to the I2C bus implementation (Dependency Injection).
     * @param address I2C slave address of the sensor (default 0x29).
     */
    explicit ProximitySensor(std::shared_ptr<hal::I2CBus> i2c_bus, uint8_t address = 0x29);

    /**
     * @brief Initialize the sensor.
     * @return true if successful, false otherwise.
     */
    bool initialize();

    /**
     * @brief Read the distance from the sensor.
     * @param distance_mm Reference to store the distance in millimeters.
     * @return true if successful, false otherwise.
     */
    bool get_distance(uint16_t& distance_mm);

private:
    std::shared_ptr<hal::I2CBus> _i2c_bus;
    uint8_t _address;

    static constexpr uint8_t REG_DISTANCE_MSB = 0x1E;
    static constexpr uint8_t REG_DISTANCE_LSB = 0x1F;
};

} // namespace drivers

#endif // PROXIMITY_SENSOR_HPP
