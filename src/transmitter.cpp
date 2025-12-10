#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/buffer.h"
#include "../include/compression.h"

void transmitter_thread(TelemetryBuffer &buffer)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("socket");
    return;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(5000);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("connect");
    close(sock);
    return;
  }

  while (!buffer.is_shutdown())
  {
    TelemetryPacket pkt = buffer.pop();

    if (pkt.timestamp == 0)
      break;

    std::vector<uint8_t> bytes = serialise(pkt);
    bytes = compress(bytes);

    uint32_t len = htonl(bytes.size());
    send(sock, &len, sizeof(len), 0);
    send(sock, bytes.data(), bytes.size(), 0);

    std::cout << "[Transmitter] Sent packet with timestamp " << pkt.timestamp
              << " (" << bytes.size() << " bytes)\n";
  }
  close(sock);
}
