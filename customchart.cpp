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
    // Create a sophisticated background with multiple gradients
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Outer gradient for depth
    QLinearGradient outerGradient(rect().topLeft(), rect().bottomRight());
    outerGradient.setColorAt(0, QColor(248, 250, 252));    // Very light blue-gray
    outerGradient.setColorAt(0.3, QColor(241, 245, 249));  // Light blue-gray
    outerGradient.setColorAt(0.7, QColor(226, 232, 240));  // Medium light blue-gray
    outerGradient.setColorAt(1, QColor(203, 213, 225));    // Darker blue-gray
    painter.fillRect(rect(), QBrush(outerGradient));
    
    // Inner glow effect
    QRadialGradient glowGradient(rect().center(), qMin(rect().width(), rect().height()) / 2);
    glowGradient.setColorAt(0, QColor(255, 255, 255, 60));  // White center glow
    glowGradient.setColorAt(0.6, QColor(59, 130, 246, 20)); // Blue mid glow
    glowGradient.setColorAt(1, QColor(59, 130, 246, 0));    // Transparent edge
    painter.fillRect(rect(), QBrush(glowGradient));
    
    // Subtle pattern overlay for texture
    QPen patternPen(QColor(59, 130, 246, 15), 1, Qt::DotLine);
    painter.setPen(patternPen);
    
    // Draw subtle diagonal pattern
    for (int i = 0; i < rect().width() + rect().height(); i += 20) {
        painter.drawLine(i, 0, 0, i);
        painter.drawLine(rect().width() - i, rect().height(), rect().width(), rect().height() - i);
    }
}

void CustomChart::drawGrid(QPainter& painter)
{
    if (!showGrid) return;
    
    painter.setPen(QPen(QColor(100, 149, 237), 1, Qt::DotLine));  // Cornflower blue grid
    
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
    painter.setPen(QPen(QColor(25, 25, 112), 2));  // Midnight blue axes
    painter.drawLine(plotArea.bottomLeft(), plotArea.bottomRight());
    painter.drawLine(plotArea.bottomLeft(), plotArea.topLeft());
}

void CustomChart::drawLegend(QPainter& painter)
{
    painter.setPen(QColor(25, 25, 112));  // Midnight blue text
    painter.setFont(QFont("Arial", 8));
    
    int legendY = plotArea.top() + 10;
    int legendX = plotArea.right() - 150;
    
    switch (chartType) {
    case FFT_CHART:
        painter.drawText(legendX, legendY, "FFT Signal");
        painter.setPen(QColor(34, 139, 34));  // Forest green
        painter.drawLine(legendX - 20, legendY - 5, legendX - 5, legendY - 5);
        
        painter.setPen(QColor(25, 25, 112));
        painter.drawText(legendX, legendY + 15, "Threshold");
        painter.setPen(QColor(255, 140, 0));  // Dark orange
        painter.drawLine(legendX - 20, legendY + 10, legendX - 5, legendY + 10);
        break;
        
    case DETECTION_CHART:
        painter.setPen(QColor(25, 25, 112));
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
    painter.setPen(QColor(25, 25, 112));  // Midnight blue
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, plotArea.center().y(), "Amplitude (dB)");
    painter.drawText(plotArea.center().x() - 30, height() - 10, "Frequency (Hz)");
    
    // Draw threshold line
    painter.setPen(QPen(QColor(255, 140, 0), 2));  // Dark orange
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
    painter.setPen(QPen(QColor(34, 139, 34), 2));  // Forest green
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
    painter.setPen(QPen(QColor(128, 128, 128), 1));  // Gray
    painter.drawLine(plotArea.left(), zeroY, plotArea.right(), zeroY);
    
    // Draw raw signal data
    painter.setPen(QPen(QColor(0, 191, 255), 2));  // Deep sky blue
    QPolygon signalPoly;
    for (size_t i = 0; i < rawSignalData.size(); ++i) {
        int x = plotArea.left() + i * plotArea.width() / (rawSignalData.size() - 1);
        int y = zeroY - rawSignalData[i] * plotArea.height() / 4;
        signalPoly << QPoint(x, y);
    }
    painter.drawPolyline(signalPoly);
    
    // Draw labels
    painter.setPen(QColor(25, 25, 112));  // Midnight blue
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, plotArea.center().y(), "Amplitude");
    painter.drawText(plotArea.center().x() - 20, height() - 10, "Samples");
}

void CustomChart::drawDetectionChart(QPainter& painter)
{
    QMutexLocker locker(&dataMutex);
    
    drawGrid(painter);
    
    // Calculate semicircle parameters - DOUBLED THE SIZE
    QPoint center = QPoint(plotArea.center().x(), plotArea.bottom() - 20);
    int maxRadius = (qMin(plotArea.width(), plotArea.height()) * zoomLevel) - 40; // Doubled from /2 to *1, adjusted margin
    
    // Ensure semicircle fits within the plot area
    if (maxRadius > plotArea.height() - 40) {
        maxRadius = plotArea.height() - 40;
    }
    if (maxRadius > plotArea.width() / 2 - 40) {
        maxRadius = plotArea.width() / 2 - 40;
    }
    
    // Draw range circles (semicircles from -90 to +90 degrees)
    painter.setPen(QPen(QColor(148, 163, 184, 180), 2, Qt::SolidLine));  // Enhanced gray with transparency
    for (int i = 1; i <= 5; ++i) {
        int r = maxRadius * i / 5;
        // Add subtle gradient to range circles
        QPen circlePen(QColor(100, 149, 237, 120 + i * 20), 2);
        painter.setPen(circlePen);
        painter.drawArc(center.x() - r, center.y() - r, 2 * r, 2 * r, 0, 180 * 16); // 180 degrees in 16th of a degree
    }
    
    // Draw azimuth lines from -90 to +90 degrees
    painter.setPen(QPen(QColor(100, 149, 237, 100), 1, Qt::DashLine));  // Enhanced cornflower blue with transparency
    for (int angle = 0; angle <= 180; angle += 15) {
        double rad = angle * M_PI / 180.0;
        int x = center.x() + maxRadius * cos(rad);
        int y = center.y() - maxRadius * sin(rad);
        
        // Create gradient pen for azimuth lines
        QPen azimuthPen(QColor(59, 130, 246, 80 + angle), 1, Qt::DashLine);
        painter.setPen(azimuthPen);
        painter.drawLine(center, QPoint(x, y));
    }
    
    // Draw main axes (horizontal baseline and vertical 0-degree line) with enhanced styling
    QPen axisPen(QColor(30, 41, 59), 3);  // Darker midnight blue, thicker
    axisPen.setCapStyle(Qt::RoundCap);
    painter.setPen(axisPen);
    painter.drawLine(center.x() - maxRadius, center.y(), center.x() + maxRadius, center.y()); // Horizontal baseline
    painter.drawLine(center.x(), center.y(), center.x(), center.y() - maxRadius); // Vertical 0-degree line
    
    // Add subtle shadow effect to main axes
    QPen shadowPen(QColor(0, 0, 0, 40), 4);
    shadowPen.setCapStyle(Qt::RoundCap);
    painter.setPen(shadowPen);
    painter.drawLine(center.x() - maxRadius + 1, center.y() + 1, center.x() + maxRadius + 1, center.y() + 1);
    painter.drawLine(center.x() + 1, center.y() + 1, center.x() + 1, center.y() - maxRadius + 1);
    
    // Draw range labels with enhanced styling
    painter.setPen(QColor(30, 41, 59));  // Dark blue text
    painter.setFont(QFont("Arial", 9, QFont::Bold));
    for (int i = 1; i <= 5; ++i) {
        int r = maxRadius * i / 5;
        QString label = QString("%1m").arg(i * 20); // Assuming 100m max range
        
        // Draw label background
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(label);
        QRect bgRect(center.x() + r + 2, center.y() - 10, textRect.width() + 6, textRect.height() + 4);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 255, 220)));
        painter.drawRoundedRect(bgRect, 4, 4);
        
        // Draw label text
        painter.setPen(QColor(30, 41, 59));
        painter.drawText(center.x() + r + 5, center.y() - 2, label);
    }
    
    // Draw azimuth angle labels with enhanced styling
    painter.setPen(QColor(30, 41, 59));  // Dark blue text
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    for (int angle = 0; angle <= 180; angle += 30) {
        double rad = angle * M_PI / 180.0;
        int x = center.x() + (maxRadius + 15) * cos(rad);
        int y = center.y() - (maxRadius + 15) * sin(rad);
        QString angleLabel = QString("%1°").arg(angle-90);
        
        // Draw angle label background
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(angleLabel);
        QRect bgRect(x - textRect.width()/2 - 3, y - textRect.height()/2 - 2, textRect.width() + 6, textRect.height() + 4);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
        painter.drawRoundedRect(bgRect, 3, 3);
        
        // Draw angle text
        painter.setPen(QColor(30, 41, 59));
        painter.drawText(x - textRect.width()/2, y + textRect.height()/2 - 2, angleLabel);
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
                int size = qMax(12, qMin(24, (int)(detection.amplitude / 5.0 + 12))); // Increased base size
                
                // Draw shadow first
                QColor shadowColor(0, 0, 0, 80);
                painter.setPen(QPen(shadowColor, 1));
                painter.setBrush(QBrush(shadowColor));
                painter.drawEllipse(x - size/2 + 2, y - size/2 + 2, size, size);
                
                // Draw outer glow
                QRadialGradient glowGradient(QPointF(x, y), size);
                glowGradient.setColorAt(0, QColor(color.red(), color.green(), color.blue(), 180));
                glowGradient.setColorAt(0.7, QColor(color.red(), color.green(), color.blue(), 100));
                glowGradient.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 0));
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(glowGradient));
                painter.drawEllipse(x - size, y - size, size * 2, size * 2);
                
                // Draw main detection marker with gradient
                QRadialGradient markerGradient(QPointF(x - size/4, y - size/4), size/2);
                markerGradient.setColorAt(0, color.lighter(150));
                markerGradient.setColorAt(1, color.darker(120));
                
                painter.setPen(QPen(color.darker(140), 2));
                painter.setBrush(QBrush(markerGradient));
                painter.drawEllipse(x - size/2, y - size/2, size, size);
                
                // Draw inner highlight
                QColor highlightColor = color.lighter(200);
                highlightColor.setAlpha(120);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(highlightColor));
                painter.drawEllipse(x - size/4, y - size/4, size/2, size/2);
                
                // Draw target ID with enhanced styling
                painter.setPen(QColor(30, 41, 59));  // Dark blue text
                painter.setFont(QFont("Arial", 9, QFont::Bold));
                QString idText = QString::number(detection.target_id);
                QFontMetrics fm(painter.font());
                QRect textRect = fm.boundingRect(idText);
                
                // Draw text background
                QRect bgRect(x + size/2 + 4, y - size/2 - 4, textRect.width() + 4, textRect.height() + 2);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
                painter.drawRoundedRect(bgRect, 3, 3);
                
                // Draw text
                painter.setPen(QColor(30, 41, 59));
                painter.drawText(x + size/2 + 6, y - size/2 + textRect.height() - 6, idText);
            }
        }
    }
    
    // Draw enhanced title and labels
    QFont titleFont("Arial", 14, QFont::Bold);
    QFont subtitleFont("Arial", 11, QFont::Normal);
    QFontMetrics titleFm(titleFont);
    QFontMetrics subtitleFm(subtitleFont);
    
    QString mainTitle = "Detection Plot";
    QString subtitle = "Range vs Azimuth (-90° to +90°)";
    
    // Calculate title area
    int titleWidth = qMax(titleFm.horizontalAdvance(mainTitle), subtitleFm.horizontalAdvance(subtitle)) + 20;
    int titleHeight = titleFm.height() + subtitleFm.height() + 15;
    QRect titleArea(15, 15, titleWidth, titleHeight);
    
    // Draw title background with gradient
    QLinearGradient titleGradient(titleArea.topLeft(), titleArea.bottomLeft());
    titleGradient.setColorAt(0, QColor(59, 130, 246, 200));
    titleGradient.setColorAt(1, QColor(37, 99, 235, 220));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(titleGradient));
    painter.drawRoundedRect(titleArea, 8, 8);
    
    // Draw title shadow
    painter.setBrush(QBrush(QColor(0, 0, 0, 60)));
    painter.drawRoundedRect(titleArea.adjusted(2, 2, 2, 2), 8, 8);
    
    // Redraw title background on top
    painter.setBrush(QBrush(titleGradient));
    painter.drawRoundedRect(titleArea, 8, 8);
    
    // Draw title text
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(titleFont);
    painter.drawText(titleArea.left() + 10, titleArea.top() + titleFm.ascent() + 5, mainTitle);
    
    painter.setFont(subtitleFont);
    painter.drawText(titleArea.left() + 10, titleArea.top() + titleFm.height() + subtitleFm.ascent() + 8, subtitle);
    
    // Draw zoom level indicator with enhanced styling
    QString zoomText = QString("Zoom: %1x").arg(zoomLevel, 0, 'f', 1);
    QFont zoomFont("Arial", 10, QFont::Bold);
    QFontMetrics zoomFm(zoomFont);
    
    int zoomWidth = zoomFm.horizontalAdvance(zoomText) + 16;
    int zoomHeight = zoomFm.height() + 8;
    QRect zoomArea(15, height() - zoomHeight - 15, zoomWidth, zoomHeight);
    
    // Draw zoom background
    QLinearGradient zoomGradient(zoomArea.topLeft(), zoomArea.bottomLeft());
    zoomGradient.setColorAt(0, QColor(34, 197, 94, 180));
    zoomGradient.setColorAt(1, QColor(22, 163, 74, 200));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(zoomGradient));
    painter.drawRoundedRect(zoomArea, 6, 6);
    
    // Draw zoom text
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(zoomFont);
    painter.drawText(zoomArea.left() + 8, zoomArea.bottom() - 6, zoomText);
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
    int maxRadius = (qMin(plotArea.width(), plotArea.height()) * zoomLevel) - 40; // Doubled from /2 to *1, adjusted margin
    
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
