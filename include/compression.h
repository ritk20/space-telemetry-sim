#pragma once

std::vector<uint8_t> serialise(const TelemetryPacket &pkt);
TelemetryPacket deserialise(const std::vector<uint8_t> data);
std::vector<uint8_t> compress(const std::vector<uint8_t> &input);
std::vector<uint8_t> decompress(const std::vector<uint8_t> &input, size_t expected_size);