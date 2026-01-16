#ifndef UDPHANDLER_H
#define UDPHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>
#include <vector>
#include <memory>
#include "structures.h"

class UdpHandler : public QObject
{
    Q_OBJECT

public:
    explicit UdpHandler(QObject *parent = nullptr);
    ~UdpHandler();

    // Connection management
    bool connectToHost(const QString& host, int port);
    void disconnectFromHost();
    bool isConnected() const;
    
    // Configuration
    void setMaxDetections(int max) { maxDetections = max; }
    void setDetectionTimeout(int timeoutMs) { detectionTimeoutMs = timeoutMs; }
    void setRemoteHost(const QString& host, int port);
    
    // Data access
    std::vector<DetectionData> getRecentDetections() const;
    int getDetectionCount() const;
    
    // Send DSP settings to radar
    bool sendDSPSettings(const DSP_Settings_t& settings);
    
    // Statistics
    int getPacketsReceived() const { return packetsReceived; }
    int getPacketsDropped() const { return packetsDropped; }
    double getDataRate() const; // packets per second

signals:
    void connectionStatusChanged(bool connected);
    void newDetectionReceived(const DetectionData& detection);
    void detectionsUpdated();
    void errorOccurred(const QString& error);
    void statisticsUpdated(int packetsReceived, int packetsDropped, double dataRate);
    void dspSettingsSent(bool success);

private slots:
    void readPendingDatagrams();
    void cleanupOldDetections();
    void updateStatistics();

private:
    // Connection
    std::unique_ptr<QUdpSocket> udpSocket;
    QString currentHost;
    int currentPort;
    bool connected;
    
    // Remote destination for sending settings
    QString remoteHost;
    int remotePort;
    
    // Data storage
    mutable QMutex detectionsMutex;
    std::vector<DetectionData> detections;
    int maxDetections;
    int detectionTimeoutMs;
    
    // Statistics
    QTimer* cleanupTimer;
    QTimer* statisticsTimer;
    int packetsReceived;
    int packetsDropped;
    qint64 lastStatisticsUpdate;
    qint64 lastPacketTime;
    
    // Data parsing
    bool parseDetectionData(const QString& data);
    bool parseJsonData(const QJsonDocument& doc);
    bool parseCsvData(const QString& csvData);
    void addDetection(const DetectionData& detection);
    
    // Validation
    bool isValidDetection(const DetectionData& detection) const;
    
    // Helper functions
    void resetStatistics();
    void emitStatistics();
};

#endif // UDPHANDLER_H
