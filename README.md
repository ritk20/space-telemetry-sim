# space-telemetry-sim 
This is a C++ simulation of a spacecraft telemetry system.

It models onboard sensors generating real-time data, buffered transmission over TCP, and ground-station reception with compression and logging.

The project will (hopefully) demonstrate multithreading, data serialization, socket communication, and systems-level design inspired by real satellite telemetry architectures (by the time it's done).

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

## Testing

For now, a simple `test_main.cpp` is implemented which can be run using:
```
g++ -std=c++17 src/test_main.cpp -Iinclude -pthread -O2 -o bin/test_sim
./bin/test_sim
```
This will simulate the 5 packets for now

## References
- [Temperature Sensing and Measurement in Satellites and Spacecraft](https://www.te.com/en/industries/space/applications/temperature-sensing-measurement-satellites-spacecraft.html#:~:text=Integration%20and%20Placement%20of%20Temperature,general%20information%20about%20selection%20criteria.)
- [Timekeeping for the Space Technology 5 (ST-5) Mission](https://ntrs.nasa.gov/api/citations/20030025281/downloads/20030025281.pdf)
