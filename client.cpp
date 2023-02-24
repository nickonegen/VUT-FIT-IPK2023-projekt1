/**
 * @file client.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Client
 * @date 2023-??-??
 *
 */

#include "client.hpp"

IPKCPClient::IPKCPClient(int port, std::string hostname, int protocol) {
	/* Set attributes */
	this->port = port;
	this->hostname = std::move(hostname);
	this->protocol = protocol;

	/* Resolve hostname */
	this->host = gethostbyname(this->hostname.c_str());
	if (this->host == NULL) {
		std::cerr << "!ERR! Failed to resolve hostname!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Create socket */
	this->fd = socket(AF_INET, this->protocol, 0);
	if (this->fd < 0) {
		std::cerr << "!ERR! Failed to create socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(reinterpret_cast<char*>(&(this->addr)), 0, this->addr_len);
	this->addr.sin_family = AF_INET;
	memcpy(this->host->h_addr,
		  reinterpret_cast<char*>(&(this->addr).sin_addr.s_addr),
		  this->host->h_length);
	this->addr.sin_port = htons(this->port);
}

IPKCPClient::~IPKCPClient() { close(fd); }

void IPKCPClient::connect() {
	/* UDP is connectionless */
	if (this->protocol == SOCK_DGRAM) {
		std::cout << "Connected to " << this->hostname << ":" << this->port
				<< std::endl;
		return;
	}

	/* Connect to TCP server */
	if (::connect(this->fd, reinterpret_cast<struct sockaddr*>(&(this->addr)),
			    this->addr_len)
	    < 0) {
		std::cerr << "!ERR! Failed to connect to server!" << std::endl;
		exit(EXIT_FAILURE);
	} else {
		std::cout << "Connected to " << this->hostname << ":" << this->port
				<< std::endl;
	}
}

ssize_t IPKCPClient::send(char* buffer) {
	if (this->protocol == SOCK_STREAM) {
		return this->send_tcp(buffer);
	}

	return this->send_udp(buffer);
}

ssize_t IPKCPClient::recv(char* buffer) {
	if (this->protocol == SOCK_STREAM) {
		return this->recv_tcp(buffer);
	}

	return this->recv_udp(buffer);
}

ssize_t IPKCPClient::send_tcp(char* buffer) const {
	ssize_t write_size = write(this->fd, buffer, strlen(buffer));

	if (write_size < 0) {
		std::cerr << "!ERR! Failed to send data to server!" << std::endl;
	}

	return write_size;
}

ssize_t IPKCPClient::send_udp(char* buffer) {
	ssize_t write_size = sendto(
	    this->fd, buffer, strlen(buffer), 0,
	    reinterpret_cast<struct sockaddr*>(&(this->addr)), this->addr_len);

	if (write_size < 0) {
		std::cerr << "!ERR! Failed to send data to server!" << std::endl;
	}

	return write_size;
}

ssize_t IPKCPClient::recv_tcp(char* buffer) const {
	ssize_t read_size = read(this->fd, buffer, BUFFER_SIZE - 1);

	if (read_size < 0) {
		std::cerr << "!ERR! Failed to read data from server!" << std::endl;
	}

	if (read_size == 0) {
		std::cerr << "!ERR! Server unexpectedly disconnected!" << std::endl;
	}

	return read_size;
}

ssize_t IPKCPClient::recv_udp(char* buffer) {
	ssize_t read_size = recvfrom(
	    this->fd, buffer, BUFFER_SIZE - 1, 0,
	    reinterpret_cast<struct sockaddr*>(&(this->addr)), &(this->addr_len));

	if (read_size < 0) {
		std::cerr << "!ERR! Failed to read data from server!" << std::endl;
	}

	return read_size;
}
