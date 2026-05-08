#include "buzzer.hpp"

namespace drivers {

Buzzer::Buzzer(std::shared_ptr<hal::GpioPin> gpio_pin) 
    : _gpio_pin(gpio_pin) {}

bool Buzzer::initialize() {
    if (!_gpio_pin) return false;
    return _gpio_pin->set_direction(hal::GpioDirection::OUT);
}

void Buzzer::on() {
    if (_gpio_pin) {
        _gpio_pin->write(hal::GpioValue::HIGH);
    }
}

void Buzzer::off() {
    if (_gpio_pin) {
        _gpio_pin->write(hal::GpioValue::LOW);
    }
}

} // namespace drivers
