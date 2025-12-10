/* * Comprehensive Test Suite for Space Telemetry Sim
 * Includes:
 * 1. Unit Tests for Buffer Logic
 * 2. Unit Tests for Serialization & Compression
 * 3. Concurrency Tests for Thread Safety
 * 4. Integration Test
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

// Include project headers
#include "../include/telemetry.h"
#include "../include/buffer.h"
#include "../include/compression.h"

// --- Helper Macros for Testing ---
#define ASSERT_TRUE(condition, message)                                                             \
  do                                                                                                \
  {                                                                                                 \
    if (!(condition))                                                                               \
    {                                                                                               \
      std::cerr << "[\033[1;31mFAILED\033[0m] " << message << " at line " << __LINE__ << std::endl; \
      std::exit(1);                                                                                 \
    }                                                                                               \
  } while (0)

#define ASSERT_EQUAL(a, b, message)                                                                                 \
  do                                                                                                                \
  {                                                                                                                 \
    if ((a) != (b))                                                                                                 \
    {                                                                                                               \
      std::cerr << "[\033[1;31mFAILED\033[0m] " << message << ": Expected " << (a) << ", got " << (b) << std::endl; \
      std::exit(1);                                                                                                 \
    }                                                                                                               \
  } while (0)

#define LOG_TEST(name) \
  std::cout << "[\033[1;32mRUNNING\033[0m] " << name << "..." << std::endl

#define PASS_TEST()                          \
  std::cout << "[\033[1;32mPASSED\033[0m]\n" \
            << std::endl

void sensor_thread(TelemetryBuffer &);
void transmitter_thread(TelemetryBuffer &);
void ground_station_thread();

// --- Helper Functions ---
bool float_eq(float a, float b, float epsilon = 0.001f)
{
  return std::abs(a - b) < epsilon;
}

bool compare_packets(const TelemetryPacket &a, const TelemetryPacket &b)
{
  if (a.timestamp != b.timestamp)
    return false;
  if (!float_eq(a.temperature, b.temperature))
    return false;
  if (!float_eq(a.radiation, b.radiation))
    return false;
  if (!float_eq(a.battery_voltage, b.battery_voltage))
    return false;

  for (int i = 0; i < 3; ++i)
  {
    if (!float_eq(a.position[i], b.position[i]))
      return false;
    if (!float_eq(a.orientation[i], b.orientation[i]))
      return false;
  }
  return true;
}

// --- Test Cases ---

void test_serialization()
{
  LOG_TEST("Serialization & Deserialization");

  TelemetryPacket original{};
  original.timestamp = 123456789;
  original.temperature = 25.5f;
  original.radiation = 0.05f;
  original.battery_voltage = 12.0f;
  original.position = {100.0f, 200.0f, 300.0f};
  original.orientation = {0.1f, 0.2f, 0.3f};

  std::vector<uint8_t> data = serialise(original);

  ASSERT_EQUAL(data.size(), sizeof(TelemetryPacket), "Serialized size mismatch");

  TelemetryPacket deserialized = deserialise(data);

  ASSERT_TRUE(compare_packets(original, deserialized), "Deserialized packet does not match original");

  PASS_TEST();
}

void test_compression()
{
  LOG_TEST("Compression Logic (zlib)");

  std::vector<uint8_t> original(1000, 67);

  std::vector<uint8_t> compressed = compress_data(original);

  // Zlib should reduce size significantly
  ASSERT_TRUE(compressed.size() < original.size(), "Compression did not reduce size");

  std::vector<uint8_t> decompressed = decompress_data(compressed, original.size());

  ASSERT_EQUAL(decompressed.size(), original.size(), "Decompressed size mismatch");
  ASSERT_TRUE(decompressed == original, "Decompressed data content mismatch");

  PASS_TEST();
}

void test_buffer_behavior()
{
  LOG_TEST("TelemetryBuffer Basic Behavior");

  size_t capacity = 5;
  TelemetryBuffer buffer(capacity);

  ASSERT_EQUAL(buffer.size(), 0, "Buffer should be empty initially");

  for (size_t i = 0; i < capacity; ++i)
  {
    TelemetryPacket pkt{};
    pkt.timestamp = i;
    buffer.push(pkt);
  }

  ASSERT_EQUAL(buffer.size(), capacity, "Buffer should be full");

  TelemetryPacket pkt = buffer.pop();
  ASSERT_EQUAL(pkt.timestamp, 0, "First popped item incorrect");
  ASSERT_EQUAL(buffer.size(), capacity - 1, "Buffer size incorrect after pop");

  TelemetryPacket new_pkt{};
  new_pkt.timestamp = 99;
  buffer.push(new_pkt); // This should wrap around to the front internal index

  ASSERT_EQUAL(buffer.size(), capacity, "Buffer should be full again");

  PASS_TEST();
}

void test_buffer_concurrency()
{
  LOG_TEST("TelemetryBuffer Thread Safety (Producer-Consumer)");

  TelemetryBuffer buffer(10);
  const int NUM_ITEMS = 1000;

  std::vector<uint64_t> received_timestamps;

  std::thread consumer([&]()
                       {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            TelemetryPacket pkt = buffer.pop();
            received_timestamps.push_back(pkt.timestamp);
        } });

  std::thread producer([&]()
                       {
        for (int i = 0; i < NUM_ITEMS; ++i) {
            TelemetryPacket pkt{};
            pkt.timestamp = static_cast<uint64_t>(i);
            buffer.push(pkt);
        } });

  producer.join();
  consumer.join();

  ASSERT_EQUAL(received_timestamps.size(), NUM_ITEMS, "Lost packets during concurrent access");

  for (int i = 0; i < NUM_ITEMS; ++i)
  {
    ASSERT_EQUAL(received_timestamps[i], (uint64_t)i, "Packets received out of order");
  }

  PASS_TEST();
}

void test_full_system_integration()
{
  LOG_TEST("Full System Integration (Sensors -> TX -> RX)");

  TelemetryBuffer buffer(100);

  std::thread ground_station(ground_station_thread);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::thread sensor(sensor_thread, std::ref(buffer));
  std::thread transmitter(transmitter_thread, std::ref(buffer));

  std::cout << "  > Simulating for 5 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));

  std::cout << "  > Shutting down systems..." << std::endl;
  buffer.shutdown();

  sensor.join();
  transmitter.join();
  ground_station.join();

  PASS_TEST();
}

int main()
{
  std::cout << "======================================\n";
  std::cout << "   SPACE TELEMETRY SIM - TEST SUITE   \n";
  std::cout << "======================================\n\n";

  test_serialization();
  test_compression();
  test_buffer_behavior();
  test_buffer_concurrency();
  test_full_system_integration();

  std::cout << "All tests passed successfully!" << std::endl;
  return 0;
}