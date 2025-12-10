#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include "../include/telemetry.h"

class TelemetryLogger
{
private:
  std::ofstream file_;

public:
  explicit TelemetryLogger(const std::string &filename)
  {
    namespace fs = std::filesystem;

    fs::path log_dir = "logs";
    if (!fs::exists(log_dir))
      fs::create_directory(log_dir);

    fs::path filepath = log_dir / filename;
    file_.open(filepath, std::ios::app);
    if (!file_.is_open())
      throw std::runtime_error("Failed to open log file.");

    if (file_.tellp() == 0)
    {
      file_ << "timestamp,temperature,radiation,"
               "pos_x,pos_y,pos_z,"
               "pitch,roll,yaw,battery\n";
    }
  }

  void log_packet(const TelemetryPacket &pkt)
  {
    file_ << pkt.timestamp << ","
          << pkt.temperature << ","
          << pkt.radiation << ","
          << pkt.position[0] << ","
          << pkt.position[1] << ","
          << pkt.position[2] << ","
          << pkt.orientation[0] << ","
          << pkt.orientation[1] << ","
          << pkt.orientation[2] << ","
          << pkt.battery_voltage << "\n";
    file_.flush(); // ensure immediate write (safe but slightly slower)
  }

  ~TelemetryLogger()
  {
    if (file_.is_open())
      file_.close();
  }
};
