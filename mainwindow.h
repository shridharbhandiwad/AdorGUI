#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QGroupBox>
#include <QCheckBox>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QTableWidget>
#include <QMutex>
#include <memory>

#include "structures.h"
#include "customchart.h"
#include "targetlist.h"
#include "dialogs.h"
#include "udphandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // Menu actions
    void saveConfiguration();
    void loadConfiguration();
    void showUdpConfigDialog();
    void showOutputConfigDialog();
    void showAngleCorrectionDialog();
    void showAmplificationDialog();
    void showDSPSettingsDialog();
    
    // Control actions
    void toggleLiveStream();
    void toggleFreezeRun();
    void writeToEEPROM();
    
    // Parameter changes
    void onThresholdChanged(int value);
    void onAmplificationChanged(int value);
    void onChannelChanged(int index);
    void onLineFilterChanged();
    
    // UDP data handling
    void onUdpConnectionChanged(bool connected);
    void onNewDetectionReceived(const DetectionData& detection);
    void onUdpStatisticsUpdated(int received, int dropped, double rate);
    void onTargetSelected(const TargetDetection& target);
    void onChartDetectionClicked(const TargetDetection& target);
    void onTrackTableSelectionChanged();
    
    // Chart updates
    void updateChartsWithDetections();
    void updateStatus();
    
    // Zoom handling
    void onZoomChanged(double zoomLevel);
    
    // DSP Settings
    void onSendDSPSettings(const DSP_Settings_t& settings);
    void onDSPSettingsSent(bool success);
    void applyDSPSettingsFromUI();

private:
    // UI Setup
    void setupMenuBar();
    void setupUI();
    void setupStatusBar();
    void setupConnections();
    
    // Tab creation
    QWidget* createRawSignalTab();
    QWidget* createDetectionTab();
    QWidget* createOutputTab(int outputNumber);
    
    // Settings
    void loadSettings();
    void saveSettings();
    void applySettings();
    
    // Data processing
    void processDetection(const DetectionData& detection);
    void updateDetectionCharts();
    void updateTargetLists();
    
    // UI Components
    QTabWidget* mainTabs;
    QSplitter* mainSplitter;
    
    // Charts
    CustomChart* rawChart;
    CustomChart* fftChart;
    CustomChart* detectionChart;  // Main detection chart in detection tab
    std::vector<CustomChart*> outputCharts;
    
    // Controls
    QPushButton* liveStreamButton;
    QPushButton* freezeButton;
    QPushButton* eepromButton;
    
    // Configuration controls
    QSlider* thresholdSlider;
    QSlider* amplificationSlider;
    QLabel* thresholdLabel;
    QLabel* amplificationLabel;
    QComboBox* channelCombo;
    
    // Line filter checkboxes
    QCheckBox* filter50Hz;
    QCheckBox* filter100Hz;
    QCheckBox* filter150Hz;
    
    // DSP Control Line Edits (Radar Settings)
    // Detection threshold settings
    QLineEdit* dspDetectionThresholdEdit;
    QLineEdit* dspCfarThresholdEdit;
    
    // Range settings
    QLineEdit* dspRangeMinEdit;
    QLineEdit* dspRangeMaxEdit;
    
    // Speed settings
    QLineEdit* dspSpeedMinEdit;
    QLineEdit* dspSpeedMaxEdit;
    
    // FFT settings
    QLineEdit* dspFftSizeEdit;
    QLineEdit* dspFftWindowTypeEdit;
    QLineEdit* dspFftAveragingEdit;
    
    // Filter settings
    QLineEdit* dspFilterEnabledEdit;
    QLineEdit* dspMovingAvgEnabledEdit;
    QLineEdit* dspMovingAvgWindowEdit;
    
    // Line filter settings
    QLineEdit* dspLineFilter50HzEdit;
    QLineEdit* dspLineFilter100HzEdit;
    QLineEdit* dspLineFilter150HzEdit;
    
    // Amplification settings
    QLineEdit* dspAmplificationEdit;
    QLineEdit* dspAutoAmplificationEdit;
    QLineEdit* dspAutoAmpInnerThresholdEdit;
    QLineEdit* dspAutoAmpOuterThresholdEdit;
    
    // Target selection settings
    QLineEdit* dspTargetSelectionModeEdit;
    QLineEdit* dspMaxTargetsEdit;
    QLineEdit* dspDirectionFilterEdit;
    
    // Signal processing settings
    QLineEdit* dspNoiseFloorTrackingEdit;
    QLineEdit* dspClutterRemovalEdit;
    QLineEdit* dspDopplerCompensationEdit;
    
    // Azimuth settings
    QLineEdit* dspAzimuthOffsetEdit;
    QLineEdit* dspAzimuthMinEdit;
    QLineEdit* dspAzimuthMaxEdit;
    
    // Apply DSP Settings button
    QPushButton* applyDSPSettingsButton;
    
    // Zoom controls
    QLabel* zoomLevelLabel;
    
    // Track table for detection tab
    QTableWidget* trackTable;
    
    // Target lists
    std::vector<TargetListWidget*> targetLists;
    
    // Status bar components
    QLabel* connectionStatusLabel;
    QLabel* dataRateLabel;
    QLabel* targetCountLabel;
    
    // Dialogs
    std::unique_ptr<UdpConfigDialog> udpConfigDialog;
    std::unique_ptr<OutputConfigDialog> outputConfigDialog;
    std::unique_ptr<AngleCorrectionDialog> angleDialog;
    std::unique_ptr<AmplificationDialog> amplificationDialog;
    std::unique_ptr<DSPSettingsDialog> dspSettingsDialog;
    
    // Data management
    std::vector<DetectionData> recentDetections;
    mutable QMutex recentDetectionsMutex;
    QTimer* updateTimer;
    
    // State
    bool liveStreamActive;
    bool frozen;
    bool connected;
    
    // Configuration
    struct AppConfig {
        int threshold;
        int amplification;
        int channel;
        bool filter50Hz;
        bool filter100Hz;
        bool filter150Hz;
        bool autoAmplification;
        
        AppConfig() : threshold(0), amplification(20), channel(0), 
                     filter50Hz(false), filter100Hz(false), filter150Hz(false),
                     autoAmplification(false) {}
    } config;
    
    // Helper methods
    void updateConnectionStatus(bool connected);
    void updateDataRate(double rate);
    void updateTargetCount(int count);
    void updateTrackTable();
    void showDetectionInChart(const DetectionData& detection);
    void highlightTargetInChart(const TargetDetection& target);
    
    // DSP Settings UI helpers
    void setupDSPControlsUI(QVBoxLayout* parentLayout);
    DSP_Settings_t collectDSPSettingsFromUI() const;
    void populateDSPControlsWithDefaults();
    
    // Constants
    static constexpr int UPDATE_INTERVAL_MS = 100;
    static constexpr int MAX_RECENT_DETECTIONS = 1000;
};

#endif // MAINWINDOW_H