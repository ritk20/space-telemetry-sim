#include <iostream>

#include "../include/buffer.h"

void transmitter_thread(TelemetryBuffer &buffer)
{
  while (true)
  {
    TelemetryPacket pkt = buffer.pop();

    // buffer.pop() returns a default-initialized TelemetryPacket when shutdown + empty.
    // TelemetrySimulator timestamps start at 1, so timestamp == 0 indicates the shutdown sentinel.
    if (pkt.timestamp == 0)
      break;

    // print a short summary for testing
    std::cout << "RX tick=" << pkt.timestamp
              << " temp=" << pkt.temperature
              << " rad=" << pkt.radiation
              << " batt=" << pkt.battery_voltage << "\n";
  }
}
