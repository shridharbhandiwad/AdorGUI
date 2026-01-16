#include "udphandler.h"
#include <QNetworkDatagram>
#include <QHostAddress>
#include <algorithm>

UdpHandler::UdpHandler(QObject *parent)
    : QObject(parent)
    , udpSocket(nullptr)
    , currentPort(0)
    , connected(false)
    , remoteHost("127.0.0.1")
    , remotePort(5001)
    , maxDetections(1000)
    , detectionTimeoutMs(60000) // 60 seconds
    , packetsReceived(0)
    , packetsDropped(0)
    , lastStatisticsUpdate(0)
    , lastPacketTime(0)
{
    // Setup cleanup timer to remove old detections
    cleanupTimer = new QTimer(this);
    connect(cleanupTimer, &QTimer::timeout, this, &UdpHandler::cleanupOldDetections);
    cleanupTimer->start(5000); // Clean up every 5 seconds

    // Setup statistics timer
    statisticsTimer = new QTimer(this);
    connect(statisticsTimer, &QTimer::timeout, this, &UdpHandler::updateStatistics);
    statisticsTimer->start(1000); // Update statistics every second

    resetStatistics();
}

UdpHandler::~UdpHandler()
{
    disconnectFromHost();
}

bool UdpHandler::connectToHost(const QString& host, int port)
{
    // Disconnect existing connection
    disconnectFromHost();

    // Create new UDP socket
    udpSocket = std::make_unique<QUdpSocket>(this);

    // Try to bind to the specified port
    QHostAddress bindAddress = QHostAddress::Any;
    if (host != "0.0.0.0" && host != "127.0.0.1") {
        bindAddress = QHostAddress(host);
    }

    if (!udpSocket->bind(bindAddress, port)) {
        QString error = QString("Failed to bind to %1:%2 - %3")
                        .arg(host)
                        .arg(port)
                        .arg(udpSocket->errorString());
        emit errorOccurred(error);
        udpSocket.reset();
        return false;
    }

    // Connect signals
    connect(udpSocket.get(), &QUdpSocket::readyRead, this, &UdpHandler::readPendingDatagrams);
    connect(udpSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [this](QAbstractSocket::SocketError error) {
                Q_UNUSED(error);
                emit errorOccurred(QString("UDP Socket Error: %1").arg(udpSocket->errorString()));
            });

    // Store connection info
    currentHost = host;
    currentPort = port;
    connected = true;

    // Reset statistics
    resetStatistics();

    //qDebug() << "UDP Handler connected to" << host << ":" << port;
    emit connectionStatusChanged(true);

    return true;
}

void UdpHandler::disconnectFromHost()
{
    if (udpSocket) {
        udpSocket->close();
        udpSocket.reset();
    }

    connected = false;
    currentHost.clear();
    currentPort = 0;

    emit connectionStatusChanged(false);
}

bool UdpHandler::isConnected() const
{
    return connected && udpSocket && udpSocket->state() == QAbstractSocket::BoundState;
}

std::vector<DetectionData> UdpHandler::getRecentDetections() const
{
    QMutexLocker locker(&detectionsMutex);
    return detections;
}

int UdpHandler::getDetectionCount() const
{
    QMutexLocker locker(&detectionsMutex);
    return detections.size();
}

double UdpHandler::getDataRate() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeDiff = currentTime - lastStatisticsUpdate;

    if (timeDiff > 0) {
        return (packetsReceived * 1000.0) / timeDiff; // packets per second
    }

    return 0.0;
}

void UdpHandler::readPendingDatagrams()
{
    if (!udpSocket || !connected) {
        return;
    }

    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();

        if (datagram.isValid()) {
            QByteArray data = datagram.data();

            if (parseDetectionData(data)) {
                //qDebug()<<packetsReceived<<"\n";
                packetsReceived++;
                lastPacketTime = QDateTime::currentMSecsSinceEpoch();
            } else {
                packetsDropped++;
                //qDebug() << "Failed to parse UDP packet from" << datagram.senderAddress().toString();
            }
        }
    }

    emit detectionsUpdated();
}

bool UdpHandler::parseDetectionData(const QString& data)
{
    if (data.isEmpty()) {
        return false;
    }

//    try {
//        // Try to parse as JSON first
//        QJsonParseError error;
//        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

//        if (error.error == QJsonParseError::NoError && doc.isObject()) {
//            return parseJsonData(doc);
//        }

//        // Try to parse as CSV
//        QString csvData = QString::fromUtf8(data).trimmed();
//        if (!csvData.isEmpty()) {
//            return parseCsvData(csvData);
//        }

//    } catch (const std::exception& e) {
//        //qDebug() << "Exception parsing detection data:" << e.what();
//    } catch (...) {
//        //qDebug() << "Unknown exception parsing detection data";
//    }

//    return false;

    QStringList lines = data.split("\n", QString::SkipEmptyParts);

    //QVector<DetectionData> targets;
    DetectionData targets;
    //qDebug()<<"lines "<<lines;
    for (const QString &line : lines) {
        //if (!line.contains("NumTargets:")) continue;

        QStringList parts = line.split(" ", QString::SkipEmptyParts);
        //qDebug()<<"parts "<<parts.size();
        TargetDetection target;
        for (int i = 0; i < parts.size(); ++i) {
            if (parts[i] == "TgtId:" && i + 1 < parts.size())
                targets.target_id = parts[i + 1].toInt();
            if (parts[i] == "Range:" && i + 1 < parts.size())
                targets.radius = parts[i + 1].toFloat();
            if (parts[i] == "Speed:" && i + 1 < parts.size())
                targets.radial_speed = parts[i + 1].toFloat();
            if (parts[i] == "azimuth:" && i + 1 < parts.size())
                targets.azimuth = parts[i + 1].toFloat();
            if (parts[i] == "amplitude:" && i + 1 < parts.size())
                targets.amplitude = parts[i + 1].toFloat();
            if (parts[i] == "timestamp:" && i + 1 < parts.size())
                targets.timestamp = parts[i + 1].toInt();
        }
       // targets.append(target);
        addDetection(targets);
    }
    return true;
}

bool UdpHandler::parseJsonData(const QJsonDocument& doc)
{
    QJsonObject obj = doc.object();
    bool parsedAny = false;

    // Handle single detection
    if (obj.contains("target_id") && obj.contains("radius") &&
        obj.contains("radial_speed") && obj.contains("azimuth") &&
        obj.contains("amplitude") && obj.contains("timestamp")) {

        DetectionData detection;
        detection.target_id = obj["target_id"].toInt();
        detection.radius = obj["radius"].toDouble();
        detection.radial_speed = obj["radial_speed"].toDouble();
        detection.azimuth = obj["azimuth"].toDouble();
        QJsonValue amplitudeValue = obj.value("amplitude");
        detection.amplitude = amplitudeValue.isUndefined() ? 0.0 : amplitudeValue.toDouble();
        detection.timestamp = QDateTime::currentMSecsSinceEpoch();

        if (isValidDetection(detection)) {
            addDetection(detection);
            parsedAny = true;
        }
    }
    // Handle array of detections
    else if (obj.contains("detections") && obj["detections"].isArray()) {
        QJsonArray detArray = obj["detections"].toArray();
        for (const auto& detValue : detArray) {
            if (detValue.isObject()) {
                QJsonObject detObj = detValue.toObject();
                DetectionData detection;
                detection.target_id = detObj["target_id"].toInt();
                detection.radius = detObj["radius"].toDouble();
                detection.radial_speed = detObj["radial_speed"].toDouble();
                detection.azimuth = detObj["azimuth"].toDouble();
                //QJsonValue amplitudeValue = detObj.value("amplitude");
                //detection.amplitude = amplitudeValue.isUndefined() ? 0.0 : amplitudeValue.toDouble();
                //detection.timestamp = QDateTime::currentMSecsSinceEpoch();

                if (isValidDetection(detection)) {
                    addDetection(detection);
                    parsedAny = true;
                }
            }
        }
    }

    return parsedAny;
}

bool UdpHandler::parseCsvData(const QString& csvData)
{
    QStringList lines = csvData.split('\n');
    bool parsedAny = false;

    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) continue;

        QStringList parts = trimmedLine.split(',');
        if (parts.size() >= 4) {
            DetectionData detection;
            bool ok = true;

            detection.target_id = parts[0].toInt(&ok);
            if (!ok) continue;

            detection.radius = parts[1].toDouble(&ok);
            if (!ok) continue;

            detection.radial_speed = parts[2].toDouble(&ok);
            if (!ok) continue;

            detection.azimuth = parts[3].toDouble(&ok);
            if (!ok) continue;

            //detection.amplitude = parts.size() > 4 ? parts[4].toDouble() : 0.0;
            //detection.timestamp = QDateTime::currentMSecsSinceEpoch();
            //qDebug()<<"before adding";
            if (isValidDetection(detection)) {
                //qDebug()<<"adding";
                addDetection(detection);
                parsedAny = true;
            }
        }
    }

    return parsedAny;
}

void UdpHandler::addDetection(const DetectionData& detection)
{
    //qDebug()<<"Added";
    QMutexLocker locker(&detectionsMutex);
    qDebug()<<detection.radius<<"\n";

    detections.push_back(detection);

    // Emit signal for new detection
    emit newDetectionReceived(detection);

    // Limit number of stored detections
    if (static_cast<int>(detections.size()) > maxDetections) {
        detections.erase(detections.begin(), detections.begin() + (detections.size() - maxDetections));
    }
}

bool UdpHandler::isValidDetection(const DetectionData& detection) const
{
    // Basic validation
    if (detection.target_id < 0 || detection.target_id > 999) {
        return false;
    }

    if (detection.radius < 0 || detection.radius > 1000) { // 1km max range
        return false;
    }

    if (detection.azimuth < -180 || detection.azimuth > 180) {
        return false;
    }

    if (std::abs(detection.radial_speed) > 200) { // 200 m/s max speed
        return false;
    }

    return true;
}

void UdpHandler::cleanupOldDetections()
{
    QMutexLocker locker(&detectionsMutex);

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 cutoffTime = currentTime - detectionTimeoutMs;

    // Remove old detections
    detections.erase(
        std::remove_if(detections.begin(), detections.end(),
            [cutoffTime](const DetectionData& d) {
                return d.timestamp < cutoffTime;
            }),
        detections.end());
}

void UdpHandler::updateStatistics()
{
    emitStatistics();
}

void UdpHandler::resetStatistics()
{
    packetsReceived = 0;
    packetsDropped = 0;
    lastStatisticsUpdate = QDateTime::currentMSecsSinceEpoch();
    lastPacketTime = 0;
}

void UdpHandler::emitStatistics()
{
    emit statisticsUpdated(packetsReceived, packetsDropped, getDataRate());
}

void UdpHandler::setRemoteHost(const QString& host, int port)
{
    remoteHost = host;
    remotePort = port;
    qDebug() << "Remote host set to" << host << ":" << port;
}

bool UdpHandler::sendDSPSettings(const DSP_Settings_t& settings)
{
    if (!udpSocket || !connected) {
        emit errorOccurred("Cannot send DSP settings: Not connected");
        emit dspSettingsSent(false);
        return false;
    }
    
    // Create a copy and update checksum
    DSP_Settings_t settingsToSend = settings;
    settingsToSend.updateChecksum();
    
    // Create packet with header
    QByteArray packet;
    
    // Add packet header (identifier for DSP settings packet)
    const char* header = "DSPS"; // DSP Settings header identifier
    packet.append(header, 4);
    
    // Add packet version
    uint8_t version = 1;
    packet.append(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Add packet length
    uint16_t dataLength = sizeof(DSP_Settings_t);
    packet.append(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
    
    // Reserved byte for alignment
    uint8_t reserved = 0;
    packet.append(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
    
    // Add the DSP settings data
    packet.append(reinterpret_cast<const char*>(&settingsToSend), sizeof(DSP_Settings_t));
    
    // Send the packet
    QHostAddress destAddress(remoteHost);
    qint64 bytesSent = udpSocket->writeDatagram(packet, destAddress, remotePort);
    
    if (bytesSent == -1) {
        QString error = QString("Failed to send DSP settings: %1").arg(udpSocket->errorString());
        emit errorOccurred(error);
        emit dspSettingsSent(false);
        return false;
    }
    
    qDebug() << "DSP Settings sent:" << bytesSent << "bytes to" << remoteHost << ":" << remotePort;
    emit dspSettingsSent(true);
    return true;
}
