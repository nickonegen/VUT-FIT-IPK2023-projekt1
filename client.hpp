/**
 * @file client.hpp
 * @brief Header file for the IPKCP Client
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

/**
 * @brief Enumeration to represent the possible states of the client
 */
enum class IPKCPCState {
	INIT,	  /**< Initial state */
	READY,	  /**< Ready to connect */
	UP,		  /**< Connected */
	EXPECT_BYE, /**< Expecting BYE from server */
	DOWN,	  /**< Disconnected */
	ERROR,	  /**< Error */
};

/**
 * @class IPKCPClient
 * @brief Class representing the IPKCP Client
 */
class IPKCPClient {
   public:
	IPKCPClient(int port, std::string hostname, int protocol);
	IPKCPClient& operator=(IPKCPClient&& other) = default;
	IPKCPClient& operator=(const IPKCPClient&) = default;
	IPKCPClient(IPKCPClient&& other) = default;
	IPKCPClient(const IPKCPClient&) = default;
	~IPKCPClient();

	/* ==== Public Variables ==== */

	std::string error_msg; /**< Error message (if any) */

	/* ==== Getters ==== */

	/**
	 * @brief Get the current state of the client
	 *
	 * @return IPKCPCState - Current state of the client
	 */
	IPKCPCState get_state() const { return this->state; }

	/**
	 * @brief Get the port the client is connected to
	 *
	 * @return int - Port the client is connected to
	 */
	int get_port() const { return this->port; }

	/**
	 * @brief Get the hostname the client is connected to
	 *
	 * @return std::string - Hostname the client is connected to
	 */
	std::string get_hostname() const { return this->hostname; }

	/**
	 * @brief Get the protocol the client is using
	 *
	 * @return int - Protocol the client is using
	 */
	int get_protocol() const { return this->protocol; }

	/* ==== Public Methods ==== */

	bool connect();
	std::string disconnect();

	/**
	 * @brief Send a message to the server
	 *
	 * Sends a message to the server - abstraction over send_tcp and send_udp.
	 *
	 * @param input - Message to send
	 * @return ssize_t - Number of bytes sent
	 */
	ssize_t send(const std::string& input) {
		if (this->state != IPKCPCState::UP) {
			std::cerr << "!ERR! Client is not connected!" << std::endl;
			return -1;
		};
		return (this->protocol == SOCK_STREAM) ? this->send_tcp(input)
									    : this->send_udp(input);
	}

	/**
	 * @brief Receive a message from the server
	 *
	 * Receives a message from the server - abstraction over recv_tcp and
	 * recv_udp.
	 *
	 * @return std::string - Message received
	 */
	std::string recv() {
		return (this->protocol == SOCK_STREAM) ? this->recv_tcp()
									    : this->recv_udp();
	}

   private:
	/* ==== Private Variables ==== */

	int fd;			  /**< Socket file descriptor */
	int port;			  /**< Port to connect to */
	int protocol;		  /**< Protocol to use (SOCK_STREAM or SOCK_DGRAM) */
	IPKCPCState state;	  /**< State of the client */
	std::string hostname; /**< Hostname to connect to */
	struct hostent* host; /**< Host entity */
	struct sockaddr_in addr {};		/**< Socket address */
	socklen_t addr_len = sizeof(addr); /**< Socket address length */

	/* ==== Private Methods ==== */

	ssize_t send_tcp(const std::string& input);
	ssize_t send_udp(const std::string& input);
	std::string recv_tcp();
	std::string recv_udp();
};

#endif  // CLIENT_HPP
