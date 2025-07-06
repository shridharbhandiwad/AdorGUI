#ifndef ISYS4001_GUI_H
#define ISYS4001_GUI_H

#include <QtWidgets>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QSplitter>
#include <QTextEdit>
#include <QMenuBar>
#include <QStatusBar>
#include <QDialog>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QAbstractSocket>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QJsonParseError>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

// Forward declarations
class CustomChart;
class UdpConfigDialog;
class OutputConfigDialog;
class TargetListWidget;
class AngleCorrectionDialog;
class AmplificationDialog;
class UdpHandler;
class iSys4001MainWindow;

// Configuration structure for sensor parameters
struct SensorConfig {
    int amplification = 20;         // dB
    int threshold = 0;              // dB
    int channel = 0;                // 0-3 for channels 1-4
    bool autoAmplification = false;
    int innerThreshold = 30;        // dB
    int outerThreshold = 70;        // dB
    
    // Angle correction
    bool useAngleCorrection = false;
    bool useHeightCorrection = false;
    double mountingAngle = 0.0;     // degrees
    double mountingHeight = 3.0;    // meters
    
    // Output configurations
    struct OutputConfig {
        bool enabled = true;
        enum Type { DIGITAL, PWM_VELOCITY, PWM_RANGE } type = DIGITAL;
        enum PinConfig { 
            ACTIVE_LOW_NORMALLY_OPEN,
            ACTIVE_LOW_NORMALLY_CLOSED,
            ACTIVE_HIGH_NORMALLY_OPEN, 
            ACTIVE_HIGH_NORMALLY_CLOSED,
            TOTEM_POLE_NORMALLY_OPEN,
            TOTEM_POLE_NORMALLY_CLOSED
        } pinConfig = ACTIVE_LOW_NORMALLY_OPEN;
        
        int risingDelay = 375;      // ms
        int fallingDelay = 75;      // ms
        
        // Filter settings
        enum FilterType {
            INACTIVE,
            NONE,
            HIGHEST_AMPLITUDE,
            MEAN_RANGE,
            MEDIAN_RANGE, 
            MEAN_VELOCITY,
            MEDIAN_VELOCITY
        } filterType = HIGHEST_AMPLITUDE;
        
        // Target filter ranges
        int speedMin = 0, speedMax = 250;        // km/h
        int distanceMin = 0, distanceMax = 50;   // m
        int signalMin = 0, signalMax = 250;      // dB
        
        enum Direction { BOTH, APPROACHING, RECEDING } direction = BOTH;
        int singleTargetFilter = 20;             // %
        bool showHistogram = true;
    } outputs[3];
    
    // Line filters
    bool filter50Hz = false;
    bool filter100Hz = false; 
    bool filter150Hz = false;
};

// Data structures for sensor data
struct TargetData {
    double velocity;    // km/h (positive = approaching, negative = receding)
    double range;       // meters
    double amplitude;   // dB
    double snr;         // signal-to-noise ratio
    qint64 timestamp;   // ms since epoch
};

struct SensorData {
    std::vector<double> fftMagnitude;     // FFT magnitude data
    std::vector<double> fftFrequencies;   // Corresponding frequencies
    std::vector<double> thresholdData;    // Adaptive threshold
    std::vector<double> rawSignalI;       // In-phase raw signal
    std::vector<double> rawSignalQ;       // Quadrature raw signal
    std::vector<TargetData> targets;      // Detected targets
    qint64 timestamp;                     // Data timestamp
    bool valid;                           // Data validity flag
    
    SensorData() : valid(false) {}
};

// Configuration File Handler
class ConfigHandler {
public:
    static bool saveConfiguration(const QString& filename, const SensorConfig& config);
    static bool loadConfiguration(const QString& filename, SensorConfig& config);
    static SensorConfig getDefaultShortRangeConfig();
    static SensorConfig getDefaultLongRangeConfig();
    
private:
    static QJsonObject configToJson(const SensorConfig& config);
    static SensorConfig jsonToConfig(const QJsonObject& json);
};

// Global application settings
class AppSettings {
public:
    static AppSettings& instance();
    
    // Window settings
    QByteArray mainWindowGeometry;
    QByteArray mainWindowState;
    int lastTabIndex;
    
    // Connection settings
    QString lastHost;
    int lastPort;
    bool autoConnect;
    
    // Display settings
    bool darkTheme;
    bool showGrid;
    bool showLegend;
    int updateInterval;
    
    // Chart settings
    bool antialiasing;
    int chartBufferSize;
    double zoomSensitivity;
    
    void load();
    void save();
    
private:
    AppSettings() = default;
    static AppSettings* m_instance;
};

// Constants
namespace Constants {
    const int DEFAULT_UDP_PORT = 4001;
    const int DATA_REQUEST_INTERVAL = 100; // ms
    const int CHART_UPDATE_INTERVAL = 50; // ms
    const int CHART_MIN_WIDTH = 400;
    const int CHART_MIN_HEIGHT = 300;
    const int TARGET_LIST_MIN_WIDTH = 200;
    
    // Colors (RGB)
    const int FFT_COLOR[3] = {0, 255, 255};        // Cyan
    const int THRESHOLD_COLOR[3] = {255, 255, 0};  // Yellow
    const int GRID_COLOR[3] = {64, 64, 64};        // Dark gray
    const int APPROACHING_COLOR[3] = {255, 0, 0};  // Red
    const int RECEDING_COLOR[3] = {0, 255, 0};     // Green
    const int NO_OBJECT_COLOR[3] = {255, 165, 0};  // Orange
}

// Utility functions
namespace Utils {
    double calculateCosineCorrection(double angle);
    double calculateHeightCorrection(double height, double range);
    
    std::vector<double> applyLineFilter(const std::vector<double>& data,
                                       const std::vector<double>& frequencies,
                                       double filterFreq, double bandwidth = 2.0);
    
    std::vector<TargetData> filterTargets(const std::vector<TargetData>& targets,
                                         const SensorConfig::OutputConfig& config);
    
    TargetData selectTarget(const std::vector<TargetData>& targets,
                           SensorConfig::OutputConfig::FilterType filterType);
    
    // File and path utilities
    QString getApplicationDataPath();
    QString getConfigurationPath();
    QString getLogPath();
    
    // Validation utilities
    bool isValidHost(const QString& hostAddress);
    bool isValidPort(int port);
    bool isValidFrequency(double frequency);
    
    // Formatting utilities
    QString formatVelocity(double velocity);
    QString formatDistance(double distance);
    QString formatAmplitude(double amplitude);
    QString formatFrequency(double frequency);
    
    // Color utilities
    QColor getVelocityColor(double velocity);
    QColor getAmplitudeColor(double amplitude);
    QGradient createSpectrumGradient();
}

#endif // ISYS4001_GUI_H
