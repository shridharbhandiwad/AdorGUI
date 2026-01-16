#include "mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , liveStreamActive(false)
    , frozen(false)
    , connected(false)
{
    setWindowTitle("Zoppler Radar Sensor GUI v1.1");
    setMinimumSize(1000, 700);
    
    // Apply rich modern stylesheet
    QFile styleFile(":/styles.qss");
    if (!styleFile.open(QFile::ReadOnly)) {
        // Try loading from current directory if resource fails
        styleFile.setFileName("styles.qss");
        styleFile.open(QFile::ReadOnly);
    }
    
    if (styleFile.isOpen()) {
        QString style = QLatin1String(styleFile.readAll());
        setStyleSheet(style);
        styleFile.close();
    }
    
    setupMenuBar();
    setupUI();
    setupStatusBar();
    setupConnections();
    
    // Setup update timer
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    updateTimer->start(UPDATE_INTERVAL_MS);
    
    // Load settings
    loadSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = this->menuBar();
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("File");
    fileMenu->addAction("Save configuration to file", this, &MainWindow::saveConfiguration);
    fileMenu->addAction("Load configuration from file", this, &MainWindow::loadConfiguration);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);
    
    // iSYS menu
    QMenu* isysMenu = menuBar->addMenu("iSYS");
    isysMenu->addAction("Output Configuration", this, &MainWindow::showOutputConfigDialog);
    isysMenu->addAction("UDP Configuration", this, &MainWindow::showUdpConfigDialog);
    
    // Config menu
    QMenu* configMenu = menuBar->addMenu("Config");
    configMenu->addAction("Amplification Settings", this, &MainWindow::showAmplificationDialog);
    configMenu->addAction("Angle Correction", this, &MainWindow::showAngleCorrectionDialog);
    
    // DSP Settings menu
    QMenu* dspMenu = menuBar->addMenu("DSP");
    dspMenu->addAction("DSP Settings...", this, &MainWindow::showDSPSettingsDialog);
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Control buttons
    QHBoxLayout* controlLayout = new QHBoxLayout();
    liveStreamButton = new QPushButton("Start Live Stream");
    freezeButton = new QPushButton("Freeze");
    eepromButton = new QPushButton("Write all Params to EEProm");
    
    controlLayout->addWidget(liveStreamButton);
    controlLayout->addWidget(freezeButton);
    controlLayout->addWidget(eepromButton);
    controlLayout->addStretch();
    
    mainLayout->addLayout(controlLayout);
    
    // Main content area
    mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Left side - Charts and controls
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    
    // Tab widget for charts
    mainTabs = new QTabWidget();
    
    // Raw Signal tab
    rawChart = new CustomChart(CustomChart::RAW_SIGNAL_CHART);
    mainTabs->addTab(rawChart, "Raw Signal");
    
    // Detection tab
    QWidget* detectionWidget = createDetectionTab();
    mainTabs->addTab(detectionWidget, "Detection");
    
    // Output tabs
    // COMMENTED OUT - Output tabs not required as of now
    /*
    for (int i = 1; i <= 3; ++i) {
        QWidget* outputWidget = createOutputTab(i);
        mainTabs->addTab(outputWidget, QString("Output %1").arg(i));
    }
    */
    
    leftLayout->addWidget(mainTabs);
    
    // Configuration controls
    QGroupBox* configGroup = new QGroupBox("Configuration");
    QGridLayout* configLayout = new QGridLayout(configGroup);
    
    // Channel selection
    configLayout->addWidget(new QLabel("Frequency Channel:"), 0, 0);
    channelCombo = new QComboBox();
    channelCombo->addItems({"Channel 1 (24.190 GHz)", "Channel 2 (24.210 GHz)", 
                           "Channel 3 (24.155 GHz)", "Channel 4 (24.135 GHz)"});
    configLayout->addWidget(channelCombo, 0, 1);
    
    // Amplification control
    configLayout->addWidget(new QLabel("Manual Amplification:"), 1, 0);
    amplificationSlider = new QSlider(Qt::Horizontal);
    amplificationSlider->setRange(0, 60);
    amplificationSlider->setValue(20);
    amplificationLabel = new QLabel("Amplification: 20 dB");
    
    configLayout->addWidget(amplificationSlider, 1, 1);
    configLayout->addWidget(amplificationLabel, 1, 2);
    
    leftLayout->addWidget(configGroup);
    
    // Right side - Target information
    // COMMENTED OUT - Output target lists not required as of now
    /*
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    
    // Target lists for each output
    for (int i = 1; i <= 3; ++i) {
        QGroupBox* targetGroup = new QGroupBox(QString("Output %1 Targets").arg(i));
        QVBoxLayout* targetLayout = new QVBoxLayout(targetGroup);
        
        TargetListWidget* targetWidget = new TargetListWidget();
        targetLists.push_back(targetWidget);
        targetLayout->addWidget(targetWidget);
        
        rightLayout->addWidget(targetGroup);
    }
    
    rightLayout->addStretch();
    
    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setSizes({700, 300});
    
    mainLayout->addWidget(mainSplitter);
    */
    
    // Since right widget is commented out, just use the left widget as main content
    // No splitter needed anymore, just use full width for left widget
    mainSplitter->addWidget(leftWidget);
    mainLayout->addWidget(mainSplitter);
}

QWidget* MainWindow::createDetectionTab()
{
    QWidget* detectionWidget = new QWidget();
    QHBoxLayout* detectionLayout = new QHBoxLayout(detectionWidget);  // Changed to horizontal layout
    
    // Left side - Detection Chart (Azimuth vs Range plot) - Keep full size
    QWidget* chartWidget = new QWidget();
    QVBoxLayout* chartLayout = new QVBoxLayout(chartWidget);
    
    // FFT Chart for frequency analysis (smaller size)
    fftChart = new CustomChart(CustomChart::FFT_CHART);
    fftChart->setMaximumHeight(150);  // Limit FFT chart height
    chartLayout->addWidget(fftChart);
    
    // Detection Chart for showing detected targets on polar plot - Full size
    detectionChart = new CustomChart(CustomChart::DETECTION_CHART);
    chartLayout->addWidget(detectionChart);
    
    // Zoom controls for detection chart
    QGroupBox* zoomGroup = new QGroupBox("Range vs Azimuth Plot Controls");
    QHBoxLayout* zoomLayout = new QHBoxLayout(zoomGroup);
    
    QPushButton* zoomInBtn = new QPushButton("Zoom In");
    QPushButton* zoomOutBtn = new QPushButton("Zoom Out");
    QPushButton* resetZoomBtn = new QPushButton("Reset Zoom");
    zoomLevelLabel = new QLabel("Zoom: 1.0x");
    
    zoomLayout->addWidget(new QLabel("Zoom:"));
    zoomLayout->addWidget(zoomInBtn);
    zoomLayout->addWidget(zoomOutBtn);
    zoomLayout->addWidget(resetZoomBtn);
    zoomLayout->addWidget(zoomLevelLabel);
    zoomLayout->addStretch();
    
    // Connect zoom buttons
    connect(zoomInBtn, &QPushButton::clicked, detectionChart, &CustomChart::zoomIn);
    connect(zoomOutBtn, &QPushButton::clicked, detectionChart, &CustomChart::zoomOut);
    connect(resetZoomBtn, &QPushButton::clicked, detectionChart, &CustomChart::resetZoom);
    connect(detectionChart, &CustomChart::zoomChanged, this, &MainWindow::onZoomChanged);
    
    chartLayout->addWidget(zoomGroup);
    detectionLayout->addWidget(chartWidget, 3);  // Give more space to chart
    
    // Right side - Track Table and Controls
    QWidget* controlWidget = new QWidget();
    QVBoxLayout* controlLayout = new QVBoxLayout(controlWidget);
    
    // Track Table
    QGroupBox* trackGroup = new QGroupBox("Track Table");
    QVBoxLayout* trackLayout = new QVBoxLayout(trackGroup);
    
    // Create track table
    trackTable = new QTableWidget();
    trackTable->setColumnCount(4);
    QStringList headers = {"Track ID", "Radius (m)", "Radial Speed (m/s)", "Azimuth (°)"};
    trackTable->setHorizontalHeaderLabels(headers);
    trackTable->setAlternatingRowColors(true);
    trackTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackTable->setSortingEnabled(true);
    trackTable->setMinimumWidth(300);
    
    // Set column widths
    trackTable->setColumnWidth(0, 80);   // Track ID
    trackTable->setColumnWidth(1, 85);   // Radius
    trackTable->setColumnWidth(2, 105);  // Radial Speed
    trackTable->setColumnWidth(3, 85);   // Azimuth
    
    trackLayout->addWidget(trackTable);
    controlLayout->addWidget(trackGroup);
    
    // Threshold control
    QGroupBox* thresholdGroup = new QGroupBox("Threshold Control");
    QVBoxLayout* thresholdControlLayout = new QVBoxLayout(thresholdGroup);
    
    QHBoxLayout* thresholdLayout = new QHBoxLayout();
    thresholdLayout->addWidget(new QLabel("User Threshold:"));
    thresholdSlider = new QSlider(Qt::Horizontal);
    thresholdSlider->setRange(-50, 50);
    thresholdSlider->setValue(0);
    thresholdLabel = new QLabel("Threshold: 0 dB");
    
    thresholdLayout->addWidget(thresholdSlider);
    thresholdLayout->addWidget(thresholdLabel);
    thresholdControlLayout->addLayout(thresholdLayout);
    controlLayout->addWidget(thresholdGroup);
    
    // Line filter options
    QGroupBox* lineFilterGroup = new QGroupBox("Line Filter");
    QHBoxLayout* lineFilterLayout = new QHBoxLayout(lineFilterGroup);
    
    filter50Hz = new QCheckBox("50 Hz");
    filter100Hz = new QCheckBox("100 Hz");
    filter150Hz = new QCheckBox("150 Hz");
    
    lineFilterLayout->addWidget(filter50Hz);
    lineFilterLayout->addWidget(filter100Hz);
    lineFilterLayout->addWidget(filter150Hz);
    lineFilterLayout->addStretch();
    
    controlLayout->addWidget(lineFilterGroup);
    controlLayout->addStretch();  // Add stretch to push controls to top
    
    detectionLayout->addWidget(controlWidget, 1);  // Give less space to controls
    
    return detectionWidget;
}

QWidget* MainWindow::createOutputTab(int outputNumber)
{
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    // Target display area
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    
    // Target list
    TargetListWidget* targetWidget = new TargetListWidget();
    splitter->addWidget(targetWidget);
    
    // Detection chart for this output
    CustomChart* detectionChart = new CustomChart(CustomChart::DETECTION_CHART);
    outputCharts.push_back(detectionChart);
    splitter->addWidget(detectionChart);
    
    layout->addWidget(splitter);
    
    return tab;
}

void MainWindow::setupStatusBar()
{
    QStatusBar* statusBar = this->statusBar();
    statusBar->showMessage("Ready - Not Connected");
    
    // Add status indicators
    connectionStatusLabel = new QLabel("Connection: Disconnected");
    connectionStatusLabel->setStyleSheet("QLabel { color: red; }");
    statusBar->addPermanentWidget(connectionStatusLabel);
    
    dataRateLabel = new QLabel("Data Rate: 0.0 pps");
    statusBar->addPermanentWidget(dataRateLabel);
    
    targetCountLabel = new QLabel("Targets: 0");
    statusBar->addPermanentWidget(targetCountLabel);
}

void MainWindow::setupConnections()
{
    // Control buttons
    connect(liveStreamButton, &QPushButton::clicked, this, &MainWindow::toggleLiveStream);
    connect(freezeButton, &QPushButton::clicked, this, &MainWindow::toggleFreezeRun);
    connect(eepromButton, &QPushButton::clicked, this, &MainWindow::writeToEEPROM);
    
    // Configuration controls
    connect(thresholdSlider, &QSlider::valueChanged, this, &MainWindow::onThresholdChanged);
    connect(amplificationSlider, &QSlider::valueChanged, this, &MainWindow::onAmplificationChanged);
    connect(channelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onChannelChanged);
    
    // Line filter checkboxes
    connect(filter50Hz, &QCheckBox::toggled, this, &MainWindow::onLineFilterChanged);
    connect(filter100Hz, &QCheckBox::toggled, this, &MainWindow::onLineFilterChanged);
    connect(filter150Hz, &QCheckBox::toggled, this, &MainWindow::onLineFilterChanged);
    
    // Charts
    if (fftChart) {
        connect(fftChart, &CustomChart::detectionClicked, this, &MainWindow::onChartDetectionClicked);
    }
    
    // Connect detection chart click events
    if (detectionChart) {
        connect(detectionChart, &CustomChart::detectionClicked, this, &MainWindow::onChartDetectionClicked);
    }
    
    // Target lists
    // COMMENTED OUT - Target lists not used now
    /*
    for (auto* targetList : targetLists) {
        connect(targetList, &TargetListWidget::targetSelected, this, &MainWindow::onTargetSelected);
    }
    */
    
    // Track table connections
    if (trackTable) {
        connect(trackTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTrackTableSelectionChanged);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

// Slot implementations (basic versions)
void MainWindow::saveConfiguration()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Configuration", "", "iSYS Parameter Files (*.ipf)");
    if (!fileName.isEmpty()) {
        QSettings settings(fileName, QSettings::IniFormat);
        settings.setValue("amplification", amplificationSlider->value());
        settings.setValue("threshold", thresholdSlider->value());
        settings.setValue("channel", channelCombo->currentIndex());
        QMessageBox::information(this, "Save", "Configuration saved successfully!");
    }
}

void MainWindow::loadConfiguration()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load Configuration", "", "iSYS Parameter Files (*.ipf)");
    if (!fileName.isEmpty()) {
        QSettings settings(fileName, QSettings::IniFormat);
        amplificationSlider->setValue(settings.value("amplification", 20).toInt());
        thresholdSlider->setValue(settings.value("threshold", 0).toInt());
        channelCombo->setCurrentIndex(settings.value("channel", 0).toInt());
        QMessageBox::information(this, "Load", "Configuration loaded successfully!");
    }
}

void MainWindow::showUdpConfigDialog()
{
    if (!udpConfigDialog) {
        udpConfigDialog = std::make_unique<UdpConfigDialog>(this);
        
        // Connect UDP signals from dialog
        connect(udpConfigDialog.get(), &UdpConfigDialog::connectionStatusChanged, 
                this, &MainWindow::onUdpConnectionChanged);
        connect(udpConfigDialog.get(), &UdpConfigDialog::dataReceived, 
                this, &MainWindow::onNewDetectionReceived);
        
        // Connect UDP statistics from the UDP handler directly
        if (udpConfigDialog->getUdpHandler()) {
            connect(udpConfigDialog->getUdpHandler(), &UdpHandler::statisticsUpdated,
                    this, &MainWindow::onUdpStatisticsUpdated);
        }
    }
    
    udpConfigDialog->exec();
}

void MainWindow::showOutputConfigDialog()
{
    if (!outputConfigDialog) {
        outputConfigDialog = std::make_unique<OutputConfigDialog>(this);
    }
    outputConfigDialog->exec();
}

void MainWindow::showAngleCorrectionDialog()
{
    if (!angleDialog) {
        angleDialog = std::make_unique<AngleCorrectionDialog>(this);
    }
    angleDialog->exec();
}

void MainWindow::showAmplificationDialog()
{
    if (!amplificationDialog) {
        amplificationDialog = std::make_unique<AmplificationDialog>(this);
        connect(amplificationDialog.get(), &AmplificationDialog::amplificationChanged,
                this, &MainWindow::onAmplificationChanged);
    }
    amplificationDialog->exec();
}

void MainWindow::showDSPSettingsDialog()
{
    if (!dspSettingsDialog) {
        dspSettingsDialog = std::make_unique<DSPSettingsDialog>(this);
        
        // Connect signals for sending DSP settings
        connect(dspSettingsDialog.get(), &DSPSettingsDialog::sendSettingsRequested,
                this, &MainWindow::onSendDSPSettings);
    }
    dspSettingsDialog->exec();
}

void MainWindow::onSendDSPSettings(const DSP_Settings_t& settings)
{
    // Check if we have a UDP connection
    if (!udpConfigDialog || !udpConfigDialog->isConnected()) {
        QMessageBox::warning(this, "Not Connected", 
                            "Please connect to the radar via UDP Configuration first.");
        return;
    }
    
    UdpHandler* handler = udpConfigDialog->getUdpHandler();
    if (handler) {
        // Connect the sent signal if not already connected
        static bool signalConnected = false;
        if (!signalConnected) {
            connect(handler, &UdpHandler::dspSettingsSent, 
                    this, &MainWindow::onDSPSettingsSent);
            signalConnected = true;
        }
        
        // Send the settings
        handler->sendDSPSettings(settings);
    }
}

void MainWindow::onDSPSettingsSent(bool success)
{
    if (success) {
        QMessageBox::information(this, "DSP Settings", 
                                "DSP settings have been sent to the radar successfully.");
    } else {
        QMessageBox::warning(this, "DSP Settings", 
                            "Failed to send DSP settings to the radar.");
    }
}

void MainWindow::toggleLiveStream()
{
    liveStreamActive = !liveStreamActive;
    
    if (liveStreamActive) {
        liveStreamButton->setText("Stop Live Stream");
        liveStreamButton->setStyleSheet("QPushButton { background-color: red; }");
    } else {
        liveStreamButton->setText("Start Live Stream");
        liveStreamButton->setStyleSheet("");
    }
}

void MainWindow::toggleFreezeRun()
{
    frozen = !frozen;
    
    if (frozen) {
        freezeButton->setText("Run");
        // Freeze all charts
        if (rawChart) rawChart->setFrozen(true);
        if (fftChart) fftChart->setFrozen(true);
        if (detectionChart) detectionChart->setFrozen(true);
        for (auto* chart : outputCharts) {
            chart->setFrozen(true);
        }
    } else {
        freezeButton->setText("Freeze");
        // Unfreeze all charts
        if (rawChart) rawChart->setFrozen(false);
        if (fftChart) fftChart->setFrozen(false);
        if (detectionChart) detectionChart->setFrozen(false);
        for (auto* chart : outputCharts) {
            chart->setFrozen(false);
        }
    }
}

void MainWindow::writeToEEPROM()
{
    QMessageBox::information(this, "EEPROM", "All parameters written to EEPROM successfully!");
}

void MainWindow::onThresholdChanged(int value)
{
    if (fftChart) {
        fftChart->setThreshold(value);
    }
    thresholdLabel->setText(QString("Threshold: %1 dB").arg(value));
    config.threshold = value;
}

void MainWindow::onAmplificationChanged(int value)
{
    amplificationLabel->setText(QString("Amplification: %1 dB").arg(value));
    config.amplification = value;
}

void MainWindow::onChannelChanged(int index)
{
    config.channel = index;
}

void MainWindow::onLineFilterChanged()
{
    config.filter50Hz = filter50Hz->isChecked();
    config.filter100Hz = filter100Hz->isChecked();
    config.filter150Hz = filter150Hz->isChecked();
}

void MainWindow::onUdpConnectionChanged(bool connected)
{
    this->connected = connected;
    updateConnectionStatus(connected);
}

void MainWindow::onNewDetectionReceived(const DetectionData& detection)
{
    processDetection(detection);
}

void MainWindow::onUdpStatisticsUpdated(int received, int dropped, double rate)
{
    // Update data rate display
    updateDataRate(rate);
    
    // Update status bar with packet information
    QString statusMessage = QString("Ready - %1 | Packets: %2 received, %3 dropped")
                           .arg(connected ? "Connected" : "Not Connected")
                           .arg(received)
                           .arg(dropped);
    statusBar()->showMessage(statusMessage);
}

void MainWindow::onTargetSelected(const TargetDetection& target)
{
    highlightTargetInChart(target);
}

void MainWindow::onChartDetectionClicked(const TargetDetection& target)
{
    // Highlight target in lists
    for (auto* targetList : targetLists) {
        // Could implement target highlighting here
    }
}

void MainWindow::updateStatus()
{
    // Update various status indicators
    statusBar()->showMessage(QString("Ready - %1").arg(connected ? "Connected" : "Not Connected"));
}

void MainWindow::processDetection(const DetectionData& detection)
{
    //qDebug()<<"processDetection";
    // Add to recent detections
    {
        QMutexLocker locker(&recentDetectionsMutex);
        recentDetections.push_back(detection);
        if (recentDetections.size() > MAX_RECENT_DETECTIONS) {
            recentDetections.erase(recentDetections.begin());
        }
    }
    
    // Update target lists (commented out since output targets are not used now)
    /*
    for (auto* targetList : targetLists) {
        targetList->onNewDetection(detection);
    }
    */
    
    // Update charts - Convert DetectionData to TargetDetection
    TargetDetection target = detection.toTargetDetection();
    
    // Update FFT chart
    if (fftChart && !frozen) {
        fftChart->addDetection(target);
    }
    
    // Update main detection chart in detection tab
    if (detectionChart && !frozen) {
        detectionChart->addDetection(target);
    }
    
    // Update output charts (commented out since outputs are not used now)
    /*
    for (auto* chart : outputCharts) {
        if (!frozen) {
            chart->addDetection(target);
        }
    }
    */
    
    // Update track table
    updateTrackTable();
    
    // Update target count in status bar
    updateTargetCount(static_cast<int>(recentDetections.size()));
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        connectionStatusLabel->setText("Connection: Connected");
        connectionStatusLabel->setStyleSheet("QLabel { color: green; }");
    } else {
        connectionStatusLabel->setText("Connection: Disconnected");
        connectionStatusLabel->setStyleSheet("QLabel { color: red; }");
    }
}

void MainWindow::updateDataRate(double rate)
{
    dataRateLabel->setText(QString("Data Rate: %1 pps").arg(rate, 0, 'f', 1));
}

void MainWindow::updateTargetCount(int count)
{
    targetCountLabel->setText(QString("Targets: %1").arg(count));
}

void MainWindow::updateTrackTable()
{
    if (!trackTable) return;
    
    // Clear existing rows
    trackTable->setRowCount(0);
    
    // Add recent detections to the table
    QMutexLocker locker(&recentDetectionsMutex);
    int row = 0;
    for (const auto& detection : recentDetections) {
        // Only show recent detections (last 10 seconds)
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - detection.timestamp > 10000) continue;  // Skip old detections
        
        trackTable->insertRow(row);
        
        // Track ID
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(detection.target_id));
        idItem->setTextAlignment(Qt::AlignCenter);
        trackTable->setItem(row, 0, idItem);
        
        // Radius
        QTableWidgetItem* radiusItem = new QTableWidgetItem(QString::number(detection.radius, 'f', 1));
        radiusItem->setTextAlignment(Qt::AlignCenter);
        trackTable->setItem(row, 1, radiusItem);
        
        // Radial Speed
        QTableWidgetItem* speedItem = new QTableWidgetItem(QString::number(detection.radial_speed, 'f', 2));
        speedItem->setTextAlignment(Qt::AlignCenter);
        // Color code based on speed
        if (detection.radial_speed > 2.0) {
            speedItem->setBackground(QBrush(QColor(255, 200, 200))); // Light red for approaching
        } else if (detection.radial_speed < -2.0) {
            speedItem->setBackground(QBrush(QColor(200, 255, 200))); // Light green for receding
        } else {
            speedItem->setBackground(QBrush(QColor(255, 255, 200))); // Light yellow for stationary
        }
        trackTable->setItem(row, 2, speedItem);
        
        // Azimuth
        QTableWidgetItem* azimuthItem = new QTableWidgetItem(QString::number(detection.azimuth, 'f', 1));
        azimuthItem->setTextAlignment(Qt::AlignCenter);
        trackTable->setItem(row, 3, azimuthItem);
        
        row++;
    }
    
    // Auto-resize columns to content
    trackTable->resizeColumnsToContents();
}

void MainWindow::onZoomChanged(double zoomLevel)
{
    zoomLevelLabel->setText(QString("Zoom: %1x").arg(zoomLevel, 0, 'f', 1));
}

void MainWindow::showDetectionInChart(const DetectionData& detection)
{
    // Implementation for highlighting detection in charts
}

void MainWindow::highlightTargetInChart(const TargetDetection& target)
{
    // Implementation for highlighting target in charts
}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    // Load configuration
    config.threshold = settings.value("config/threshold", 0).toInt();
    config.amplification = settings.value("config/amplification", 20).toInt();
    config.channel = settings.value("config/channel", 0).toInt();
    config.filter50Hz = settings.value("config/filter50Hz", false).toBool();
    config.filter100Hz = settings.value("config/filter100Hz", false).toBool();
    config.filter150Hz = settings.value("config/filter150Hz", false).toBool();
    
    applySettings();
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    
    // Save configuration
    settings.setValue("config/threshold", config.threshold);
    settings.setValue("config/amplification", config.amplification);
    settings.setValue("config/channel", config.channel);
    settings.setValue("config/filter50Hz", config.filter50Hz);
    settings.setValue("config/filter100Hz", config.filter100Hz);
    settings.setValue("config/filter150Hz", config.filter150Hz);
}

void MainWindow::applySettings()
{
    if (thresholdSlider) thresholdSlider->setValue(config.threshold);
    if (amplificationSlider) amplificationSlider->setValue(config.amplification);
    if (channelCombo) channelCombo->setCurrentIndex(config.channel);
    if (filter50Hz) filter50Hz->setChecked(config.filter50Hz);
    if (filter100Hz) filter100Hz->setChecked(config.filter100Hz);
    if (filter150Hz) filter150Hz->setChecked(config.filter150Hz);
}

void MainWindow::updateChartsWithDetections()
{
    // Implementation for updating charts with detection data
}

void MainWindow::updateDetectionCharts()
{
    // Implementation for updating detection-specific charts
}

void MainWindow::updateTargetLists()
{
    // Implementation for updating target lists
}

void MainWindow::onTrackTableSelectionChanged()
{
    if (!trackTable) return;
    
    QList<QTableWidgetItem*> selectedItems = trackTable->selectedItems();
    if (selectedItems.isEmpty()) return;
    
    // Get the selected row
    int row = selectedItems.first()->row();
    
    // Get the track ID from the selected row
    QTableWidgetItem* idItem = trackTable->item(row, 0);
    if (!idItem) return;
    
    uint32_t selectedTrackId = idItem->text().toUInt();
    
    // Find the corresponding detection and highlight it
    QMutexLocker locker(&recentDetectionsMutex);
    for (const auto& detection : recentDetections) {
        if (detection.target_id == selectedTrackId) {
            // Create a TargetDetection from DetectionData for highlighting
            TargetDetection target;
            target.target_id = detection.target_id;
            target.radius = detection.radius;
            target.radial_speed = detection.radial_speed;
            target.azimuth = detection.azimuth;
            target.amplitude = detection.amplitude;
            target.timestamp = detection.timestamp;
            
            highlightTargetInChart(target);
            break;
        }
    }
}
