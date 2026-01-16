#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QtCore/QDateTime>
#include <cstdint>

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

// UDP settings for radar communication
#define UDP_SEND_PORT 5000
#define UDP_RECEIVE_PORT 5001
#define UDP_GUI_IP "127.0.0.1"

// Mode flags
typedef enum {
    MODE_RAW_ONLY = 1,
    MODE_TARGET_ONLY = 2,
    MODE_BOTH = 3
} extraction_mode_t;

// DSP command types
typedef enum {
    DSP_CMD_SET = 1,      // Set DSP parameters
    DSP_CMD_GET = 2,      // Get DSP parameters
    DSP_CMD_RESPONSE = 3  // Response with DSP parameters
} dsp_command_type_t;

// DSP settings payload for UDP communication
typedef struct {
    uint8_t range_mvg_avg_length;  // Moving average length
    uint16_t min_range_cm;         // Minimum range (cm)
    uint16_t max_range_cm;         // Maximum range (cm)
    uint16_t min_speed_kmh;        // Minimum speed (km/h)
    uint16_t max_speed_kmh;        // Maximum speed (km/h)
    uint16_t min_angle_degree;     // Minimum angle (degrees)
    uint16_t max_angle_degree;     // Maximum angle (degrees)
    uint16_t range_threshold;      // Range FFT threshold
    uint16_t speed_threshold;      // Doppler FFT threshold
    uint8_t enable_tracking;       // Enable tracking
    uint8_t num_of_tracks;         // Number of active tracks
    uint8_t median_filter_length;  // Depth of median filter
    uint8_t enable_mti_filter;     // Enable MTI filter
    uint16_t mti_filter_length;    // MTI filter length
} DSP_Settings_t;

// DSP message structure for UDP communication
typedef struct {
    uint32_t msg_type;      // DSP_CMD_SET, DSP_CMD_GET, DSP_CMD_RESPONSE
    DSP_Settings_t settings;
} dsp_message_t;

#endif // STRUCTURES_H
