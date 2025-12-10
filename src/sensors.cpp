#include <cstdint>
#include <random>
#include <array>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <thread>

#include "../include/buffer.h"

static std::mt19937 rng(std::random_device{}());

class Sensor
{
public:
  virtual void update(double dt) = 0;
  virtual ~Sensor() = default;
};

class TemperatureSensor : public Sensor
{
  float temperature;
  std::normal_distribution<float> noise;
  float cooling_rate;

public:
  TemperatureSensor(float init = 25.0f, float c_rate = -0.005f) : temperature(init), noise(0.0f, 0.05f), cooling_rate(c_rate) {}

  void update(double dt) override
  {
    float new_temp = temperature + cooling_rate * dt + noise(rng);
    temperature = std::clamp(new_temp, -50.0f, 80.0f);
  }

  float value() const { return temperature; }
};

class RadiationSensor : public Sensor
{
  float radiation;
  std::normal_distribution<float> noise;

public:
  RadiationSensor(float init = 0.1f)
      : radiation(init), noise(0.0f, 0.002f) {}

  void update(double z) override
  {
    float base = 0.05f + 0.0001f * z;
    radiation = std::max(0.0f, base + noise(rng));
  }

  float value() const { return radiation; }
};

class BatterySensor : public Sensor
{
  float voltage;
  std::normal_distribution<float> noise;
  float discharge_rate;

public:
  BatterySensor(float init = 12.5f, float rate = 0.0001f)
      : voltage(init), noise(0.0f, 0.002f), discharge_rate(rate) {}

  void update(double dt) override
  {
    float new_v = voltage - discharge_rate * dt + noise(rng);
    voltage = std::clamp(new_v, 9.0f, 12.6f);
  }

  float value() const { return voltage; }
};

class PositionSensor : public Sensor
{
  float x, y, z;
  const float R = 7000.0f;                // orbit radius (km)
  const float omega = 2 * M_PI / 5400.0f; // one orbit per 90 min
  double elapsed_time = 0.0;

public:
  PositionSensor() : x(R), y(0.0f), z(0.0f) {}

  void update(double dt) override
  {
    elapsed_time += dt;
    x = R * std::cos(omega * elapsed_time);
    y = R * std::sin(omega * elapsed_time);
    z = 0.0f;
  }

  std::array<float, 3> value() const { return {x, y, z}; }
};

class OrientationSensor : public Sensor
{
  float pitch, roll, yaw;
  std::normal_distribution<float> noise_pitch;
  std::normal_distribution<float> noise_roll;
  std::normal_distribution<float> noise_yaw;

  static float wrap(float angle)
  {
    return std::fmod(angle + 180.0f, 360.0f) - 180.0f;
  }

public:
  OrientationSensor()
      : pitch(0), roll(0), yaw(0),
        noise_pitch(0.0f, 0.01f),
        noise_roll(0.0f, 0.01f),
        noise_yaw(0.0f, 0.02f) {}

  void update(double dt) override
  {
    pitch = wrap(pitch + noise_pitch(rng));
    roll = wrap(roll + noise_roll(rng));
    yaw = wrap(yaw + noise_yaw(rng));
  }

  std::array<float, 3> value() const { return {pitch, roll, yaw}; }
};

class TelemetrySimulator
{
  TemperatureSensor temp_sensor;
  RadiationSensor radiation_sensor;
  BatterySensor battery_sensor;
  PositionSensor position_sensor;
  OrientationSensor orientation_sensor;

  uint64_t tick = 0;

public:
  TelemetryPacket generate_packet(double dt)
  {
    tick++;

    temp_sensor.update(dt);
    position_sensor.update(dt);
    radiation_sensor.update(position_sensor.value()[2]);
    orientation_sensor.update(dt);
    battery_sensor.update(dt);

    TelemetryPacket pkt;
    pkt.timestamp = tick;
    pkt.temperature = temp_sensor.value();
    pkt.radiation = radiation_sensor.value();
    pkt.battery_voltage = battery_sensor.value();
    pkt.position = position_sensor.value();
    pkt.orientation = orientation_sensor.value();

    return pkt;
  }
};

void sensor_thread(TelemetryBuffer &buffer)
{
  TelemetrySimulator sim;
  auto last = std::chrono::steady_clock::now();

  while (!buffer.is_shutdown())
  {
    auto now = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(now - last).count();
    last = now;

    TelemetryPacket pkt = sim.generate_packet(dt);
    buffer.push(pkt);

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
