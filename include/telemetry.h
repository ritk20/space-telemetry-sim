#pragma once
#include <array>
#include <cstdint>

struct TelemetryPacket
{
  uint64_t timestamp; // seconds since start of simulation
  float temperature;
  float radiation;
  float battery_voltage;
  std::array<float, 3> position;
  std::array<float, 3> orientation;
};
