# space-telemetry-sim 
This is a C++ simulation of a spacecraft telemetry system.

It models onboard sensors generating real-time data, buffered transmission over TCP, and ground-station reception with compression and logging.

The project (hopefully) demonstrates multithreading, data serialization, socket communication, and systems-level design inspired by real satellite telemetry architectures.

## Architecture
1. Sensors - Generate stateful data like temperature, radiation, battery voltage, position, orientation in the form of `TelemetryPacket`.
2. Buffer - `TelemetryPacket` are pushed to the buffer which implements circular thread-safe producer/consumer operation.
3. Transmitter - Takes the front packet from the buffer and serialises, compresses and sends the file over a TCP connection.
4. Ground Station - Receives and decompresses and logs the relevent data.

## Key Features

- Multi-threaded producer–consumer design
- Data compression using `zlib`
- Binary serialization for efficient transmission
- C++ BSD Socket implementation of TCP protocol

## Folder Structure

```
space-telemetry-sim/
├── src/
│   ├── sensors.cpp
│   ├── buffer.cpp
│   ├── transmitter.cpp
│   ├── ground_station.cpp
│   ├── compression.cpp
│   ├── main.cpp
│   └── test_main.cpp
├── include/
│   ├── telemetry.h
│   ├── buffer.h
│   └── compression.h
├── logs/
│   └── telemetry_log.csv
├── CMakeLists.txt
├── README.md
└── docs/
    ├── design.md
    └── architecture.png
```
## Log File Format
```
timestamp,temperature,radiation,pos_x,pos_y,pos_z,pitch,roll,yaw,battery
1733400001,28.5,0.12,7000,1200,340,-0.5,0.1,1.2,11.8
1733400002,28.6,0.11,7000,1200,341,-0.5,0.1,1.3,11.8
```
## Build

```
mkdir build
cd build
cmake ..
make

# Now you can run either:
./sim        # The main simulation
./test_sim   # The test suite
```

## Issues
1. cout can be out-of-order due to both transmitter and ground_station threads calling std::cout simultaneously. Need to be fixed by global mutex

## Future Scope

- Configurable parameters via CLI (interval, compression, ports) (To be added very soon)
- System Metrics
- Adding encryption
- Async I/O
- Add UDP + packet loss simulation
- Integrate protobuf serialization
- Add web dashboard for live telemetry

## References
- [Temperature Sensing and Measurement in Satellites and Spacecraft](https://www.te.com/en/industries/space/applications/temperature-sensing-measurement-satellites-spacecraft.html#:~:text=Integration%20and%20Placement%20of%20Temperature,general%20information%20about%20selection%20criteria.)
- [Timekeeping for the Space Technology 5 (ST-5) Mission](https://ntrs.nasa.gov/api/citations/20030025281/downloads/20030025281.pdf)

- [TM Space Data Link Protocol](https://ccsds.org/Pubs/132x0b3.pdf)

- [Linux Sockets Tutorial](https://www.linuxhowtos.org/C_C++/socket.htm)
