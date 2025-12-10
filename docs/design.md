## Simulation Design
### 1. Sensor Model
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

### 2. Simulation Loop 

The main simulation runs continuously in a background thread:
- The loop ensures one packet per second, mimicking onboard telemetry rates.
- `dt` is computed using `steady_clock` → unaffected by wall-clock jumps or system time changes.

### 3. Dual Clock System
This uses two distinct clocks — one for simulation / internal timing, another for mission timestamps.
This provides real-world Unix timestamps in each telemetry packet while preserving a stable simulation rate

In real spacecraft systems, time synchronization between onboard systems and ground stations is crucial.
This project mimics that by separating simulation time (steady-clock) from mission timestamping (system-clock Unix time).

- `steady_clock` ensures that simulation physics are unaffected by changes in system time, NTP updates, or drift.
- `system_clock` provides an accurate real-world Unix timestamp embedded in telemetry packets, allowing the ground station to reconstruct when each packet was generated.

This dual-clock model mirrors real satellite architectures, where:

- Mission Elapsed Time (MET) corresponds to `steady_clock`, and
- UTC timestamp corresponds to `system_clock`

### 4. Compression

#### Serialization & Deserialization
We used raw binary copy using memcpy as our struct is a plain old data type. It is simple and extremely fast, but is not portable for systems with different endianness or alignment.

It can be further modified to manual field-by-field serialization if we want cross-platform safety (it's slower and verbose). We can also use some serialization frameworks for real systems.

### 5. TCP
Telemetry in real spacecraft systems is usually transmitted over RF links, which are unreliable. In our simulation on a computer, we emulate this with network sockets. TCP is chosen because:

- Reliable delivery: TCP ensures all bytes reach the receiver and in the correct order.
- Stream-oriented: You can send a continuous stream of bytes (good for periodic telemetry).
- Built-in flow control: Prevents the sender from overwhelming the receiver.

Alternative: UDP is faster but unreliable. It’s sometimes used in real-time telemetry where missing a packet occasionally is acceptable, but for your simulation, TCP keeps things simpler and deterministic.

