# Code Modifications Summary

## Overview
This document summarizes the modifications made to the Zoppler Radar Sensor GUI according to the user requirements:

1. Comment out output1, output2, output3 (not required as of now)
2. Keep full size for azimuth vs Range plot and create a track table showing trackId, Radius, Radial Speed, Azimuth in the same detection tab
3. Keep background color of the UI rich look with lighter colors

## 1. Output Tabs Commented Out

### Files Modified: `mainwindow_basic.cpp`

**Changes Made:**
- Commented out the creation of Output 1, 2, 3 tabs in the `setupUI()` method
- Commented out the right side target information panel that displayed output target lists
- Simplified the layout to use only the left widget (charts and controls) without the splitter

**Code Changes:**
```cpp
// Output tabs
// COMMENTED OUT - Output tabs not required as of now
/*
for (int i = 1; i <= 3; ++i) {
    QWidget* outputWidget = createOutputTab(i);
    mainTabs->addTab(outputWidget, QString("Output %1").arg(i));
}
*/

// Right side target information commented out
// Target lists for each output commented out in setupConnections()
```

## 2. Detection Tab Redesign

### Files Modified: `mainwindow_basic.cpp`, `mainwindow.h`

**Changes Made:**
- **Full-size Azimuth vs Range Plot**: Modified `createDetectionTab()` to use horizontal layout
- **Added Track Table**: Created a new table widget showing:
  - Track ID
  - Radius (m)
  - Radial Speed (m/s) 
  - Azimuth (°)
- **Improved Layout**: 
  - Charts on the left (3/4 width)
  - Controls and track table on the right (1/4 width)
  - FFT chart limited to 150px height to give more space to detection chart

**New Features:**
- Color-coded radial speed in track table:
  - Light red: Approaching (speed > 2.0 m/s)
  - Light green: Receding (speed < -2.0 m/s)
  - Light yellow: Stationary (speed between -2.0 and 2.0 m/s)
- Auto-resizing columns
- Row selection highlights corresponding target in chart
- Shows only recent detections (last 10 seconds)

**New Files/Members:**
- Added `QTableWidget* trackTable` to MainWindow
- Added `QMutex recentDetectionsMutex` for thread safety
- Added `updateTrackTable()` method
- Added `onTrackTableSelectionChanged()` slot

## 3. Rich Light Color Theme

### Files Modified: `main_app.cpp`, `customchart.cpp`, `targetlist.cpp`

**Application-wide Color Scheme:**
```cpp
// New light palette with rich colors
QPalette lightPalette;
lightPalette.setColor(QPalette::Window, QColor(240, 248, 255));        // Alice blue background
lightPalette.setColor(QPalette::WindowText, QColor(25, 25, 112));      // Midnight blue text
lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));          // White base
lightPalette.setColor(QPalette::AlternateBase, QColor(230, 230, 250)); // Lavender alternate
lightPalette.setColor(QPalette::Button, QColor(220, 220, 220));        // Light gray button
lightPalette.setColor(QPalette::Highlight, QColor(135, 206, 250));     // Sky blue highlight
```

**Chart Color Updates:**
- **Background**: Light blue gradient (245,250,255) to (230,240,250)
- **Grid**: Cornflower blue (100,149,237) dotted lines
- **Axes**: Midnight blue (25,25,112) solid lines
- **Text/Labels**: Midnight blue (25,25,112)
- **FFT Signal**: Forest green (34,139,34)
- **Threshold Line**: Dark orange (255,140,0)
- **Raw Signal**: Deep sky blue (0,191,255)

**Status Button Colors:**
- **No Object**: Moccasin background (#FFE4B5) with saddle brown text (#8B4513)
- **Approaching**: Light pink (#FFB6C1) with dark red text (#8B0000)
- **Receding**: Pale green (#98FB98) with dark green text (#006400)
- **Stationary**: Light yellow (#FFFFE0) with dark goldenrod text (#B8860B)

## 4. Additional Improvements

### Thread Safety
- Added mutex protection for `recentDetections` vector
- Safe access to detection data from multiple threads

### UI Responsiveness
- Track table updates in real-time with new detections
- Auto-cleanup of old detections (>10 seconds)
- Proper column sizing and sorting

### Code Organization
- Commented out unused output-related code
- Maintained backward compatibility
- Clean separation of concerns

## 5. Files Modified Summary

1. **mainwindow_basic.cpp**: Major UI restructuring, track table implementation
2. **mainwindow.h**: Added new members and method declarations
3. **main_app.cpp**: Application-wide color theme update
4. **customchart.cpp**: Chart color scheme updates
5. **targetlist.cpp**: Status button color updates

## 6. Build Requirements

The project requires:
- Qt6 development libraries
- CMake or qmake build system
- C++ compiler with C++11 support

## 7. Testing Recommendations

1. **Track Table Functionality**:
   - Verify table updates with new detections
   - Test row selection highlighting in chart
   - Check color coding of radial speeds

2. **UI Layout**:
   - Confirm full-size azimuth vs range plot
   - Verify responsive layout on different window sizes
   - Test zoom controls functionality

3. **Color Theme**:
   - Verify readability with new light theme
   - Check chart visibility and contrast
   - Test status button color changes

## 8. Future Enhancements

1. **Track Table Features**:
   - Add filtering/sorting options
   - Export track data to CSV
   - Configurable time window for displayed tracks

2. **Chart Improvements**:
   - Adjustable color schemes
   - Enhanced target highlighting
   - Chart zoom synchronization with table selection

3. **Performance**:
   - Optimize table updates for high-frequency data
   - Memory management for large datasets

---

## 9. Raw Data Reception Structure (New)

### Overview
Added support for receiving raw radar frame data from an external system via UDP. The implementation matches the exact binary format specified by the external radar system.

### Files Modified

1. **structures.h**
   - Added `RawDataHeader_t` packed structure matching external system format
   - Added `RawFrameData` structure for storing complete frames with samples
   - Added `MessageType` enum for packet type identification
   - Added `DataFormat` enum for sample data format types
   - Added helper methods for calculating data sizes and accessing samples

2. **udphandler.h**
   - Added raw frame data signals (`rawFrameReceived`, `rawDataUpdated`)
   - Added raw frame storage with thread-safe access
   - Added methods for accessing raw frame data (`getLatestRawFrame`, `getRecentRawFrames`, etc.)
   - Added binary data parsing declarations

3. **udphandler.cpp**
   - Implemented binary packet detection and routing (`parseBinaryData`)
   - Implemented raw frame parsing (`parseRawFrameData`)
   - Implemented frame validation (`isValidRawFrame`)
   - Added raw frame storage management with configurable buffer size
   - Updated packet reception to try binary parsing first, then fall back to text formats

4. **CMakeLists.txt**
   - Fixed source file name (`mainwindow_basic.cpp` instead of `mainwindow.cpp`)

### New Data Structures

#### RawDataHeader_t (24 bytes, packed)
```cpp
struct RawDataHeader_t {
    uint32_t message_type;           // 0x01 for raw data
    uint32_t frame_number;           // Frame sequence number
    uint32_t num_chirps;             // Number of chirps in this frame
    uint8_t  num_rx_antennas;        // Number of receive antennas
    uint32_t num_samples_per_chirp;  // Samples per chirp
    uint8_t  rx_mask;                // Receive antenna mask
    uint8_t  adc_resolution;         // ADC resolution in bits
    uint8_t  interleaved_rx;         // 1 if RX data is interleaved
    uint32_t data_format;            // Data format type
};
```

### Test Sender Application

Created a test application in `test_sender/` directory:

1. **test_raw_data_sender.cpp** - Standalone UDP sender that simulates external radar system
2. **verify_structures.cpp** - Verifies structure layout matches expected format
3. **CMakeLists.txt** and **Makefile** - Build files
4. **README.md** - Usage documentation

#### Building the Test Sender
```bash
cd test_sender
make
```

#### Running the Test Sender
```bash
./test_raw_data_sender -h 127.0.0.1 -p 5000 -n 100 -d 100
```

### Testing Procedure

1. Start the iSys4001 GUI application
2. Open UDP Configuration dialog and set port to 5000
3. Connect to start receiving
4. Run the test sender to send simulated radar frames
5. Verify reception in the GUI (check packet statistics)

### API Usage for Raw Frame Data

```cpp
// Access latest frame
RawFrameData frame = udpHandler->getLatestRawFrame();
if (frame.isValid()) {
    qDebug() << "Frame" << frame.header.frame_number;
    qDebug() << "Chirps:" << frame.header.num_chirps;
    qDebug() << "Samples:" << frame.samples.size();
}

// Connect to raw frame signal
connect(udpHandler, &UdpHandler::rawFrameReceived, 
        [](const RawFrameData& frame) {
    // Process frame data
});
```

---

*All modifications maintain backward compatibility and can be easily reverted if needed.*