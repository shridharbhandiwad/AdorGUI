# Raw Data Test Sender

This is a test application to verify the raw data reception in the iSys4001 GUI.

## Building

### Using Make (Linux/macOS)

```bash
cd test_sender
make
```

### Using CMake

```bash
cd test_sender
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./test_raw_data_sender [options]
```

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `-h, --host <ip>` | Destination IP address | 127.0.0.1 |
| `-p, --port <port>` | Destination UDP port | 5000 |
| `-n, --frames <num>` | Number of frames to send (0=infinite) | 100 |
| `-d, --delay <ms>` | Delay between frames in milliseconds | 100 |
| `-c, --chirps <num>` | Number of chirps per frame | 64 |
| `-r, --rx <num>` | Number of RX antennas | 4 |
| `-s, --samples <num>` | Samples per chirp | 256 |
| `--range <meters>` | Simulated target range | 10.0 |
| `--speed <m/s>` | Simulated target speed | 5.0 |
| `--noise <level>` | Noise amplitude | 0.1 |

### Examples

1. Send 100 frames to localhost on port 5000 (default):
   ```bash
   ./test_raw_data_sender
   ```

2. Send frames to a specific host:
   ```bash
   ./test_raw_data_sender -h 192.168.1.100 -p 5000
   ```

3. Send frames continuously with 50ms delay:
   ```bash
   ./test_raw_data_sender -n 0 -d 50
   ```

4. Use custom radar parameters:
   ```bash
   ./test_raw_data_sender -c 128 -r 2 -s 512
   ```

## Data Format

The sender transmits data in the following binary format:

### Header (RawDataHeader_t)

| Field | Type | Description |
|-------|------|-------------|
| message_type | uint32_t | Always 0x01 for raw data |
| frame_number | uint32_t | Incrementing frame counter |
| num_chirps | uint32_t | Number of chirps in frame |
| num_rx_antennas | uint8_t | Number of RX antennas |
| num_samples_per_chirp | uint32_t | Samples per chirp |
| rx_mask | uint8_t | Bit mask of enabled antennas |
| adc_resolution | uint8_t | ADC resolution in bits |
| interleaved_rx | uint8_t | 1 if data is interleaved |
| data_format | uint32_t | Data format type |

### Sample Data

After the header, the packet contains `num_chirps * num_rx_antennas * num_samples_per_chirp` float values.

## Testing Procedure

1. Start the iSys4001 GUI application
2. Configure UDP reception (port 5000)
3. Start live streaming in the GUI
4. Run the test sender:
   ```bash
   ./test_raw_data_sender -n 100 -d 100
   ```
5. Verify that frames are received in the GUI
