cmake_minimum_required(VERSION 3.16)

project(ZopplerRadarGUI VERSION 1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Set Qt specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(SOURCES
    main_app.cpp
    mainwindow.cpp
    customchart.cpp
    udphandler.cpp
    dialogs.cpp
    targetlist.cpp
)

# Header files
set(HEADERS
    structures.h
    mainwindow.h
    customchart.h
    udphandler.h
    dialogs.h
    targetlist.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install targets
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
