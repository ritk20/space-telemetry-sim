// #include <thread>

// #include "../include/buffer.h"
// #include "sensors.cpp"
// #include "transmitter.cpp"

// int main()
// {
//   TelemetryBuffer buffer(100);

//   std::thread sensor(sensor_thread, std::ref(buffer));
//   std::thread transmitter(transmitter_thread, std::ref(buffer));

//   sensor.join();
//   transmitter.join();
// }