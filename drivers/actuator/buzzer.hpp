#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "../../hal/gpio/gpio_pin.hpp"
#include <memory>

namespace drivers {

/**
 * @brief Driver for a simple Piezo Buzzer.
 */
class Buzzer {
public:
    /**
     * @brief Construct a new Buzzer object.
     * @param gpio_pin Shared pointer to the GPIO pin (Dependency Injection).
     */
    explicit Buzzer(std::shared_ptr<hal::GpioPin> gpio_pin);

    /**
     * @brief Initialize the Buzzer (sets direction to OUT).
     * @return true if successful.
     */
    bool initialize();

    /**
     * @brief Activate the buzzer.
     */
    void on();

    /**
     * @brief Deactivate the buzzer.
     */
    void off();

private:
    std::shared_ptr<hal::GpioPin> _gpio_pin;
};

} // namespace drivers

#endif // BUZZER_HPP
