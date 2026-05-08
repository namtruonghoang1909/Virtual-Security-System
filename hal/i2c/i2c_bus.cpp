#include "i2c_bus.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace hal {

I2CBus::I2CBus(const std::string& device_path) : _fd(-1), _device_path(device_path)
{
    _fd = open(_device_path.c_str(), O_RDWR);
    if(_fd < 0){
        std::cerr << "Failed to open I2C bus: " << _device_path << " (" << strerror(errno) << ")" << std::endl;
    }
}

I2CBus::~I2CBus()
{
    if(_fd >= 0){
        close(_fd);
    }
}

bool I2CBus::set_address(uint8_t slave_address)
{
    if(_fd < 0) return false;
    if(ioctl(_fd, I2C_SLAVE, slave_address) < 0){
        std::cerr << "Failed to set I2C address 0x" << std::hex << (int)slave_address << std::endl;
        return false;
    }
    return true;
}

bool I2CBus::write_byte(uint8_t reg, uint8_t val){
    if(_fd < 0) return false;
    uint8_t buf[2] = {reg, val};
    return (write(_fd, buf, 2) == 2);
}

bool I2CBus::read_byte(uint8_t reg, uint8_t& val)
{
    if(_fd < 0) return false;
    if(write(_fd, &reg, 1) != 1) return false;
    return (read(_fd, &val, 1) == 1);
}

bool I2CBus::read_word(uint8_t reg, uint16_t& value) {
    if (_fd < 0) return false;
    if (write(_fd, &reg, 1) != 1) return false;
    
    uint8_t buf[2];
    if (read(_fd, buf, 2) != 2) return false;
    
    // Combine Big-Endian
    value = (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
    return true;
}

bool I2CBus::read_consecutive_bytes(uint8_t reg_start, uint8_t regs_num, std::vector<uint8_t>& data)
{
    if(_fd < 0) return false;

    if(write(_fd, &reg_start, 1) != 1) return false;

    data.resize(regs_num);

    if(read(_fd, data.data(), regs_num) != static_cast<ssize_t>(regs_num)) {
        return false;
    }

    return true;
}

} // namespace hal
