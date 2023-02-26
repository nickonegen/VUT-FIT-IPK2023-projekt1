// IPKCP Client header file

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// IMPORTANT: REMOVE IOMANIP BEFORE SUBMISSION
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

#define OP_REQUEST (u_int8_t)0
#define OP_RESPONSE (u_int8_t)1
#define STATUS_OK (u_int8_t)0
#define STATUS_ERROR (u_int8_t)1

#define MAX_PAYLOAD_LEN 255
#define BUFFER_SIZE (MAX_PAYLOAD_LEN + 3)
#define TIMEOUT 4  // in seconds

class IPKCPClient {
   public:
	IPKCPClient(int port, std::string hostname, int protocol);
	IPKCPClient& operator=(IPKCPClient&& other) = default;
	IPKCPClient& operator=(const IPKCPClient&) = default;
	IPKCPClient(IPKCPClient&& other) = default;
	IPKCPClient(const IPKCPClient&) = default;
	~IPKCPClient();

	bool connect();
	ssize_t send(const std::string& input) {
		return (this->protocol == SOCK_STREAM) ? this->send_tcp(input)
									    : this->send_udp(input);
	}
	std::string recv() {
		return (this->protocol == SOCK_STREAM) ? this->recv_tcp()
									    : this->recv_udp();
	}

   private:
	int fd;
	int port;
	int protocol;
	std::string hostname;
	struct hostent* host;
	struct sockaddr_in addr {};
	socklen_t addr_len = sizeof(addr);
	ssize_t send_tcp(const std::string& input);
	ssize_t send_udp(const std::string& input);
	std::string recv_tcp();
	std::string recv_udp();
};

#endif  // CLIENT_HPP
