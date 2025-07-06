#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QtCore/QDateTime>

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
