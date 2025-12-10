#include <thread>
#include <iostream>
#include <vector>
#include "../include/buffer.h"

// Forward declarations of the thread functions defined in other files
void sensor_thread(TelemetryBuffer &buffer);
void transmitter_thread(TelemetryBuffer &buffer);
void ground_station_thread();

int main()
{
  std::cout << "Starting Space Telemetry Simulation..." << std::endl;

  TelemetryBuffer buffer(100);

  std::thread ground_station(ground_station_thread);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::thread sensor(sensor_thread, std::ref(buffer));
  std::thread transmitter(transmitter_thread, std::ref(buffer));

  sensor.join();
  transmitter.join();
  ground_station.join();

  return 0;
}