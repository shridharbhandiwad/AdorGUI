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
    
    // Chart updates
    void updateChartsWithDetections();
    void updateStatus();

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
    
    // Data management
    std::vector<DetectionData> recentDetections;
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
    void showDetectionInChart(const DetectionData& detection);
    void highlightTargetInChart(const TargetDetection& target);
    
    // Constants
    static constexpr int UPDATE_INTERVAL_MS = 100;
    static constexpr int MAX_RECENT_DETECTIONS = 1000;
};

#endif // MAINWINDOW_H