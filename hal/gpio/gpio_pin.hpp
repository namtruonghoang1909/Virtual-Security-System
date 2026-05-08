#ifndef GPIO_PIN_HPP
#define GPIO_PIN_HPP

#include <string>

namespace hal {

/**
 * @brief Direction for GPIO pins.
 */
enum class GpioDirection {
    IN,
    OUT
};

/**
 * @brief Logic level for GPIO pins.
 */
enum class GpioValue {
    LOW = 0,
    HIGH = 1
};

/**
 * @brief Hardware Abstraction Layer for a single GPIO pin using sysfs.
 */
class GpioPin {
public:
    /**
     * @brief Construct a new Gpio Pin object.
     * @param pin_number The Linux GPIO number.
     */
    explicit GpioPin(int pin_number);
    virtual ~GpioPin();

    /**
     * @brief Set the pin direction.
     * @param dir GpioDirection::IN or GpioDirection::OUT.
     * @return true if successful.
     */
    virtual bool set_direction(GpioDirection dir);

    /**
     * @brief Write a value to the pin (must be OUT).
     * @param val GpioValue::LOW or GpioValue::HIGH.
     * @return true if successful.
     */
    virtual bool write(GpioValue val);

    /**
     * @brief Read the current value of the pin.
     * @param val Reference to store the value.
     * @return true if successful.
     */
    virtual bool read(GpioValue& val);

private:
    int _pin_number;
    std::string _base_path;

    bool write_sysfs(const std::string& path, const std::string& value);
    bool read_sysfs(const std::string& path, std::string& value);

    // Prevent copying (RAII)
    GpioPin(const GpioPin&) = delete;
    GpioPin& operator=(const GpioPin&) = delete;
};

} // namespace hal

#endif // GPIO_PIN_HPP
