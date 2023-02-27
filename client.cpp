/**
 * @file client.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Client
 * @date 2023-??-??
 *
 */

#include "client.hpp"

/* Helper methods */
std::string trunc_payload(std::string input) {
	input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
	input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
	return input.substr(0, MAX_PAYLOAD_LEN - 1) + "\n";
}

/* IPKCPClient methods */
IPKCPClient::IPKCPClient(int port, std::string hostname, int protocol) {
	this->state = IPKCPCState::INIT;

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

	memset(reinterpret_cast<char *>(&(this->addr)), 0, this->addr_len);
	this->addr.sin_family = AF_INET;
	memcpy(this->host->h_addr,
		  reinterpret_cast<char *>(&(this->addr).sin_addr.s_addr),
		  this->host->h_length);
	this->addr.sin_port = htons(this->port);
	int vld
	    = inet_pton(AF_INET, this->hostname.c_str(), &(this->addr.sin_addr));
	if (vld == 0) {
		std::cerr << "!ERR! Invalid address!" << std::endl;
		exit(EXIT_FAILURE);
	} else if (vld < 0) {
		std::cerr << "!ERR! Failed to convert address!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Set up timeout */
	struct timeval timeout {
		TIMEOUT, 0
	};
	if (setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO,
				reinterpret_cast<char *>(&timeout), sizeof(timeout))
		   < 0
	    || setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO,
				   reinterpret_cast<char *>(&timeout), sizeof(timeout))
			 < 0) {
		std::cerr << "!ERR! Failed to set socket timeout!" << std::endl;
		exit(EXIT_FAILURE);
	}

	this->state = IPKCPCState::READY;
}

void IPKCPClient::disconnect() {
	/* TCP */
	if (this->protocol == SOCK_STREAM) {
		/* Client is up - graceful exit */
		if (this->state == IPKCPCState::UP) {
			std::cout << "BYE" << std::endl;
			this->send("BYE");
			std::cout << this->recv() << std::endl;
			return;
		}

		/* Client is already expecting BYE */
		if (this->state == IPKCPCState::EXPECT_BYE) {
			std::cout << this->recv() << std::endl;
			return;
		}

		/* Non-connected client - nothing to do */
		return;
	}

	/* UDP */
	if (this->protocol == SOCK_DGRAM) {
		this->state = IPKCPCState::DOWN;
		return;
	}
}

IPKCPClient::~IPKCPClient() {
	shutdown(this->fd, SHUT_RDWR);
	close(fd);
}

bool IPKCPClient::connect() {
	/* UDP is connectionless */
	if (this->protocol == SOCK_DGRAM) {
		std::cout << "Connected to " << this->hostname << ":" << this->port
				<< std::endl;
		this->state = IPKCPCState::UP;
		return true;
	}

	/* Connect to TCP server */
	if (::connect(this->fd, reinterpret_cast<struct sockaddr *>(&(this->addr)),
			    this->addr_len)
	    < 0) {
		std::cerr << "!ERR! Failed to connect to server!" << std::endl;
		return false;
	}

	std::cout << "Connected to " << this->hostname << ":" << this->port
			<< std::endl;
	this->state = IPKCPCState::UP;
	return true;
}

ssize_t IPKCPClient::send_tcp(const std::string &input) {
	/* Enforce maximum length and line ending */
	const std::string payload = trunc_payload(input);

	/* Create buffer */
	std::array<char, BUFFER_SIZE> buffer{0};
	memcpy(buffer.data(), payload.data(), payload.size() + 1);

	/* Set EXPECT_BYE state if message is "BYE" */
	if (strncmp(buffer.data(), "BYE", 3) == 0) {
		this->state = IPKCPCState::EXPECT_BYE;
	}

	/* Send data */
	ssize_t write_size = write(this->fd, buffer.data(), payload.size());

	/* Handle errors */
	if (write_size < 0) {
		if (errno == EAGAIN || errno == ETIMEDOUT) {
			std::cerr << "!ERR! Connection timed out!" << std::endl;
		} else {
			std::cerr << "!ERR! Failed to send data to server!" << std::endl;
		}
	}

	return write_size;
}

ssize_t IPKCPClient::send_udp(const std::string &input) {
	/* Enforce maximum length and line ending */
	std::string payload = trunc_payload(input);

	/* Create buffer */
	std::array<unsigned char, BUFFER_SIZE> buffer{0};
	buffer[0] = OP_REQUEST;
	buffer[1] = static_cast<uint8_t>(payload.size());
	memcpy(buffer.data() + 2, payload.c_str(), payload.size() + 1);

	/* Send data */
	ssize_t write_size = sendto(
	    this->fd, buffer.data(), payload.size() + 2, 0,
	    reinterpret_cast<struct sockaddr *>(&(this->addr)), this->addr_len);

	/* Handle errors */
	if (write_size < 0) {
		if (errno == EAGAIN || errno == ETIMEDOUT) {
			std::cerr << "!ERR! Connection timed out!" << std::endl;
		} else {
			std::cerr << "!ERR! Failed to send data to server!" << std::endl;
		}
	}

	return write_size;
}

std::string IPKCPClient::recv_tcp() {
	/* Create buffer */
	std::array<char, BUFFER_SIZE> buffer{0};

	/* Receive data */
	ssize_t read_size = read(this->fd, buffer.data(), BUFFER_SIZE - 1);

	/* Handle errors */
	if (read_size < 0) {
		if (errno == EAGAIN || errno == ETIMEDOUT) {
			std::cerr << "!ERR! Connection timed out!" << std::endl;
			return "";
		}

		std::cerr << "!ERR! Failed to send data to server!" << std::endl;
		return "";
	}

	if (read_size == 0) {
		this->state = IPKCPCState::ERROR;
		return "";
	}

	/* Recieved "BYE" */
	if (strncmp(buffer.data(), "BYE", 3) == 0) {
		/* Expected */
		if (this->state == IPKCPCState::EXPECT_BYE) {
			this->state = IPKCPCState::DOWN;
			return "BYE";
		}

		/* Unexpected */
		this->state = IPKCPCState::ERROR;
		return "BYE";
	}

	return std::string(buffer.data());
}

std::string IPKCPClient::recv_udp() {
	/* Create buffer */
	std::array<char, BUFFER_SIZE> buffer{0};

	/* Receive data */
	ssize_t read_size = recvfrom(
	    this->fd, buffer.data(), BUFFER_SIZE - 1, 0,
	    reinterpret_cast<struct sockaddr *>(&(this->addr)), &(this->addr_len));

	/* Handle errors */
	if (read_size < 0) {
		if (errno == EAGAIN || errno == ETIMEDOUT) {
			std::cerr << "!ERR! Connection timed out!" << std::endl;
			return "";
		}

		std::cerr << "!ERR! Failed to send data to server!" << std::endl;
		return "";
	}

	if (read_size == 0) {
		std::cerr << "!ERR! Server unexpectedly disconnected!" << std::endl;
		return "";
	}

	return (buffer[1] == STATUS_OK ? "OK:" : "ERR:")
		  + std::string(buffer.data() + 3);
}
