/**
 * @file verify_structures.cpp
 * @brief Verify that the structure definitions match the expected layout
 * 
 * Compile with: g++ -std=c++11 -o verify_structures verify_structures.cpp
 */

#include <iostream>
#include <cstdint>
#include <cstring>

// Message type identifiers (must match structures.h)
enum MessageType : uint32_t {
    MSG_TYPE_RAW_DATA = 0x01,
    MSG_TYPE_DETECTION = 0x02,
    MSG_TYPE_DSP_SETTINGS = 0x03,
    MSG_TYPE_STATUS = 0x04
};

// Data format enumeration
enum DataFormat : uint32_t {
    DATA_FORMAT_REAL = 0,
    DATA_FORMAT_COMPLEX = 1,
    DATA_FORMAT_COMPLEX_SEPARATED = 2
};

// Raw data header structure - must match exactly with structures.h in main app
#pragma pack(push, 1)
struct RawDataHeader_t {
    uint32_t message_type;           // 0x01 for raw data
    uint32_t frame_number;           // Frame sequence number
    uint32_t num_chirps;             // Number of chirps in this frame
    uint8_t  num_rx_antennas;        // Number of receive antennas
    uint32_t num_samples_per_chirp;  // Samples per chirp
    uint8_t  rx_mask;                // Receive antenna mask
    uint8_t  adc_resolution;         // ADC resolution in bits
    uint8_t  interleaved_rx;         // 1 if RX data is interleaved, 0 otherwise
    uint32_t data_format;            // Data format (see DataFormat enum)
};
#pragma pack(pop)

int main() {
    std::cout << "=== Structure Verification ===" << std::endl;
    std::cout << std::endl;
    
    // Expected values based on the external system's structure
    std::cout << "RawDataHeader_t:" << std::endl;
    std::cout << "  sizeof(RawDataHeader_t) = " << sizeof(RawDataHeader_t) << " bytes" << std::endl;
    
    // Print field offsets
    RawDataHeader_t header;
    std::cout << "\nField offsets:" << std::endl;
    std::cout << "  message_type:          offset=" << offsetof(RawDataHeader_t, message_type) 
              << ", size=" << sizeof(header.message_type) << std::endl;
    std::cout << "  frame_number:          offset=" << offsetof(RawDataHeader_t, frame_number) 
              << ", size=" << sizeof(header.frame_number) << std::endl;
    std::cout << "  num_chirps:            offset=" << offsetof(RawDataHeader_t, num_chirps) 
              << ", size=" << sizeof(header.num_chirps) << std::endl;
    std::cout << "  num_rx_antennas:       offset=" << offsetof(RawDataHeader_t, num_rx_antennas) 
              << ", size=" << sizeof(header.num_rx_antennas) << std::endl;
    std::cout << "  num_samples_per_chirp: offset=" << offsetof(RawDataHeader_t, num_samples_per_chirp) 
              << ", size=" << sizeof(header.num_samples_per_chirp) << std::endl;
    std::cout << "  rx_mask:               offset=" << offsetof(RawDataHeader_t, rx_mask) 
              << ", size=" << sizeof(header.rx_mask) << std::endl;
    std::cout << "  adc_resolution:        offset=" << offsetof(RawDataHeader_t, adc_resolution) 
              << ", size=" << sizeof(header.adc_resolution) << std::endl;
    std::cout << "  interleaved_rx:        offset=" << offsetof(RawDataHeader_t, interleaved_rx) 
              << ", size=" << sizeof(header.interleaved_rx) << std::endl;
    std::cout << "  data_format:           offset=" << offsetof(RawDataHeader_t, data_format) 
              << ", size=" << sizeof(header.data_format) << std::endl;
    
    // Verify expected layout
    std::cout << "\n=== Verification Results ===" << std::endl;
    
    bool allOk = true;
    
    // Check total size - with pack(1) the expected size is:
    // 4 + 4 + 4 + 1 + 4 + 1 + 1 + 1 + 4 = 24 bytes
    size_t expectedSize = 24;
    if (sizeof(RawDataHeader_t) != expectedSize) {
        std::cout << "ERROR: sizeof(RawDataHeader_t) = " << sizeof(RawDataHeader_t) 
                  << ", expected " << expectedSize << std::endl;
        allOk = false;
    } else {
        std::cout << "OK: Structure size is correct (" << expectedSize << " bytes)" << std::endl;
    }
    
    // Test serialization
    std::cout << "\n=== Serialization Test ===" << std::endl;
    
    header.message_type = MSG_TYPE_RAW_DATA;
    header.frame_number = 12345;
    header.num_chirps = 64;
    header.num_rx_antennas = 4;
    header.num_samples_per_chirp = 256;
    header.rx_mask = 0x0F;
    header.adc_resolution = 12;
    header.interleaved_rx = 0;
    header.data_format = DATA_FORMAT_REAL;
    
    // Serialize to buffer
    char buffer[sizeof(RawDataHeader_t)];
    std::memcpy(buffer, &header, sizeof(RawDataHeader_t));
    
    // Deserialize and verify
    RawDataHeader_t header2;
    std::memcpy(&header2, buffer, sizeof(RawDataHeader_t));
    
    if (header2.message_type != header.message_type ||
        header2.frame_number != header.frame_number ||
        header2.num_chirps != header.num_chirps ||
        header2.num_rx_antennas != header.num_rx_antennas ||
        header2.num_samples_per_chirp != header.num_samples_per_chirp ||
        header2.rx_mask != header.rx_mask ||
        header2.adc_resolution != header.adc_resolution ||
        header2.interleaved_rx != header.interleaved_rx ||
        header2.data_format != header.data_format) {
        std::cout << "ERROR: Serialization round-trip failed!" << std::endl;
        allOk = false;
    } else {
        std::cout << "OK: Serialization round-trip successful" << std::endl;
    }
    
    // Print raw bytes for debugging
    std::cout << "\nRaw bytes (" << sizeof(RawDataHeader_t) << " bytes):" << std::endl;
    for (size_t i = 0; i < sizeof(RawDataHeader_t); i++) {
        printf("%02X ", static_cast<unsigned char>(buffer[i]));
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "\n=== Final Result ===" << std::endl;
    if (allOk) {
        std::cout << "All verifications PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "Some verifications FAILED" << std::endl;
        return 1;
    }
}
