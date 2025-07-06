#ifndef CUSTOMCHART_H
#define CUSTOMCHART_H

#include <QWidget>
#include <QTimer>
#include <QMutex>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <vector>
#include <memory>
#include <random>
#include "structures.h"

class CustomChart : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        FFT_CHART,
        RAW_SIGNAL_CHART,
        DETECTION_CHART,
        HISTOGRAM_CHART
    };

    explicit CustomChart(ChartType type, QWidget* parent = nullptr);
    ~CustomChart();

    // Control methods
    void setFrozen(bool freeze);
    bool isFrozen() const { return frozen; }
    
    // Configuration
    void setThreshold(double threshold);
    double getThreshold() const { return threshold; }
    
    void setUpdateInterval(int intervalMs);
    int getUpdateInterval() const;
    
    // Data management
    void addDetection(const TargetDetection& detection);
    void setDetections(const std::vector<TargetDetection>& detections);
    void clearDetections();
    
    // Chart-specific data
    void setFFTData(const std::vector<double>& data);
    void setRawSignalData(const std::vector<double>& data);
    void setHistogramData(const std::vector<double>& data);
    
    // Display options
    void setShowLegend(bool show) { showLegend = show; update(); }
    void setShowGrid(bool show) { showGrid = show; update(); }
    void setMaxDataPoints(int maxPoints) { maxDataPoints = maxPoints; }
    
    // Zoom functionality
    void zoomIn();
    void zoomOut();
    void resetZoom();
    double getZoomLevel() const { return zoomLevel; }

signals:
    void dataUpdated();
    void detectionClicked(const TargetDetection& detection);
    void zoomChanged(double zoomLevel);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void updateData();

private:
    // Chart properties
    ChartType chartType;
    bool frozen;
    double threshold;
    bool showLegend;
    bool showGrid;
    int maxDataPoints;
    double zoomLevel;
    
    // Timers
    QTimer* updateTimer;
    
    // Data storage
    mutable QMutex dataMutex;
    std::vector<double> fftData;
    std::vector<double> rawSignalData;
    std::vector<double> histogramData;
    std::vector<double> thresholdData;
    std::vector<TargetDetection> detections;
    
    // Chart dimensions
    QRect plotArea;
    QRect legendArea;
    
    // Random number generator for sample data
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    std::uniform_real_distribution<> fft_dis;
    
    // Drawing methods
    void drawFFTChart(QPainter& painter);
    void drawRawSignalChart(QPainter& painter);
    void drawDetectionChart(QPainter& painter);
    void drawHistogramChart(QPainter& painter);
    
    // Helper methods
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawAxes(QPainter& painter);
    void drawLegend(QPainter& painter);
    void calculatePlotArea();
    
    // Data generation (for testing)
    void generateSampleData();
    
    // Detection interaction
    TargetDetection getDetectionAt(const QPoint& point) const;
    QPoint detectionToPoint(const TargetDetection& detection) const;
    
    // Color utilities
    QColor getColorForSpeed(double speed) const;
    QColor getColorForAmplitude(double amplitude) const;
};

#endif // CUSTOMCHART_H