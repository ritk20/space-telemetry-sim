#include <cstdint>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

#include "../include/telemetry.h"
#include "../include/buffer.h"

// capacity_ = N + 1 is used to distinguish full vs empty states using front and back
// Effective capacity is actually capacity_ - 1
TelemetryBuffer::TelemetryBuffer(size_t capacity) : capacity_(capacity + 1), buffer_(capacity + 1), front(0), back(0) {}

bool TelemetryBuffer::isEmpty() const
{
  return front == back;
}

bool TelemetryBuffer::isFull() const
{
  return (back + 1) % capacity_ == front;
}

void TelemetryBuffer::push(const TelemetryPacket &pkt)
{
  std::unique_lock<std::mutex> lock(mtx_);

  cv_full_.wait(lock, [this]
                { return stop_ || !isFull(); });

  if (stop_)
    return;

  buffer_[back] = pkt;
  back = (back + 1) % capacity_;

  cv_empty_.notify_one();
}

TelemetryPacket TelemetryBuffer::pop()
{
  std::unique_lock<std::mutex> lock(mtx_);
  cv_empty_.wait(lock, [this]
                 { return stop_ || !isEmpty(); });

  if (stop_ && isEmpty())
    return TelemetryPacket{};

  TelemetryPacket prev_pkt = buffer_[front];
  front = (front + 1) % capacity_;

  cv_full_.notify_one();
  return prev_pkt;
}

void TelemetryBuffer::shutdown()
{
  stop_ = true;
  cv_full_.notify_all();
  cv_empty_.notify_all();

  std::cout << "Shutting Down" << std::endl;
}

size_t TelemetryBuffer::size()
{
  std::scoped_lock lock(mtx_);
  if (back >= front)
    return back - front;
  return capacity_ - (front - back);
}
