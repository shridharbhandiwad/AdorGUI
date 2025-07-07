#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Zoppler TM Radar GUI");
    app.setApplicationVersion("1.1");
    app.setOrganizationName("Zoppler Systems");
    app.setOrganizationDomain("zopplerlabs.com");
    
    // Set application style
    app.setStyle("Fusion");
    
    // Set up application-wide styling for a rich look with lighter colors
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(240, 248, 255));        // Alice blue background
    lightPalette.setColor(QPalette::WindowText, QColor(25, 25, 112));      // Midnight blue text
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));          // White base
    lightPalette.setColor(QPalette::AlternateBase, QColor(230, 230, 250)); // Lavender alternate
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));   // Light yellow tooltip
    lightPalette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));         // Black tooltip text
    lightPalette.setColor(QPalette::Text, QColor(25, 25, 112));            // Midnight blue text
    lightPalette.setColor(QPalette::Button, QColor(220, 220, 220));        // Light gray button
    lightPalette.setColor(QPalette::ButtonText, QColor(25, 25, 112));      // Midnight blue button text
    lightPalette.setColor(QPalette::BrightText, QColor(220, 20, 60));      // Crimson bright text
    lightPalette.setColor(QPalette::Link, QColor(65, 105, 225));           // Royal blue link
    lightPalette.setColor(QPalette::Highlight, QColor(135, 206, 250));     // Sky blue highlight
    lightPalette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));     // Black highlighted text
    app.setPalette(lightPalette);
    
    //qDebug() << "Starting Zoppler Radar Sensor GUI v1.1";
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}