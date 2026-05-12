#include "security_engine.hpp"
#include <numeric>
#include <algorithm>
#include <iostream>

namespace app {

SecurityEngine::SecurityEngine() : _is_armed(true), _alarm_active(false), _filtered_distance(0) {}

bool SecurityEngine::process_batch(const std::vector<uint16_t>& samples) {
    if (!_is_armed || samples.empty()) {
        _alarm_active = false;
        return false;
    }

    _filtered_distance = filter_noise(samples);

    // Hysteresis logic
    if (!_alarm_active) {
        if (_filtered_distance < ALARM_THRESHOLD_ON_MM) {
            _alarm_active = true;
        }
    } else {
        if (_filtered_distance > ALARM_THRESHOLD_OFF_MM) {
            _alarm_active = false;
        }
    }

    return _alarm_active;
}

void SecurityEngine::set_armed(bool armed) {
    _is_armed = armed;
    std::cout << "[Security Engine] System " << (_is_armed ? "ARMED" : "DISARMED") << std::endl;
}

uint16_t SecurityEngine::filter_noise(const std::vector<uint16_t>& samples) {
    // Implement a Median Filter for robustness against spikes
    std::vector<uint16_t> sorted_samples = samples;
    std::sort(sorted_samples.begin(), sorted_samples.end());
    
    // Return the median (middle element of 5)
    return sorted_samples[2];
}

} // namespace app
