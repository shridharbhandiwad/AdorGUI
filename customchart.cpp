#include "customchart.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QPolygon>
#include <QFont>
#include <cmath>

CustomChart::CustomChart(ChartType type, QWidget* parent)
    : QWidget(parent)
    , chartType(type)
    , frozen(false)
    , threshold(0.0)
    , showLegend(true)
    , showGrid(true)
    , maxDataPoints(1024)
    , zoomLevel(1.0)
    , gen(rd())
    , dis(-1.0, 1.0)
    , fft_dis(0.0, 100.0)
{
    setMinimumSize(400, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    
    // Timer for live updates
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &CustomChart::updateData);
    updateTimer->start(1000); // Update every second
    
    calculatePlotArea();
}

CustomChart::~CustomChart() = default;

void CustomChart::setFrozen(bool freeze)
{
    frozen = freeze;
    if (frozen) {
        updateTimer->stop();
    } else {
        updateTimer->start(1000);
    }
}

void CustomChart::setThreshold(double newThreshold)
{
    threshold = newThreshold;
    update();
}

void CustomChart::setUpdateInterval(int intervalMs)
{
    if (updateTimer) {
        updateTimer->stop();
        updateTimer->start(intervalMs);
    }
}

int CustomChart::getUpdateInterval() const
{
    return updateTimer ? updateTimer->interval() : 1000;
}

void CustomChart::zoomIn()
{
    zoomLevel *= 1.5;
    if (zoomLevel > 10.0) {
        zoomLevel = 10.0;
    }
    update();
    emit zoomChanged(zoomLevel);
}

void CustomChart::zoomOut()
{
    zoomLevel /= 1.5;
    if (zoomLevel < 0.5) {
        zoomLevel = 0.5;
    }
    update();
    emit zoomChanged(zoomLevel);
}

void CustomChart::resetZoom()
{
    zoomLevel = 1.0;
    update();
    emit zoomChanged(zoomLevel);
}

void CustomChart::addDetection(const TargetDetection& detection)
{
    QMutexLocker locker(&dataMutex);
    detections.push_back(detection);
    
    // Keep only recent detections
    if (detections.size() > maxDataPoints) {
        detections.erase(detections.begin());
    }
    
    update();
}

void CustomChart::setDetections(const std::vector<TargetDetection>& newDetections)
{
    QMutexLocker locker(&dataMutex);
    detections = newDetections;
    
    // Limit size
    if (detections.size() > maxDataPoints) {
        detections.erase(detections.begin(), detections.begin() + (detections.size() - maxDataPoints));
    }
    
    update();
}

void CustomChart::clearDetections()
{
    QMutexLocker locker(&dataMutex);
    detections.clear();
    update();
}

void CustomChart::setFFTData(const std::vector<double>& data)
{
    QMutexLocker locker(&dataMutex);
    fftData = data;
    update();
}

void CustomChart::setRawSignalData(const std::vector<double>& data)
{
    QMutexLocker locker(&dataMutex);
    rawSignalData = data;
    update();
}

void CustomChart::setHistogramData(const std::vector<double>& data)
{
    QMutexLocker locker(&dataMutex);
    histogramData = data;
    update();
}

void CustomChart::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBackground(painter);
    
    switch (chartType) {
    case FFT_CHART:
        drawFFTChart(painter);
        break;
    case RAW_SIGNAL_CHART:
        drawRawSignalChart(painter);
        break;
    case DETECTION_CHART:
        drawDetectionChart(painter);
        break;
    case HISTOGRAM_CHART:
        drawHistogramChart(painter);
        break;
    }
    
    if (showLegend) {
        drawLegend(painter);
    }
}

void CustomChart::mousePressEvent(QMouseEvent* event)
{
    if (chartType == DETECTION_CHART) {
        TargetDetection detection = getDetectionAt(event->pos());
        if (detection.target_id != 0) {
            emit detectionClicked(detection);
        }
    }
    
    QWidget::mousePressEvent(event);
}

void CustomChart::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    calculatePlotArea();
}

void CustomChart::wheelEvent(QWheelEvent* event)
{
    if (chartType == DETECTION_CHART) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

void CustomChart::updateData()
{
    if (!frozen) {
        //generateSampleData();
        update();
        emit dataUpdated();
    }
}

void CustomChart::drawBackground(QPainter& painter)
{
    painter.fillRect(rect(), Qt::black);
}

void CustomChart::drawGrid(QPainter& painter)
{
    if (!showGrid) return;
    
    painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
    
    // Vertical grid lines
    for (int i = 0; i <= 10; ++i) {
        int x = plotArea.left() + i * plotArea.width() / 10;
        painter.drawLine(x, plotArea.top(), x, plotArea.bottom());
    }
    
    // Horizontal grid lines
    for (int i = 0; i <= 10; ++i) {
        int y = plotArea.bottom() - i * plotArea.height() / 10;
        painter.drawLine(plotArea.left(), y, plotArea.right(), y);
    }
}

void CustomChart::drawAxes(QPainter& painter)
{
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(plotArea.bottomLeft(), plotArea.bottomRight());
    painter.drawLine(plotArea.bottomLeft(), plotArea.topLeft());
}

void CustomChart::drawLegend(QPainter& painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    
    int legendY = plotArea.top() + 10;
    int legendX = plotArea.right() - 150;
    
    switch (chartType) {
    case FFT_CHART:
        painter.drawText(legendX, legendY, "FFT Signal");
        painter.setPen(Qt::green);
        painter.drawLine(legendX - 20, legendY - 5, legendX - 5, legendY - 5);
        
        painter.setPen(Qt::white);
        painter.drawText(legendX, legendY + 15, "Threshold");
        painter.setPen(Qt::yellow);
        painter.drawLine(legendX - 20, legendY + 10, legendX - 5, legendY + 10);
        break;
        
    case DETECTION_CHART:
        painter.setPen(Qt::white);
        painter.drawText(legendX, legendY, "Red: Approaching");
        painter.drawText(legendX, legendY + 15, "Green: Receding");
        painter.drawText(legendX, legendY + 30, "Yellow: Stationary");
        break;
        
    default:
        break;
    }
}

void CustomChart::calculatePlotArea()
{
    const int margin = 50;
    const int legendWidth = showLegend ? 160 : 30;
    const int bottomMargin = 50;
    
    plotArea = QRect(margin, 30, width() - margin - legendWidth, height() - 30 - bottomMargin);
    legendArea = QRect(plotArea.right() + 10, plotArea.top(), legendWidth - 20, plotArea.height());
}

void CustomChart::generateSampleData()
{
    // Generate FFT data (simulated)
    fftData.clear();
    thresholdData.clear();
    for (int i = 0; i < 512; ++i) {
        double freq = i * 250.0 / 512.0; // 0-250 Hz
        double amplitude = fft_dis(gen) * exp(-freq / 50.0); // Decreasing with frequency
        if (i > 50 && i < 150) amplitude += 20 * sin(i * 0.1); // Add some peaks
        fftData.push_back(amplitude);
        thresholdData.push_back(30 + threshold + 5 * sin(i * 0.05)); // Adaptive threshold
    }
    
    // Generate raw signal data
    rawSignalData.clear();
    for (int i = 0; i < 1024; ++i) {
        double t = i / 1024.0;
        double signal = 0.5 * sin(2 * M_PI * 10 * t) + 0.3 * sin(2 * M_PI * 50 * t) + 0.1 * dis(gen);
        rawSignalData.push_back(signal);
    }
    
    // Generate histogram data
    histogramData.clear();
    for (int i = 0; i < 100; ++i) {
        histogramData.push_back(fft_dis(gen) * 0.5);
    }
}

void CustomChart::drawFFTChart(QPainter& painter)
{
    QMutexLocker locker(&dataMutex);
    
    if (fftData.empty()) return;
    
    drawGrid(painter);
    drawAxes(painter);
    
    // Draw labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, plotArea.center().y(), "Amplitude (dB)");
    painter.drawText(plotArea.center().x() - 30, height() - 10, "Frequency (Hz)");
    
    // Draw threshold line
    painter.setPen(QPen(Qt::yellow, 2));
    if (!thresholdData.empty()) {
        QPolygon thresholdPoly;
        for (size_t i = 0; i < thresholdData.size(); ++i) {
            int x = plotArea.left() + i * plotArea.width() / (thresholdData.size() - 1);
            int y = plotArea.bottom() - (thresholdData[i] / 100.0) * plotArea.height();
            thresholdPoly << QPoint(x, y);
        }
        painter.drawPolyline(thresholdPoly);
    }
    
    // Draw FFT data
    painter.setPen(QPen(Qt::green, 2));
    QPolygon fftPoly;
    for (size_t i = 0; i < fftData.size(); ++i) {
        int x = plotArea.left() + i * plotArea.width() / (fftData.size() - 1);
        int y = plotArea.bottom() - (fftData[i] / 100.0) * plotArea.height();
        fftPoly << QPoint(x, y);
    }
    painter.drawPolyline(fftPoly);
}

void CustomChart::drawRawSignalChart(QPainter& painter)
{
    QMutexLocker locker(&dataMutex);
    
    if (rawSignalData.empty()) return;
    
    drawGrid(painter);
    drawAxes(painter);
    
    // Draw zero line
    int zeroY = plotArea.bottom() - plotArea.height() / 2;
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(plotArea.left(), zeroY, plotArea.right(), zeroY);
    
    // Draw raw signal data
    painter.setPen(QPen(Qt::cyan, 2));
    QPolygon signalPoly;
    for (size_t i = 0; i < rawSignalData.size(); ++i) {
        int x = plotArea.left() + i * plotArea.width() / (rawSignalData.size() - 1);
        int y = zeroY - rawSignalData[i] * plotArea.height() / 4;
        signalPoly << QPoint(x, y);
    }
    painter.drawPolyline(signalPoly);
    
    // Draw labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, plotArea.center().y(), "Amplitude");
    painter.drawText(plotArea.center().x() - 20, height() - 10, "Samples");
}

void CustomChart::drawDetectionChart(QPainter& painter)
{
    QMutexLocker locker(&dataMutex);
    
    drawGrid(painter);
    
    // Calculate semicircle parameters
    QPoint center = QPoint(plotArea.center().x(), plotArea.bottom() - 20);
    int maxRadius = (qMin(plotArea.width(), plotArea.height()) * zoomLevel) / 2 - 20;
    
    // Ensure semicircle fits within the plot area
    if (maxRadius > plotArea.height() - 40) {
        maxRadius = plotArea.height() - 40;
    }
    if (maxRadius > plotArea.width() / 2 - 40) {
        maxRadius = plotArea.width() / 2 - 40;
    }
    
    // Draw range circles (semicircles from -90 to +90 degrees)
    painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
    for (int i = 1; i <= 5; ++i) {
        int r = maxRadius * i / 5;
        painter.drawArc(center.x() - r, center.y() - r, 2 * r, 2 * r, 0, 180 * 16); // 180 degrees in 16th of a degree
    }
    
    // Draw azimuth lines from -90 to +90 degrees
    painter.setPen(QPen(Qt::darkGray, 1, Qt::DotLine));
    for (int angle = -90; angle <= 90; angle += 15) {
        double rad = angle * M_PI / 180.0;
        int x = center.x() + maxRadius * cos(rad);
        int y = center.y() - maxRadius * sin(rad);
        painter.drawLine(center, QPoint(x, y));
    }
    
    // Draw main axes (horizontal baseline and vertical 0-degree line)
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(center.x() - maxRadius, center.y(), center.x() + maxRadius, center.y()); // Horizontal baseline
    painter.drawLine(center.x(), center.y(), center.x(), center.y() - maxRadius); // Vertical 0-degree line
    
    // Draw range labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    for (int i = 1; i <= 5; ++i) {
        int r = maxRadius * i / 5;
        QString label = QString("%1m").arg(i * 20); // Assuming 100m max range
        painter.drawText(center.x() + r + 5, center.y() - 5, label);
    }
    
    // Draw azimuth angle labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    for (int angle = -90; angle <= 90; angle += 30) {
        double rad = angle * M_PI / 180.0;
        int x = center.x() + (maxRadius + 15) * cos(rad);
        int y = center.y() - (maxRadius + 15) * sin(rad);
        painter.drawText(x - 10, y + 5, QString("%1°").arg(angle));
    }
    
    // Draw detections (only show detections within -90 to +90 degree range)
    if (!detections.empty()) {
        for (const auto& detection : detections) {
            // Only show detections within the semicircle range
            if (detection.azimuth >= -90 && detection.azimuth <= 90) {
                // Calculate position
                double normalizedRadius = qMin(detection.radius / 100.0, 1.0); // Normalize to 0-100m
                double rad = detection.azimuth * M_PI / 180.0;
                
                int x = center.x() + normalizedRadius * maxRadius * cos(rad);
                int y = center.y() - normalizedRadius * maxRadius * sin(rad);
                
                // Color based on radial speed
                QColor color = getColorForSpeed(detection.radial_speed);
                
                // Size based on amplitude, with minimum size for visibility
                int size = qMax(8, qMin(20, (int)(detection.amplitude / 5.0 + 8)));
                painter.setPen(QPen(color, 2));
                painter.setBrush(QBrush(color));
                painter.drawEllipse(x - size/2, y - size/2, size, size);
                
                // Draw target ID
                painter.setPen(Qt::white);
                painter.drawText(x + size/2 + 2, y - size/2 - 2, QString::number(detection.target_id));
            }
        }
    }
    
    // Draw labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, 20, "Detection Plot");
    painter.drawText(10, 35, "Range vs Azimuth (-90° to +90°)");
    
    // Draw zoom level indicator
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    painter.drawText(10, height() - 20, QString("Zoom: %1x").arg(zoomLevel, 0, 'f', 1));
}

void CustomChart::drawHistogramChart(QPainter& painter)
{
    QMutexLocker locker(&dataMutex);
    
    if (histogramData.empty()) return;
    
    drawAxes(painter);
    
    // Draw histogram bars
    int barWidth = plotArea.width() / histogramData.size();
    for (size_t i = 0; i < histogramData.size(); ++i) {
        int x = plotArea.left() + i * barWidth;
        int barHeight = (histogramData[i] / 50.0) * plotArea.height();
        int y = plotArea.bottom() - barHeight;
        
        QColor color = QColor::fromHsv((i * 360 / histogramData.size()) % 360, 200, 200);
        painter.fillRect(x, y, barWidth - 1, barHeight, color);
    }
    
    // Draw labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, plotArea.center().y(), "Count");
    painter.drawText(plotArea.center().x() - 20, height() - 10, "Velocity/Range");
}

TargetDetection CustomChart::getDetectionAt(const QPoint& point) const
{
    QMutexLocker locker(&dataMutex);
    
    if (chartType != DETECTION_CHART) {
        return TargetDetection();
    }
    
    QPoint center = plotArea.center();
    int maxRadius = qMin(plotArea.width(), plotArea.height()) / 2 - 20;
    
    for (const auto& detection : detections) {
        QPoint detectionPoint = detectionToPoint(detection);
        
        // Check if click is within detection circle
        int distance = sqrt(pow(point.x() - detectionPoint.x(), 2) + pow(point.y() - detectionPoint.y(), 2));
        int detectionSize = qMax(8, qMin(20, (int)(detection.amplitude / 5.0 + 8)));

        if (distance <= detectionSize / 2) {
            return detection;
        }
    }
    
    return TargetDetection();
}

QPoint CustomChart::detectionToPoint(const TargetDetection& detection) const
{
    QPoint center = QPoint(plotArea.center().x(), plotArea.bottom() - 20);
    int maxRadius = (qMin(plotArea.width(), plotArea.height()) * zoomLevel) / 2 - 20;
    
    // Ensure semicircle fits within the plot area
    if (maxRadius > plotArea.height() - 40) {
        maxRadius = plotArea.height() - 40;
    }
    if (maxRadius > plotArea.width() / 2 - 40) {
        maxRadius = plotArea.width() / 2 - 40;
    }
    
    double normalizedRadius = qMin(detection.radius / 100.0, 1.0);
    double rad = detection.azimuth * M_PI / 180.0;
    
    int x = center.x() + normalizedRadius * maxRadius * cos(rad);
    int y = center.y() - normalizedRadius * maxRadius * sin(rad);
    
    return QPoint(x, y);
}

QColor CustomChart::getColorForSpeed(double speed) const
{
    if (speed > 2.0) {
        return QColor(255, 0, 0); // Red for approaching
    } else if (speed < -2.0) {
        return QColor(0, 255, 0); // Green for receding
    } else {
        return QColor(255, 255, 0); // Yellow for stationary
    }
}

QColor CustomChart::getColorForAmplitude(double amplitude) const
{
    // Color based on amplitude: blue (low) to red (high)
    int hue = qMax(0, qMin(240, (int)(240 - amplitude * 2.4))); // 240 (blue) to 0 (red)
    return QColor::fromHsv(hue, 255, 255);
}
