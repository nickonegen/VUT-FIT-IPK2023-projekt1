/**
 * @file client.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Client
 * @date 2023-02-28
 *
 */

#include "client.hpp"

/* ==== Helper methods ==== */

/**
 * @brief Truncates and cleans up the string
 *
 * Truncates the string to MAX_PAYLOAD_LEN and removes all newline characters
 * from it (removing carriage returns for Windows compatibility). Returns the
 * truncated string with a newline character at the end in upper case.
 *
 * @param input - String to be truncated
 * @return std::string Cleaned up string
 */
std::string trunc_payload(std::string input) {
	input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
	input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
	return input.substr(0, MAX_PAYLOAD_LEN - 1) + "\n";
}

/* ==== IPKCPClient methods ==== */

/**
 * @brief Construct a new IPKCPClient object
 *
 * Initializes the client and sets up the socket. Exits the program on error.
 *
 * @param port - Port to connect to
 * @param hostname - Hostname to connect to
 * @param protocol - Protocol to use; also controls the connection variant
 */
IPKCPClient::IPKCPClient(int port, std::string hostname, int protocol) {
	this->state = IPKCPCState::INIT;

	/* Set attributes */
	this->port = port;
	this->hostname = std::move(hostname);
	this->protocol = protocol;

	/* Resolve hostname */
	this->host = gethostbyname(this->hostname.c_str());
	if (this->host == NULL) {
		this->error_msg = "Failed to resolve hostname!";
		this->state = IPKCPCState::ERROR;
		exit(EXIT_FAILURE);
	}

	/* Create socket */
	this->fd = socket(AF_INET, this->protocol, 0);
	if (this->fd < 0) {
		this->error_msg = "Failed to create socket!";
		this->state = IPKCPCState::ERROR;
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
		this->error_msg = "Invalid address!";
		this->state = IPKCPCState::ERROR;
		exit(EXIT_FAILURE);
	} else if (vld < 0) {
		this->error_msg = "Failed to convert address!";
		this->state = IPKCPCState::ERROR;
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
		this->error_msg = "Failed to set socket timeout!";
		this->state = IPKCPCState::ERROR;
		exit(EXIT_FAILURE);
	}

	this->state = IPKCPCState::READY;
}

/**
 * @brief Opens a connection to the server
 *
 * @return true - Successfully connected
 * @return false - Failed to connect (see error_msg for details)
 */
bool IPKCPClient::connect() {
	/* UDP */
	if (this->protocol == SOCK_DGRAM) {
		this->state = IPKCPCState::UP;
		return true;
	}

	/* TCP */
	if (::connect(this->fd, reinterpret_cast<struct sockaddr *>(&(this->addr)),
			    this->addr_len)
	    < 0) {
		this->error_msg = "Failed to connect to server!";
		this->state = IPKCPCState::ERROR;
		return false;
	}

	this->state = IPKCPCState::UP;
	return true;
}

/**
 * @brief Closes the connection to the server
 *
 * Proper closing of the connection. When using the textual variant (TCP), the
 * client sends a BYE message and waits for a response. When using the binary
 * variant (UDP), the client simply sets the state to DOWN. Keep in mind that
 * the socket is closed by the destructor.
 *
 * @return std::string - Final message from the server (TCP only)
 */
std::string IPKCPClient::disconnect() {
	/* TCP */
	if (this->protocol == SOCK_STREAM) {
		/* Client is up - graceful exit */
		if (this->state == IPKCPCState::UP) {
			this->send("BYE");	  // sets EXPECT_BYE state
			return this->recv();  // sets DOWN state
		}

		/* Client is already expecting BYE */
		if (this->state == IPKCPCState::EXPECT_BYE) {
			return this->recv();
		}

		/* Non-connected client - nothing to do */
		return "";
	}

	/* UDP */
	if (this->protocol == SOCK_DGRAM) {
		this->state = IPKCPCState::DOWN;
		return "";
	}

	return "";
}

/**
 * @brief Destroy the IPKCPClient object (close socket)
 *
 */
IPKCPClient::~IPKCPClient() {
	shutdown(this->fd, SHUT_RDWR);
	close(fd);
}

/**
 * @brief Sends a textual message to the remote host.
 *
 * @param input - Message to send
 * @return ssize_t - Number of bytes sent
 */
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
			this->error_msg = "Connection timed out!";
		} else {
			this->error_msg = "Failed to send data to server!";
		}
		this->state = IPKCPCState::ERROR;
	}

	return write_size;
}

/**
 * @brief Sends a binary message to the remote host.
 *
 * @param input - Message to send
 * @return ssize_t - Number of bytes sent
 */
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
			this->error_msg = "Connection timed out!";
		} else {
			this->error_msg = "Failed to send data to server!";
		}
		this->state = IPKCPCState::ERROR;
	}

	return write_size;
}

/**
 * @brief Receives a textual message from the remote host.
 *
 * @return std::string - Received message
 */
std::string IPKCPClient::recv_tcp() {
	/* Create buffer */
	std::array<char, BUFFER_SIZE> buffer{0};

	/* Receive data */
	ssize_t read_size = read(this->fd, buffer.data(), BUFFER_SIZE - 1);

	/* Handle errors */
	if (read_size < 0) {
		if (errno == EAGAIN || errno == ETIMEDOUT) {
			this->error_msg = "Connection timed out!";
			return "";
		}

		this->error_msg = "Failed to send data to server!";
		return "";
	}

	if (read_size == 0) {
		this->state = IPKCPCState::ERROR;
		this->error_msg = "Server unexpectedly disconnected!";
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
		this->error_msg = "Server unexpectedly disconnected!";
		return "BYE";
	}

	return std::string(buffer.data());
}

/**
 * @brief Receives a binary message from the remote host.
 *
 * @return std::string - Received message
 */
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
			this->error_msg = "Connection timed out!";
			return "";
		}

		this->error_msg = "Failed to send data to server!";
		return "";
	}

	if (read_size == 0) {
		this->error_msg = "Server unexpectedly disconnected!";
		return "";
	}

	return (buffer[1] == STATUS_OK ? "OK:" : "ERR:")
		  + std::string(buffer.data() + 3);
}
