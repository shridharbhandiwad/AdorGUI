QT += core widgets network

TARGET = iSys4001_GUI
CONFIG += c++17

# Application icon
# win32:RC_ICONS = app.ico

# Header files
HEADERS += \
    structures.h \
    isys4001_gui.h \
    mainwindow.h \
    customchart.h \
    dialogs.h \
    targetlist.h \
    udphandler.h

# Source files
SOURCES += \
    main_app.cpp \
    mainwindow_basic.cpp \
    customchart.cpp \
    dialogs.cpp \
    targetlist.cpp \
    udphandler.cpp

# Resources
RESOURCES += resources.qrc

# Other files for distribution
OTHER_FILES += styles.qss

# Windows-specific configuration
win32 {
    LIBS += -lws2_32 -lwsock32
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    
    # Static linking for easier deployment
    CONFIG += static
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
    
    # For MinGW static linking
    contains(QMAKE_HOST.arch, x86_64) {
        QMAKE_LFLAGS += -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
    }
}

# Linux-specific configuration
linux {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    
    # Install configuration
    target.path = /usr/local/bin
    INSTALLS += target
}

# macOS-specific configuration
macx {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
    
    # Bundle configuration
    QMAKE_INFO_PLIST = Info.plist
    ICON = app.icns
}

# Release configuration
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -O2
    DEFINES += QT_NO_DEBUG_OUTPUT
    
    # Strip symbols on Linux/macOS
    unix:!macx {
        QMAKE_POST_LINK += strip $(TARGET)
    }
}

# Debug configuration
CONFIG(debug, debug|release) {
    DEFINES += QT_DEBUG
    QMAKE_CXXFLAGS += -g
}

# MOC settings for Qt's meta-object system
QMAKE_MOC_OPTIONS += -nn

# Clean target
QMAKE_CLEAN += $(TARGET)

# Additional clean files
QMAKE_DISTCLEAN += $(TARGET) Makefile*

# Deployment folder
DEPLOYMENT_FOLDER = deploy

# Custom deployment targets
win32 {
    deploy.commands = \
        mkdir -p $$DEPLOYMENT_FOLDER && \
        cp $(DESTDIR_TARGET) $$DEPLOYMENT_FOLDER/ && \
        windeployqt --release --no-translations --no-system-d3d-compiler $$DEPLOYMENT_FOLDER/$$TARGET.exe
}

linux {
    deploy.commands = \
        mkdir -p $$DEPLOYMENT_FOLDER && \
        cp $(TARGET) $$DEPLOYMENT_FOLDER/
}

macx {
    deploy.commands = \
        mkdir -p $$DEPLOYMENT_FOLDER && \
        cp -r $(TARGET).app $$DEPLOYMENT_FOLDER/ && \
        macdeployqt $$DEPLOYMENT_FOLDER/$(TARGET).app
}

QMAKE_EXTRA_TARGETS += deploy
