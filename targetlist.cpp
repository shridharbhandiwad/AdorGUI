#include "targetlist.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>
#include <QBrush>
#include <cmath>

// TargetListWidget Implementation
TargetListWidget::TargetListWidget(QWidget* parent)
    : QWidget(parent)
    , maxTargets(100)
    , autoScroll(true)
    , showTimestamp(true)
    , compactView(false)
    , totalTargetsReceived(0)
    , lastUpdateTime(0)
{
    setMinimumSize(250, 300);
    setupUI();

    // Setup timers
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &TargetListWidget::updateStatus);
    updateTimer->start(1000); // Update every second

    cleanupTimer = new QTimer(this);
    connect(cleanupTimer, &QTimer::timeout, this, &TargetListWidget::cleanupOldTargets);
    cleanupTimer->start(10000); // Cleanup every 10 seconds

    updateStatus();
}

TargetListWidget::~TargetListWidget() = default;

void TargetListWidget::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Status indicator
    statusButton = new QPushButton("No Object Detected");
    statusButton->setStyleSheet("QPushButton { background-color: #FFE4B5; color: #8B4513; font-weight: bold; border: 1px solid #D2691E; }");  // Moccasin background
    statusButton->setEnabled(false);
    layout->addWidget(statusButton);

    // Count and statistics labels
    countLabel = new QLabel("Targets: 0");
    countLabel->setStyleSheet("QLabel { font-weight: bold; color: #191970; }");  // Midnight blue
    layout->addWidget(countLabel);

    statisticsLabel = new QLabel("Total received: 0");
    statisticsLabel->setStyleSheet("QLabel { color: #708090; font-size: 10px; }");  // Slate gray
    layout->addWidget(statisticsLabel);

    // Target list
    QLabel* listLabel = new QLabel("Detected Targets:");
    layout->addWidget(listLabel);

    targetListWidget = new QListWidget();
    targetListWidget->setAlternatingRowColors(true);
    targetListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(targetListWidget);

    connect(targetListWidget, &QListWidget::itemClicked, this, &TargetListWidget::onTargetItemClicked);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    clearButton = new QPushButton("Clear");
    exportButton = new QPushButton("Export");

    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(exportButton);
    layout->addLayout(buttonLayout);

    connect(clearButton, &QPushButton::clicked, this, &TargetListWidget::clearTargets);
    connect(exportButton, &QPushButton::clicked, this, &TargetListWidget::exportTargets);

    updateCountLabel();
    updateStatisticsLabel();
}

void TargetListWidget::addTarget(const TargetDetection& detection)
{
    QMutexLocker locker(&targetsMutex);
    addTargetInternal(detection);
    updateDisplay();
}

void TargetListWidget::setTargets(const std::vector<TargetDetection>& newTargets)
{
    QMutexLocker locker(&targetsMutex);
    targets = newTargets;

    // Limit size
    if (targets.size() > maxTargets) {
        targets.erase(targets.begin(), targets.begin() + (targets.size() - maxTargets));
    }

    updateDisplay();
}

void TargetListWidget::clearTargets()
{
    QMutexLocker locker(&targetsMutex);
    targets.clear();
    totalTargetsReceived = 0;
    updateDisplay();
}

void TargetListWidget::setMaxTargets(int maxTargets)
{
    this->maxTargets = maxTargets;

    QMutexLocker locker(&targetsMutex);
    if (targets.size() > maxTargets) {
        targets.erase(targets.begin(), targets.begin() + (targets.size() - maxTargets));
        updateDisplay();
    }
}

void TargetListWidget::setAutoScroll(bool enabled)
{
    autoScroll = enabled;
}

void TargetListWidget::setShowTimestamp(bool show)
{
    showTimestamp = show;
    updateDisplay();
}

void TargetListWidget::setCompactView(bool compact)
{
    compactView = compact;
    updateDisplay();
}

int TargetListWidget::getTargetCount() const
{
    QMutexLocker locker(&targetsMutex);
    return targets.size();
}

TargetDetection TargetListWidget::getLatestTarget() const
{
    QMutexLocker locker(&targetsMutex);
    if (targets.empty()) {
        return TargetDetection();
    }
    return targets.back();
}

std::vector<TargetDetection> TargetListWidget::getTargets() const
{
    QMutexLocker locker(&targetsMutex);
    return targets;
}

void TargetListWidget::onNewDetection(const DetectionData& detection)
{
    // Convert DetectionData to TargetDetection
    TargetDetection target = detection.toTargetDetection();
    addTarget(target);
}

void TargetListWidget::refreshDisplay()
{
    updateDisplay();
}

void TargetListWidget::onTargetItemClicked(QListWidgetItem* item)
{
    TargetListItem* targetItem = dynamic_cast<TargetListItem*>(item);
    if (targetItem) {
        emit targetSelected(targetItem->getTarget());
    }
}

void TargetListWidget::updateStatus()
{
    updateStatusButton();
    updateCountLabel();
    updateStatisticsLabel();
}

void TargetListWidget::cleanupOldTargets()
{
    QMutexLocker locker(&targetsMutex);
    removeOldTargets();
    updateDisplay();
}

void TargetListWidget::updateDisplay()
{
    // Update list widget
    targetListWidget->clear();

    for (const auto& target : targets) {
        TargetListItem* item = new TargetListItem(target, targetListWidget);

        // Color code based on speed
        QColor color = Qt::white;
        if (target.radial_speed > 2.0) {
            color = QColor(255, 200, 200); // Light red for approaching
        } else if (target.radial_speed < -2.0) {
            color = QColor(200, 255, 200); // Light green for receding
        } else {
            color = QColor(255, 255, 200); // Light yellow for stationary
        }

        item->setBackground(QBrush(color));
        targetListWidget->addItem(item);
    }

    // Auto-scroll to bottom
    if (autoScroll && !targets.empty()) {
        scrollToBottom();
    }

    // Update UI elements
    updateStatusButton();
    updateCountLabel();
    updateStatisticsLabel();

    // Emit signals
    emit targetCountChanged(targets.size());
    emit statusChanged(getStatusText());
}

void TargetListWidget::updateStatusButton()
{
    if (targets.empty()) {
        statusButton->setText("No Object Detected");
        statusButton->setStyleSheet("QPushButton { background-color: #FFE4B5; color: #8B4513; font-weight: bold; border: 1px solid #D2691E; }");  // Moccasin background
    } else {
        const auto& latest = targets.back();

        if (latest.radial_speed > 2.0) {
            statusButton->setText("Object Approaching");
            statusButton->setStyleSheet("QPushButton { background-color: #FFB6C1; color: #8B0000; font-weight: bold; border: 1px solid #DC143C; }");  // Light pink with dark red text
        } else if (latest.radial_speed < -2.0) {
            statusButton->setText("Object Receding");
            statusButton->setStyleSheet("QPushButton { background-color: #98FB98; color: #006400; font-weight: bold; border: 1px solid #228B22; }");  // Pale green with dark green text
        } else {
            statusButton->setText("Object Detected");
            statusButton->setStyleSheet("QPushButton { background-color: #FFFFE0; color: #B8860B; font-weight: bold; border: 1px solid #DAA520; }");  // Light yellow with dark goldenrod text
        }
    }
}

QString TargetListWidget::formatTarget(const TargetDetection& target) const
{
    QString text;

    if (compactView) {
        text = QString("ID:%1 R:%2m S:%3m/s A:%4°")
               .arg(target.target_id)
               .arg(target.radius, 0, 'f', 1)
               .arg(target.radial_speed, 0, 'f', 1)
               .arg(target.azimuth, 0, 'f', 0);
    } else {
        text = QString("Target %1\nRange: %2 m\nSpeed: %3 m/s\nAzimuth: %4°")
               .arg(target.target_id)
               .arg(target.radius, 0, 'f', 2)
               .arg(target.radial_speed, 0, 'f', 2)
               .arg(target.azimuth, 0, 'f', 1);

        if (target.amplitude > 0) {
            text += QString("\nAmplitude: %1 dB").arg(target.amplitude, 0, 'f', 1);
        }
    }

    if (showTimestamp) {
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(target.timestamp);
        text += QString("\nTime: %1").arg(timestamp.toString("hh:mm:ss.zzz"));
    }

    return text;
}

QString TargetListWidget::getStatusText() const
{
    if (targets.empty()) {
        return "No targets detected";
    }

    const auto& latest = targets.back();

    if (latest.radial_speed > 2.0) {
        return "Target approaching";
    } else if (latest.radial_speed < -2.0) {
        return "Target receding";
    } else {
        return "Target detected";
    }
}

QColor TargetListWidget::getStatusColor() const
{
    if (targets.empty()) {
        return Qt::gray;
    }

    const auto& latest = targets.back();

    if (latest.radial_speed > 2.0) {
        return Qt::red;
    } else if (latest.radial_speed < -2.0) {
        return Qt::green;
    } else {
        return Qt::yellow;
    }
}

void TargetListWidget::addTargetInternal(const TargetDetection& target)
{
    targets.push_back(target);
    totalTargetsReceived++;
    lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

    // Keep only recent targets
    if (targets.size() > maxTargets) {
        targets.erase(targets.begin());
    }
}

void TargetListWidget::removeOldTargets()
{
    // Remove targets older than 60 seconds
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 cutoffTime = currentTime - 60000; // 60 seconds

    targets.erase(
        std::remove_if(targets.begin(), targets.end(),
            [cutoffTime](const TargetDetection& target) {
                return target.timestamp < cutoffTime;
            }),
        targets.end());
}

void TargetListWidget::exportTargets()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export Targets",
        QString("targets_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "CSV Files (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Error", "Could not open file for writing.");
        return;
    }

    QTextStream out(&file);

    // Write header
    out << "Timestamp,Target_ID,Range_m,Radial_Speed_ms,Azimuth_deg,Amplitude_dB\n";

    // Write data
    QMutexLocker locker(&targetsMutex);
    for (const auto& target : targets) {
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(target.timestamp);
        out << timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << ","
            << target.target_id << ","
            << target.radius << ","
            << target.radial_speed << ","
            << target.azimuth << ","
            << target.amplitude << "\n";
    }

    file.close();

    QMessageBox::information(this, "Export Complete",
        QString("Successfully exported %1 targets to %2").arg(targets.size()).arg(fileName));
}

void TargetListWidget::scrollToBottom()
{
    targetListWidget->scrollToBottom();
}

void TargetListWidget::updateCountLabel()
{
    countLabel->setText(QString("Targets: %1").arg(targets.size()));
}

void TargetListWidget::updateStatisticsLabel()
{
    statisticsLabel->setText(QString("Total received: %1").arg(totalTargetsReceived));
}

// TargetListItem Implementation
TargetListItem::TargetListItem(const TargetDetection& target, QListWidget* parent)
    : QListWidgetItem(parent)
    , targetData(target)
{
    updateText();
}

void TargetListItem::updateTarget(const TargetDetection& target)
{
    targetData = target;
    updateText();
}

void TargetListItem::updateText()
{
    QString text = QString("ID:%1 R:%2m S:%3m/s A:%4°")
                   .arg(targetData.target_id)
                   .arg(targetData.radius, 0, 'f', 1)
                   .arg(targetData.radial_speed, 0, 'f', 1)
                   .arg(targetData.azimuth, 0, 'f', 0);

    if (targetData.amplitude > 0) {
        text += QString(" Amp:%1dB").arg(targetData.amplitude, 0, 'f', 0);
    }

    setText(text);

    // Set tooltip with detailed information
    QString tooltip = QString("Target ID: %1\nRange: %2 m\nRadial Speed: %3 m/s\nAzimuth: %4°\nAmplitude: %5 dB\nTime: %6")
                      .arg(targetData.target_id)
                      .arg(targetData.radius, 0, 'f', 2)
                      .arg(targetData.radial_speed, 0, 'f', 2)
                      .arg(targetData.azimuth, 0, 'f', 1)
                      .arg(targetData.amplitude, 0, 'f', 1)
                      .arg(QDateTime::fromMSecsSinceEpoch(targetData.timestamp).toString("hh:mm:ss.zzz"));

    setToolTip(tooltip);
}
