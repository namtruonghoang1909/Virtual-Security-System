#include "led.hpp"

namespace drivers {

Led::Led(std::shared_ptr<hal::GpioPin> gpio_pin) 
    : _gpio_pin(gpio_pin), _is_on(false) {}

bool Led::initialize() {
    if (!_gpio_pin) return false;
    return _gpio_pin->set_direction(hal::GpioDirection::OUT);
}

void Led::on() {
    if (_gpio_pin) {
        _gpio_pin->write(hal::GpioValue::HIGH);
        _is_on = true;
    }
}

void Led::off() {
    if (_gpio_pin) {
        _gpio_pin->write(hal::GpioValue::LOW);
        _is_on = false;
    }
}

void Led::toggle() {
    if (_is_on) {
        off();
    } else {
        on();
    }
}

} // namespace drivers
