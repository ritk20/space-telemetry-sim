#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

#include "telemetry.h"

class TelemetryBuffer
{
private:
  std::vector<TelemetryPacket> buffer_;
  size_t front, back;
  std::mutex mtx_;
  std::condition_variable cv_full_;
  std::condition_variable cv_empty_;
  const size_t capacity_;
  std::atomic<bool> stop_ = false;

  bool isEmpty() const;
  bool isFull() const;

public:
  explicit TelemetryBuffer(size_t capacity = 100);
  void push(const TelemetryPacket &pkt);
  TelemetryPacket pop();
  size_t size();
  void shutdown();
};