#ifndef SECURITY_ENGINE_HPP
#define SECURITY_ENGINE_HPP

#include <cstdint>
#include <vector>

namespace app {

/**
 * @brief Application Layer class managing the security system logic.
 * Handles noise filtering and alarm decision making.
 */
class SecurityEngine {
public:
    SecurityEngine();

    /**
     * @brief Process a batch of samples.
     * @param samples Vector of 5 proximity samples in mm.
     * @return true if an alarm should be triggered.
     */
    bool process_batch(const std::vector<uint16_t>& samples);

    /**
     * @brief Arm or disarm the system.
     * @param armed True to arm, false to disarm.
     */
    void set_armed(bool armed);

    bool is_armed() const { return _is_armed; }
    uint16_t get_filtered_distance() const { return _filtered_distance; }

private:
    bool _is_armed;
    bool _alarm_active;
    uint16_t _filtered_distance;
    
    static constexpr uint16_t ALARM_THRESHOLD_ON_MM = 100;
    static constexpr uint16_t ALARM_THRESHOLD_OFF_MM = 150; // Hysteresis

    /**
     * @brief Calculate a simple moving average or median to filter noise.
     */
    uint16_t filter_noise(const std::vector<uint16_t>& samples);
};

} // namespace app

#endif // SECURITY_ENGINE_HPP
