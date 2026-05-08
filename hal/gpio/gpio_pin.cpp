#include "gpio_pin.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

namespace hal {

GpioPin::GpioPin(int pin_number) : _pin_number(pin_number) {
    _base_path = "/sys/class/gpio/gpio" + std::to_string(_pin_number);

    // Check if exported, if not, export it
    struct stat st;
    if (stat(_base_path.c_str(), &st) != 0) {
        write_sysfs("/sys/class/gpio/export", std::to_string(_pin_number));
        // Small delay to allow udev to set permissions
        usleep(100000); 
    }
}

GpioPin::~GpioPin() {
    // Usually, we don't unexport in a real system to avoid toggling glitches,
    // but for this prototype, we'll cleanup.
    write_sysfs("/sys/class/gpio/unexport", std::to_string(_pin_number));
}

bool GpioPin::set_direction(GpioDirection dir) {
    std::string dir_str = (dir == GpioDirection::IN) ? "in" : "out";
    return write_sysfs(_base_path + "/direction", dir_str);
}

bool GpioPin::write(GpioValue val) {
    std::string val_str = (val == GpioValue::LOW) ? "0" : "1";
    return write_sysfs(_base_path + "/value", val_str);
}

bool GpioPin::read(GpioValue& val) {
    std::string val_str;
    if (!read_sysfs(_base_path + "/value", val_str)) {
        return false;
    }
    val = (val_str == "0") ? GpioValue::LOW : GpioValue::HIGH;
    return true;
}

bool GpioPin::write_sysfs(const std::string& path, const std::string& value) {
    std::ofstream fs(path);
    if (!fs.is_open()) {
        std::cerr << "GpioPin: Failed to open " << path << " for writing." << std::endl;
        return false;
    }
    fs << value;
    return fs.good();
}

bool GpioPin::read_sysfs(const std::string& path, std::string& value) {
    std::ifstream fs(path);
    if (!fs.is_open()) {
        std::cerr << "GpioPin: Failed to open " << path << " for reading." << std::endl;
        return false;
    }
    fs >> value;
    return fs.good();
}

} // namespace hal
