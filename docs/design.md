## Simulation Design
1. Sensor Model
Each sensor is represented by a stateful class implementing:

```
class Sensor {
  public:
    virtual void update(double dt) = 0;
    virtual ~Sensor() = default;
};
```


Sensors follow physically plausible models:

- Temperature: gradual drift + Gaussian noise.
- Radiation: base level increases with altitude.
- Position: circular orbit based on angular velocity.
- Orientation: small random attitude changes per tick.
- Battery: slow voltage decay with noise.

2. Simulation Loop 

The main simulation runs continuously in a background thread:
- The loop ensures one packet per second, mimicking onboard telemetry rates.
- `dt` is computed using `steady_clock` → unaffected by wall-clock jumps or system time changes.

3. Dual Clock System
This uses two distinct clocks — one for simulation / internal timing, another for mission timestamps.
This provides real-world Unix timestamps in each telemetry packet while preserving a stable simulation rate

In real spacecraft systems, time synchronization between onboard systems and ground stations is crucial.
This project mimics that by separating simulation time (steady-clock) from mission timestamping (system-clock Unix time).

- `steady_clock` ensures that simulation physics are unaffected by changes in system time, NTP updates, or drift.
- `system_clock` provides an accurate real-world Unix timestamp embedded in telemetry packets, allowing the ground station to reconstruct when each packet was generated.

This dual-clock model mirrors real satellite architectures, where:

- Mission Elapsed Time (MET) corresponds to `steady_clock`, and
- UTC timestamp corresponds to `system_clock`