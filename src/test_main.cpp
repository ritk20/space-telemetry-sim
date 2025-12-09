#include <thread>
#include <chrono>

#include "../include/buffer.h"

void sensor_thread(TelemetryBuffer &);
void transmitter_thread(TelemetryBuffer &);

int main()
{
  TelemetryBuffer buffer(100);

  std::thread sensor(sensor_thread, std::ref(buffer));
  std::thread transmitter(transmitter_thread, std::ref(buffer));

  // run for 5 seconds then shut down
  std::this_thread::sleep_for(std::chrono::seconds(5));
  buffer.shutdown();

  sensor.join();
  transmitter.join();
  return 0;
}