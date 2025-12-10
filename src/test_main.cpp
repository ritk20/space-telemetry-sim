#include <thread>
#include <chrono>

#include "../include/buffer.h"

void sensor_thread(TelemetryBuffer &);
void transmitter_thread(TelemetryBuffer &);
void ground_station_thread();

int main()
{
  TelemetryBuffer buffer(100);

  std::thread ground_station(ground_station_thread);
  std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for server ready

  std::thread sensor(sensor_thread, std::ref(buffer));
  std::thread transmitter(transmitter_thread, std::ref(buffer));

  // run for 10 seconds then shut down
  std::this_thread::sleep_for(std::chrono::seconds(10));
  buffer.shutdown();

  sensor.join();
  transmitter.join();
  ground_station.join();
  return 0;
}