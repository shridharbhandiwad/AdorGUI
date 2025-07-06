#ifndef TARGETLIST_H
#define TARGETLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QMutex>
#include <QScrollBar>
#include <QDateTime>
#include <vector>
#include <memory>
#include "structures.h"

class TargetListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TargetListWidget(QWidget* parent = nullptr);
    ~TargetListWidget();

    // Data management
    void addTarget(const TargetDetection& detection);
    void setTargets(const std::vector<TargetDetection>& targets);
    void clearTargets();
    
    // Configuration
    void setMaxTargets(int maxTargets);
    void setAutoScroll(bool enabled);
    void setShowTimestamp(bool show);
    void setCompactView(bool compact);
    
    // Statistics
    int getTargetCount() const;
    TargetDetection getLatestTarget() const;
    std::vector<TargetDetection> getTargets() const;

signals:
    void targetSelected(const TargetDetection& target);
    void targetCountChanged(int count);
    void statusChanged(const QString& status);

public slots:
    void onNewDetection(const DetectionData& detection);
    void refreshDisplay();

private slots:
    void onTargetItemClicked(QListWidgetItem* item);
    void updateStatus();
    void cleanupOldTargets();

private:
    // UI Components
    QPushButton* statusButton;
    QLabel* countLabel;
    QLabel* statisticsLabel;
    QListWidget* targetListWidget;
    QPushButton* clearButton;
    QPushButton* exportButton;
    
    // Data storage
    mutable QMutex targetsMutex;
    std::vector<TargetDetection> targets;
    int maxTargets;
    bool autoScroll;
    bool showTimestamp;
    bool compactView;
    
    // Timers
    QTimer* updateTimer;
    QTimer* cleanupTimer;
    
    // Statistics
    int totalTargetsReceived;
    qint64 lastUpdateTime;
    
    // Helper methods
    void setupUI();
    void updateDisplay();
    void updateStatusButton();
    QString formatTarget(const TargetDetection& target) const;
    QString getStatusText() const;
    QColor getStatusColor() const;
    
    // Target management
    void addTargetInternal(const TargetDetection& target);
    void removeOldTargets();
    
    // Export functionality
    void exportTargets();
    
    // UI Updates
    void scrollToBottom();
    void updateCountLabel();
    void updateStatisticsLabel();
};

// Custom list widget item for targets
class TargetListItem : public QListWidgetItem
{
public:
    TargetListItem(const TargetDetection& target, QListWidget* parent = nullptr);
    
    const TargetDetection& getTarget() const { return targetData; }
    void updateTarget(const TargetDetection& target);
    
private:
    TargetDetection targetData;
    void updateText();
};

#endif // TARGETLIST_H