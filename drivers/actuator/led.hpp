#ifndef LED_HPP
#define LED_HPP

#include "../../hal/gpio/gpio_pin.hpp"
#include <memory>

namespace drivers {

/**
 * @brief Driver for a standard LED actuator.
 */
class Led {
public:
    /**
     * @brief Construct a new Led object.
     * @param gpio_pin Shared pointer to the GPIO pin (Dependency Injection).
     */
    explicit Led(std::shared_ptr<hal::GpioPin> gpio_pin);

    /**
     * @brief Initialize the LED (sets direction to OUT).
     * @return true if successful.
     */
    bool initialize();

    /**
     * @brief Turn the LED on.
     */
    void on();

    /**
     * @brief Turn the LED off.
     */
    void off();

    /**
     * @brief Toggle the LED state.
     */
    void toggle();

private:
    std::shared_ptr<hal::GpioPin> _gpio_pin;
    bool _is_on;
};

} // namespace drivers

#endif // LED_HPP
