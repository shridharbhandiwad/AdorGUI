/**
 * @file test_raw_data_sender.cpp
 * @brief Test application to send raw radar data frames via UDP
 * 
 * This application simulates the external radar system sending raw frame data
 * in the exact format expected by the iSys4001 GUI application.
 * 
 * Usage: ./test_raw_data_sender [host] [port] [num_frames] [delay_ms]
 *   host       - Destination IP address (default: 127.0.0.1)
 *   port       - Destination UDP port (default: 5000)
 *   num_frames - Number of frames to send (default: 100, 0 = infinite)
 *   delay_ms   - Delay between frames in milliseconds (default: 100)
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

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

// Raw data header structure - must match exactly with structures.h
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

// Configuration for test data generation
struct TestConfig {
    std::string host = "127.0.0.1";
    int port = 5000;
    int num_frames = 100;       // 0 = infinite
    int delay_ms = 100;
    
    // Radar parameters
    uint32_t num_chirps = 64;
    uint8_t num_rx_antennas = 4;
    uint32_t num_samples_per_chirp = 256;
    uint8_t rx_mask = 0x0F;     // All 4 antennas enabled
    uint8_t adc_resolution = 12;
    uint8_t interleaved_rx = 0;
    uint32_t data_format = DATA_FORMAT_REAL;
    
    // Simulated target parameters
    float target_range = 10.0f;      // meters
    float target_speed = 5.0f;       // m/s
    float noise_level = 0.1f;        // noise amplitude
};

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "\nOptions:\n"
              << "  -h, --host <ip>       Destination IP address (default: 127.0.0.1)\n"
              << "  -p, --port <port>     Destination UDP port (default: 5000)\n"
              << "  -n, --frames <num>    Number of frames to send (default: 100, 0=infinite)\n"
              << "  -d, --delay <ms>      Delay between frames in ms (default: 100)\n"
              << "  -c, --chirps <num>    Number of chirps per frame (default: 64)\n"
              << "  -r, --rx <num>        Number of RX antennas (default: 4)\n"
              << "  -s, --samples <num>   Samples per chirp (default: 256)\n"
              << "  --range <meters>      Simulated target range (default: 10.0)\n"
              << "  --speed <m/s>         Simulated target speed (default: 5.0)\n"
              << "  --noise <level>       Noise amplitude (default: 0.1)\n"
              << "  --help                Show this help message\n"
              << "\nExample:\n"
              << "  " << progName << " -h 192.168.1.100 -p 5000 -n 1000 -d 50\n";
}

bool parseArgs(int argc, char* argv[], TestConfig& config) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return false;
        }
        else if ((arg == "-h" || arg == "--host") && i + 1 < argc) {
            config.host = argv[++i];
        }
        else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
            config.port = std::atoi(argv[++i]);
        }
        else if ((arg == "-n" || arg == "--frames") && i + 1 < argc) {
            config.num_frames = std::atoi(argv[++i]);
        }
        else if ((arg == "-d" || arg == "--delay") && i + 1 < argc) {
            config.delay_ms = std::atoi(argv[++i]);
        }
        else if ((arg == "-c" || arg == "--chirps") && i + 1 < argc) {
            config.num_chirps = std::atoi(argv[++i]);
        }
        else if ((arg == "-r" || arg == "--rx") && i + 1 < argc) {
            config.num_rx_antennas = std::atoi(argv[++i]);
        }
        else if ((arg == "-s" || arg == "--samples") && i + 1 < argc) {
            config.num_samples_per_chirp = std::atoi(argv[++i]);
        }
        else if (arg == "--range" && i + 1 < argc) {
            config.target_range = std::atof(argv[++i]);
        }
        else if (arg == "--speed" && i + 1 < argc) {
            config.target_speed = std::atof(argv[++i]);
        }
        else if (arg == "--noise" && i + 1 < argc) {
            config.noise_level = std::atof(argv[++i]);
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return false;
        }
    }
    return true;
}

// Generate simulated radar sample data
void generateSampleData(std::vector<float>& samples, const TestConfig& config, uint32_t frameNumber) {
    uint32_t totalSamples = config.num_samples_per_chirp * config.num_chirps * config.num_rx_antennas;
    samples.resize(totalSamples);
    
    // Radar parameters for simulation
    const float c = 3e8f;           // Speed of light
    const float fc = 24.125e9f;     // Carrier frequency (24 GHz)
    const float bandwidth = 200e6f; // Bandwidth (200 MHz)
    const float T_chirp = 50e-6f;   // Chirp duration (50 us)
    
    // Calculate beat frequency for target
    float beat_freq = 2.0f * config.target_range * bandwidth / (c * T_chirp);
    
    // Doppler shift due to target speed
    float doppler = 2.0f * config.target_speed * fc / c;
    
    // Time varying phase offset
    float phase_offset = static_cast<float>(frameNumber) * 0.1f;
    
    for (uint32_t chirp = 0; chirp < config.num_chirps; chirp++) {
        float chirp_phase = static_cast<float>(chirp) * doppler * T_chirp * 2.0f * M_PI;
        
        for (uint8_t rx = 0; rx < config.num_rx_antennas; rx++) {
            // Add antenna-specific phase shift (for array processing)
            float antenna_phase = static_cast<float>(rx) * M_PI * 0.5f;
            
            for (uint32_t sample = 0; sample < config.num_samples_per_chirp; sample++) {
                float t = static_cast<float>(sample) / static_cast<float>(config.num_samples_per_chirp);
                
                // Generate beat signal
                float signal = std::sin(2.0f * M_PI * beat_freq * t + chirp_phase + antenna_phase + phase_offset);
                
                // Add some noise
                float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * config.noise_level;
                
                // Calculate index based on interleaving
                uint32_t idx;
                if (config.interleaved_rx) {
                    // Interleaved: [chirp][sample][antenna]
                    idx = (chirp * config.num_samples_per_chirp * config.num_rx_antennas) + 
                          (sample * config.num_rx_antennas) + rx;
                } else {
                    // Non-interleaved: [chirp][antenna][sample]
                    idx = (chirp * config.num_rx_antennas * config.num_samples_per_chirp) +
                          (rx * config.num_samples_per_chirp) + sample;
                }
                
                samples[idx] = signal + noise;
            }
        }
    }
}

// Build and send a raw data frame
bool sendRawDataFrame(SOCKET sock, const sockaddr_in& destAddr, const TestConfig& config, uint32_t frameNumber) {
    // Build header
    RawDataHeader_t header;
    header.message_type = MSG_TYPE_RAW_DATA;
    header.frame_number = frameNumber;
    header.num_chirps = config.num_chirps;
    header.num_rx_antennas = config.num_rx_antennas;
    header.num_samples_per_chirp = config.num_samples_per_chirp;
    header.rx_mask = config.rx_mask;
    header.adc_resolution = config.adc_resolution;
    header.interleaved_rx = config.interleaved_rx;
    header.data_format = config.data_format;
    
    // Generate sample data
    std::vector<float> samples;
    generateSampleData(samples, config, frameNumber);
    
    // Build packet
    uint32_t totalSamples = config.num_samples_per_chirp * config.num_chirps * config.num_rx_antennas;
    uint32_t dataSize = totalSamples * sizeof(float);
    uint32_t totalSize = sizeof(RawDataHeader_t) + dataSize;
    
    std::vector<char> buffer(totalSize);
    std::memcpy(buffer.data(), &header, sizeof(RawDataHeader_t));
    std::memcpy(buffer.data() + sizeof(RawDataHeader_t), samples.data(), dataSize);
    
    // Send packet
    int bytesSent = sendto(sock, buffer.data(), totalSize, 0,
                          reinterpret_cast<const sockaddr*>(&destAddr), sizeof(destAddr));
    
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Failed to send packet" << std::endl;
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    TestConfig config;
    
    if (!parseArgs(argc, argv, config)) {
        return 1;
    }
    
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
#endif
    
    // Create UDP socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }
    
    // Setup destination address
    sockaddr_in destAddr;
    std::memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(config.port);
    
    if (inet_pton(AF_INET, config.host.c_str(), &destAddr.sin_addr) != 1) {
        std::cerr << "Invalid destination address: " << config.host << std::endl;
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return 1;
    }
    
    // Print configuration
    std::cout << "=== Raw Data Sender Test Application ===" << std::endl;
    std::cout << "Destination: " << config.host << ":" << config.port << std::endl;
    std::cout << "Frames to send: " << (config.num_frames == 0 ? "infinite" : std::to_string(config.num_frames)) << std::endl;
    std::cout << "Delay between frames: " << config.delay_ms << " ms" << std::endl;
    std::cout << "\nRadar Configuration:" << std::endl;
    std::cout << "  Chirps per frame: " << config.num_chirps << std::endl;
    std::cout << "  RX antennas: " << static_cast<int>(config.num_rx_antennas) << std::endl;
    std::cout << "  Samples per chirp: " << config.num_samples_per_chirp << std::endl;
    std::cout << "  ADC resolution: " << static_cast<int>(config.adc_resolution) << " bits" << std::endl;
    std::cout << "\nSimulated Target:" << std::endl;
    std::cout << "  Range: " << config.target_range << " m" << std::endl;
    std::cout << "  Speed: " << config.target_speed << " m/s" << std::endl;
    std::cout << "\nHeader size: " << sizeof(RawDataHeader_t) << " bytes" << std::endl;
    
    uint32_t totalSamples = config.num_samples_per_chirp * config.num_chirps * config.num_rx_antennas;
    uint32_t dataSize = totalSamples * sizeof(float);
    std::cout << "Data size per frame: " << dataSize << " bytes (" << totalSamples << " samples)" << std::endl;
    std::cout << "Total packet size: " << (sizeof(RawDataHeader_t) + dataSize) << " bytes" << std::endl;
    std::cout << "\nSending frames..." << std::endl;
    
    // Seed random number generator
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Send frames
    uint32_t frameNumber = 0;
    uint32_t framesSent = 0;
    auto startTime = std::chrono::steady_clock::now();
    
    while (config.num_frames == 0 || framesSent < static_cast<uint32_t>(config.num_frames)) {
        if (sendRawDataFrame(sock, destAddr, config, frameNumber)) {
            framesSent++;
            
            // Print progress every 10 frames
            if (framesSent % 10 == 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
                double fps = (elapsed > 0) ? (framesSent * 1000.0 / elapsed) : 0.0;
                
                std::cout << "\rFrames sent: " << framesSent;
                if (config.num_frames > 0) {
                    std::cout << "/" << config.num_frames;
                }
                std::cout << " (Frame #" << frameNumber << ", " << fps << " fps)    " << std::flush;
            }
        }
        
        frameNumber++;
        
        // Delay between frames
        if (config.delay_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.delay_ms));
        }
    }
    
    std::cout << std::endl;
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Total frames sent: " << framesSent << std::endl;
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Average frame rate: " << (totalTime > 0 ? (framesSent * 1000.0 / totalTime) : 0.0) << " fps" << std::endl;
    std::cout << "Total data sent: " << (framesSent * (sizeof(RawDataHeader_t) + dataSize)) << " bytes" << std::endl;
    
    // Cleanup
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    
    return 0;
}
