#include <vector>
#include <cstdint>
#include <cstring>
#include <zlib.h>
#include <stdexcept>

#include "../include/telemetry.h"

std::vector<uint8_t> serialise(const TelemetryPacket &pkt)
{
  std::vector<uint8_t> data(sizeof(pkt));
  std::memcpy(data.data(), &pkt, sizeof(pkt));
  return data;
}

TelemetryPacket deserialise(const std::vector<uint8_t> data)
{
  TelemetryPacket pkt{};
  std::memcpy(&pkt, data.data(), sizeof(pkt));
  return pkt;
}

std::vector<uint8_t> compress(const std::vector<uint8_t> &input)
{
  uLongf out_size = compressBound(input.size());
  std::vector<uint8_t> output(out_size);

  if (compress(output.data(), &out_size, input.data(), input.size()) != Z_OK)
    throw std::runtime_error("Compression failed");

  output.resize(out_size);
  return output;
}

std::vector<uint8_t> decompress(const std::vector<uint8_t> &input, size_t expected_size)
{
  std::vector<uint8_t> output(expected_size);

  if (uncompress(output.data(), &expected_size, input.data(), input.size()) != Z_OK)
    throw std::runtime_error("Decompression failed");

  return output;
}