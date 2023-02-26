// IPKCP Client header file

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

typedef int8_t opcode_t;
typedef int8_t status_t;

#define OP_REQUEST (opcode_t)0
#define OP_RESPONSE (opcode_t)1
#define STATUS_OK (status_t)0
#define STATUS_ERROR (status_t)1

#define BUFFER_SIZE (int)512
#define TIMEOUT (int)4  // in seconds

class IPKCPClient {
   public:
	IPKCPClient(int port, std::string hostname, int protocol);
	IPKCPClient& operator=(IPKCPClient&& other) = default;
	IPKCPClient& operator=(const IPKCPClient&) = default;
	IPKCPClient(IPKCPClient&& other) = default;
	IPKCPClient(const IPKCPClient&) = default;
	~IPKCPClient();

	bool connect();
	ssize_t send(char* buffer) {
		return (this->protocol == SOCK_STREAM) ? this->send_tcp(buffer)
									    : this->send_udp(buffer);
	}
	ssize_t recv(char* buffer) {
		return (this->protocol == SOCK_STREAM) ? this->recv_tcp(buffer)
									    : this->recv_udp(buffer);
	}

   private:
	int fd;
	int port;
	int protocol;
	std::string hostname;
	struct hostent* host;
	struct sockaddr_in addr {};
	socklen_t addr_len = sizeof(addr);
	ssize_t send_tcp(char* buffer) const;
	ssize_t send_udp(char* buffer);
	ssize_t recv_tcp(char* buffer) const;
	ssize_t recv_udp(char* buffer);
};

#endif  // CLIENT_HPP
