#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QFont>
#include <QSlider>
#include <memory>
#include "structures.h"
#include "udphandler.h"

// Forward declarations
class UdpHandler;

// UDP Configuration Dialog
class UdpConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UdpConfigDialog(QWidget* parent = nullptr);
    ~UdpConfigDialog();

    UdpHandler* getUdpHandler() const { return udpHandler.get(); }
    bool isConnected() const;

signals:
    void connectionStatusChanged(bool connected);
    void dataReceived(const DetectionData& data);

private slots:
    void connectToHost();
    void disconnectFromHost();
    void onConnectionStatusChanged(bool connected);
    void onNewDetectionReceived(const DetectionData& detection);
    void onErrorOccurred(const QString& error);
    void onStatisticsUpdated(int received, int dropped, double rate);

private:
    QLineEdit* hostEdit;
    QSpinBox* portSpinBox;
    QLabel* statusLabel;
    QPushButton* connectButton;
    QPushButton* disconnectButton;
    QPushButton* closeButton;
    QLabel* statisticsLabel;
    
    std::unique_ptr<UdpHandler> udpHandler;
    
    void setupUI();
    void updateUI();
};

// Output Configuration Dialog
class OutputConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OutputConfigDialog(QWidget* parent = nullptr);

    struct OutputSettings {
        enum OutputType {
            DIGITAL,
            PWM_VELOCITY,
            PWM_RANGE
        };
        
        enum PinConfiguration {
            ACTIVE_LOW_NO,
            ACTIVE_LOW_NC,
            ACTIVE_HIGH_NO,
            ACTIVE_HIGH_NC,
            TOTEM_POLE_NO,
            TOTEM_POLE_NC
        };
        
        OutputType type;
        PinConfiguration pinConfig;
        int risingDelay;
        int fallingDelay;
        
        OutputSettings() : type(DIGITAL), pinConfig(ACTIVE_LOW_NO), risingDelay(375), fallingDelay(75) {}
    };
    
    OutputSettings getOutputSettings(int outputNumber) const;
    void setOutputSettings(int outputNumber, const OutputSettings& settings);

private:
    QTabWidget* tabWidget;
    std::vector<QWidget*> outputTabs;
    
    void setupUI();
    QWidget* createOutputTab(int outputNum);
    void saveSettings();
    void loadSettings();
};

// Angle Correction Dialog
class AngleCorrectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AngleCorrectionDialog(QWidget* parent = nullptr);

    struct AngleCorrectionSettings {
        enum CorrectionMethod {
            MOUNTING_ANGLE,
            MOUNTING_HEIGHT
        };
        
        CorrectionMethod method;
        double mountingAngle;    // degrees
        double mountingHeight;   // meters
        
        AngleCorrectionSettings() : method(MOUNTING_ANGLE), mountingAngle(0.0), mountingHeight(3.0) {}
    };
    
    AngleCorrectionSettings getSettings() const;
    void setSettings(const AngleCorrectionSettings& settings);

private:
    QRadioButton* angleRadio;
    QRadioButton* heightRadio;
    QSpinBox* angleSpinBox;
    QDoubleSpinBox* heightSpinBox;
    
    void setupUI();
    void updateControls();
};

// Amplification Settings Dialog
class AmplificationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AmplificationDialog(QWidget* parent = nullptr);

    struct AmplificationSettings {
        int manualAmplification;     // dB
        bool automaticEnabled;
        int innerThreshold;          // dB
        int outerThreshold;          // dB
        
        AmplificationSettings() : manualAmplification(20), automaticEnabled(false), 
                                innerThreshold(30), outerThreshold(70) {}
    };
    
    AmplificationSettings getSettings() const;
    void setSettings(const AmplificationSettings& settings);

signals:
    void amplificationChanged(int amplification);
    void settingsStored();

private slots:
    void onAmplificationChanged(int value);
    void onAutoEnabledChanged(bool enabled);
    void storeSettings();

private:
    QSlider* amplificationSlider;
    QLabel* amplificationLabel;
    QCheckBox* autoEnabledCheckBox;
    QSpinBox* innerThresholdSpinBox;
    QSpinBox* outerThresholdSpinBox;
    QPushButton* storeButton;
    
    void setupUI();
    void updateControls();
};

// Filter Configuration Dialog
class FilterConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterConfigDialog(QWidget* parent = nullptr);

    struct FilterSettings {
        enum FilterType {
            INACTIVE,
            NONE,
            HIGHEST_AMPLITUDE,
            MEAN_RANGE,
            MEDIAN_RANGE,
            MEAN_VELOCITY,
            MEDIAN_VELOCITY
        };
        
        enum Direction {
            BOTH,
            APPROACHING_ONLY,
            RECEDING_ONLY
        };
        
        FilterType type;
        int speedMin;           // km/h
        int speedMax;           // km/h
        int distanceMin;        // m
        int distanceMax;        // m
        int signalMin;          // dB
        int signalMax;          // dB
        Direction direction;
        int singleTargetFilter; // %
        bool showHistogram;
        
        FilterSettings() : type(HIGHEST_AMPLITUDE), speedMin(0), speedMax(250),
                          distanceMin(0), distanceMax(50), signalMin(0), signalMax(250),
                          direction(BOTH), singleTargetFilter(20), showHistogram(true) {}
    };
    
    FilterSettings getSettings() const;
    void setSettings(const FilterSettings& settings);

private:
    QComboBox* filterTypeCombo;
    QSpinBox* speedMinSpinBox;
    QSpinBox* speedMaxSpinBox;
    QSpinBox* distanceMinSpinBox;
    QSpinBox* distanceMaxSpinBox;
    QSpinBox* signalMinSpinBox;
    QSpinBox* signalMaxSpinBox;
    QComboBox* directionCombo;
    QSpinBox* singleTargetSpinBox;
    QCheckBox* showHistogramCheckBox;
    
    void setupUI();
};

// Forward declaration for DSP Settings
struct DSP_Settings_t;

// DSP Settings Dialog
class DSPSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DSPSettingsDialog(QWidget* parent = nullptr);
    ~DSPSettingsDialog();
    
    DSP_Settings_t getSettings() const;
    void setSettings(const DSP_Settings_t& settings);

signals:
    void settingsChanged(const DSP_Settings_t& settings);
    void sendSettingsRequested(const DSP_Settings_t& settings);

private slots:
    void onApplyClicked();
    void onSendClicked();
    void onLoadDefaultsClicked();
    void updateUIFromSettings();

private:
    // Detection threshold settings
    QSpinBox* detectionThresholdSpinBox;
    QSpinBox* cfarThresholdSpinBox;
    
    // Range settings
    QDoubleSpinBox* rangeMinSpinBox;
    QDoubleSpinBox* rangeMaxSpinBox;
    
    // Speed settings
    QDoubleSpinBox* speedMinSpinBox;
    QDoubleSpinBox* speedMaxSpinBox;
    
    // FFT settings
    QComboBox* fftSizeCombo;
    QComboBox* fftWindowTypeCombo;
    QSpinBox* fftAveragingSpinBox;
    
    // Filter settings
    QCheckBox* filterEnabledCheckBox;
    QCheckBox* movingAvgEnabledCheckBox;
    QSpinBox* movingAvgWindowSpinBox;
    
    // Line filter settings
    QCheckBox* lineFilter50HzCheckBox;
    QCheckBox* lineFilter100HzCheckBox;
    QCheckBox* lineFilter150HzCheckBox;
    
    // Amplification settings
    QSlider* amplificationSlider;
    QLabel* amplificationLabel;
    QCheckBox* autoAmplificationCheckBox;
    QSpinBox* autoAmpInnerThresholdSpinBox;
    QSpinBox* autoAmpOuterThresholdSpinBox;
    
    // Target selection settings
    QComboBox* targetSelectionModeCombo;
    QSpinBox* maxTargetsSpinBox;
    QComboBox* directionFilterCombo;
    
    // Signal processing settings
    QCheckBox* noiseFloorTrackingCheckBox;
    QCheckBox* clutterRemovalCheckBox;
    QCheckBox* dopplerCompensationCheckBox;
    
    // Azimuth settings
    QDoubleSpinBox* azimuthOffsetSpinBox;
    QDoubleSpinBox* azimuthMinSpinBox;
    QDoubleSpinBox* azimuthMaxSpinBox;
    
    // Buttons
    QPushButton* applyButton;
    QPushButton* sendButton;
    QPushButton* loadDefaultsButton;
    QPushButton* closeButton;
    
    // Tab widget for organized settings
    QTabWidget* tabWidget;
    
    void setupUI();
    QWidget* createDetectionTab();
    QWidget* createFFTTab();
    QWidget* createFilterTab();
    QWidget* createAmplificationTab();
    QWidget* createTargetTab();
    QWidget* createAzimuthTab();
    
    void connectSignals();
    void updateAmplificationControls();
};

#endif // DIALOGS_H