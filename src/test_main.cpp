#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "sensors.cpp"

int main()
{
  TelemetrySimulator sim;
  for (int i = 0; i < 5; ++i)
  {
    TelemetryPacket p = sim.generate_packet(1.0);
    std::cout << "tick=" << p.timestamp
              << " temp=" << std::fixed << std::setprecision(2) << p.temperature
              << " rad=" << p.radiation
              << " batt=" << p.battery_voltage
              << " pos=(" << p.position[0] << "," << p.position[1] << "," << p.position[2] << ")"
              << " orient=(" << p.orientation[0] << "," << p.orientation[1] << "," << p.orientation[2] << ")"
              << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}