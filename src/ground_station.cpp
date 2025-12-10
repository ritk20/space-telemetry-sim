#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/buffer.h"
#include "../include/compression.h"
#include "../include/logger.h"

void ground_station_thread()
{
  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0)
  {
    perror("socket");
    return;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = INADDR_ANY; // accept from any interface

  if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)))
  {
    perror("bind");
    return;
  }

  listen(listen_sock, 1); // only 1 connection is queued

  std::cout << "[Ground Station] Waiting for connection...\n";

  int client_sock = accept(listen_sock, nullptr, nullptr);

  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::string filename = "telemetry_" + std::to_string(t) + ".csv";
  TelemetryLogger logger(filename);

  if (client_sock < 0)
  {
    perror("accept");
    return;
  }
  std::cout << "[Ground Station] Connected to transmitter.\n";

  while (true)
  {
    uint32_t len_network;
    ssize_t n = recv(client_sock, &len_network, sizeof(len_network), 0);

    if (n <= 0) // transmitter disconnected
      break;

    uint32_t len = ntohl(len_network);

    std::vector<uint8_t> buffer(len);
    size_t received = 0;
    while (received < len)
    {
      ssize_t chunk = recv(client_sock, buffer.data() + received, len - received, 0);
      if (chunk <= 0)
        break;
      received += chunk;
    }

    buffer = decompress_data(buffer, sizeof(TelemetryPacket));
    TelemetryPacket pkt = deserialise(buffer);

    std::cout << "[Ground Station] Packet received:"
              << " Temp=" << pkt.temperature
              << "  Volt=" << pkt.battery_voltage
              << "  Rad=" << pkt.radiation
              << "  Time=" << pkt.timestamp
              << std::endl;
    logger.log_packet(pkt);
  }

  close(client_sock);
  close(listen_sock);
  std::cout << "[Ground Station] Closed.\n";
}