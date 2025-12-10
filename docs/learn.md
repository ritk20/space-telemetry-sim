## Stuff Learnt

### TCP in C++ with C++ BSD Socket

- When a socket is created, the program has to specify the **address domain** and the **socket type**. Two processes can communicate with each other only if their sockets are of the same type and in the same domain.
  - 2 address domain - Unix (two processes which share a common file system communicate) and Internet (two processes running on any two hosts on the Internet communicate)


AF_INET - IPv4 family
SOCK_STREAM - TCP (connection-based stream)
0 - use default protocol (TCP in this case)

sockaddr_in is a struct that holds IP + port info.
htons(5000) converts port 5000 from host byte order → network byte order 
inet_pton() converts a human-readable IP ("127.0.0.1") into a binary representation that the OS can understand.
