#include <cstdint>
#include <vector>
#include <array>
#include <stddef.h>

#include "telemetry.h"

class TelemetryBuffer
{
  uint8_t producer;
  uint8_t consumer;
  const size_t size;
  std::vector<TelemetryPacket> buffer;

public:
  TelemetryBuffer(size_t sz) : size(sz), buffer(sz), producer(0), consumer(-1) {}
  void push(TelemetryPacket packet)
  {
    buffer[producer] = packet;
    if (consumer == -1)
      consumer = 0;
    producer = (producer + 1) % size;
  }
  void pop()
  {
    consumer = (consumer + 1) % size;
  }
};