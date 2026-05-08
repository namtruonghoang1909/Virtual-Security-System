#ifndef I2C_BUS_HPP
#define I2C_BUS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace hal {

/**
 * @brief Hardware Abstraction Layer for I2C communication.
 */
class I2CBus {
public:
    explicit I2CBus(const std::string& device_path);
    virtual ~I2CBus();

    virtual bool set_address(uint8_t slave_address);
    virtual bool write_byte(uint8_t reg, uint8_t value);
    virtual bool read_byte(uint8_t reg, uint8_t& value);
    virtual bool read_word(uint8_t reg, uint16_t& value);
    virtual bool read_consecutive_bytes(uint8_t reg_start, uint8_t regs_num, std::vector<uint8_t>& data);

private:
    int _fd;
    std::string _device_path;

    // Prevent copying (RAII)
    I2CBus(const I2CBus&) = delete;
    I2CBus& operator=(const I2CBus&) = delete;
};

} // namespace hal

#endif // I2C_BUS_HPP
