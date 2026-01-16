#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QtCore/QDateTime>
#include <cstdint>

// DSP Settings structure for radar configuration
#pragma pack(push, 1)
struct DSP_Settings_t {
    // Detection threshold settings
    int16_t detection_threshold;       // Detection threshold in dB (-50 to 50)
    int16_t cfar_threshold;            // CFAR threshold offset in dB (0 to 30)
    
    // Range settings
    float range_min;                   // Minimum detection range in meters (0.0 to 100.0)
    float range_max;                   // Maximum detection range in meters (1.0 to 150.0)
    
    // Speed settings
    float speed_min;                   // Minimum detection speed in m/s (0.0 to 50.0)
    float speed_max;                   // Maximum detection speed in m/s (1.0 to 100.0)
    
    // FFT settings
    uint16_t fft_size;                 // FFT size (64, 128, 256, 512, 1024)
    uint8_t fft_window_type;           // Window type: 0=None, 1=Hann, 2=Hamming, 3=Blackman
    uint8_t fft_averaging;             // Number of FFT averages (1 to 16)
    
    // Filter settings
    uint8_t filter_enabled;            // Master filter enable (0=disabled, 1=enabled)
    uint8_t moving_avg_enabled;        // Moving average filter (0=disabled, 1=enabled)
    uint8_t moving_avg_window;         // Moving average window size (1 to 32)
    uint8_t reserved1;                 // Reserved for alignment
    
    // Line filter settings
    uint8_t line_filter_50hz;          // 50 Hz line filter (0=disabled, 1=enabled)
    uint8_t line_filter_100hz;         // 100 Hz line filter (0=disabled, 1=enabled)
    uint8_t line_filter_150hz;         // 150 Hz line filter (0=disabled, 1=enabled)
    uint8_t reserved2;                 // Reserved for alignment
    
    // Amplification settings
    int16_t amplification;             // Manual amplification in dB (0 to 60)
    uint8_t auto_amplification;        // Auto amplification (0=disabled, 1=enabled)
    uint8_t reserved3;                 // Reserved for alignment
    int16_t auto_amp_inner_threshold;  // Auto amp inner threshold in dB (0 to 100)
    int16_t auto_amp_outer_threshold;  // Auto amp outer threshold in dB (0 to 100)
    
    // Target selection settings
    uint8_t target_selection_mode;     // 0=All, 1=Nearest, 2=Fastest, 3=Strongest
    uint8_t max_targets;               // Maximum number of targets to report (1 to 10)
    uint8_t direction_filter;          // 0=Both, 1=Approaching only, 2=Receding only
    uint8_t reserved4;                 // Reserved for alignment
    
    // Signal processing
    uint8_t noise_floor_tracking;      // Noise floor tracking (0=disabled, 1=enabled)
    uint8_t clutter_removal;           // Clutter removal (0=disabled, 1=enabled)
    uint8_t doppler_compensation;      // Doppler compensation (0=disabled, 1=enabled)
    uint8_t reserved5;                 // Reserved for alignment
    
    // Azimuth settings
    float azimuth_offset;              // Azimuth offset in degrees (-45.0 to 45.0)
    float azimuth_min;                 // Minimum azimuth angle in degrees (-60.0 to 60.0)
    float azimuth_max;                 // Maximum azimuth angle in degrees (-60.0 to 60.0)
    
    // Checksum
    uint16_t checksum;                 // CRC16 checksum for data integrity
    uint16_t reserved6;                // Reserved for alignment
    
    // Default constructor
    DSP_Settings_t() :
        detection_threshold(0),
        cfar_threshold(10),
        range_min(0.5f),
        range_max(50.0f),
        speed_min(0.0f),
        speed_max(50.0f),
        fft_size(256),
        fft_window_type(1),       // Hann window
        fft_averaging(4),
        filter_enabled(1),
        moving_avg_enabled(0),
        moving_avg_window(4),
        reserved1(0),
        line_filter_50hz(0),
        line_filter_100hz(0),
        line_filter_150hz(0),
        reserved2(0),
        amplification(20),
        auto_amplification(0),
        reserved3(0),
        auto_amp_inner_threshold(30),
        auto_amp_outer_threshold(70),
        target_selection_mode(0),
        max_targets(5),
        direction_filter(0),
        reserved4(0),
        noise_floor_tracking(1),
        clutter_removal(0),
        doppler_compensation(0),
        reserved5(0),
        azimuth_offset(0.0f),
        azimuth_min(-45.0f),
        azimuth_max(45.0f),
        checksum(0),
        reserved6(0)
    {}
    
    // Calculate CRC16 checksum
    uint16_t calculateChecksum() const {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(this);
        size_t length = sizeof(DSP_Settings_t) - sizeof(checksum) - sizeof(reserved6);
        uint16_t crc = 0xFFFF;
        for (size_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                if (crc & 1)
                    crc = (crc >> 1) ^ 0xA001;
                else
                    crc >>= 1;
            }
        }
        return crc;
    }
    
    // Update checksum before sending
    void updateChecksum() {
        checksum = calculateChecksum();
    }
    
    // Validate checksum after receiving
    bool validateChecksum() const {
        return checksum == calculateChecksum();
    }
};
#pragma pack(pop)

// Structure to hold target detection data
struct TargetDetection {
    uint32_t target_id;
    float radius;        // distance in meters
    float radial_speed;  // speed in m/s
    float azimuth;       // angle in degrees
    float amplitude;     // signal strength in dB
    qint64 timestamp;     // timestamp when received

    TargetDetection() : target_id(0), radius(0.0), radial_speed(0.0), azimuth(0.0), amplitude(0.0), timestamp(0) {}

    TargetDetection(int id, double r, double speed, double az, double amp = 0.0)
        : target_id(id), radius(r), radial_speed(speed), azimuth(az), amplitude(amp), timestamp(QDateTime::currentMSecsSinceEpoch()) {}
};

// Structure to hold detection data (same as TargetDetection but keeping both for compatibility)
struct DetectionData {
    uint32_t target_id;
    float radius;
    float radial_speed;
    float azimuth;
    float amplitude;
    qint64 timestamp;

    DetectionData() : target_id(0), radius(0.0), radial_speed(0.0), azimuth(0.0), amplitude(0.0), timestamp(0) {}
    DetectionData(int id, double r, double rs, double az, double amp = 0.0)
        : target_id(id), radius(r), radial_speed(rs), azimuth(az), amplitude(amp), timestamp(QDateTime::currentMSecsSinceEpoch()) {}
    
    // Convert to TargetDetection
    TargetDetection toTargetDetection() const {
        return TargetDetection(target_id, radius, radial_speed, azimuth, amplitude);
    }
};

#endif // STRUCTURES_H
