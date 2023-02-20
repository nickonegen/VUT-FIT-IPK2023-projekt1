/**
 * @file client.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Client
 * @date 2023-??-??
 *
 */

#include "client.hpp"

IPKCPClient* client = new IPKCPClient();

int communicate_tcp() {
	/* Set up text buffer */
	char buffer[BUFFER_SIZE] = {0};
	bzero(buffer, BUFFER_SIZE);

	/* Read from stdin */
	cin.getline(buffer, BUFFER_SIZE - 1);

	/* Send message to server */
	ssize_t write_size = write(client->fd, buffer, strlen(buffer));
	if (write_size < 0) {
		cerr << "!ERR! Failed to send data to server!" << endl;
		return -1;
	}

	/* Read response from server */
	bzero(buffer, BUFFER_SIZE);
	ssize_t read_size = read(client->fd, buffer, BUFFER_SIZE - 1);
	if (read_size < 0) {
		cerr << "!ERR! Failed to read data from server!" << endl;
		return -1;
	} else if (read_size == 0) {
		cerr << "!ERR! Server unexpectedly disconnected!" << endl;
		return -1;
	}

	/* Print response */
	cout << buffer << endl;

	/* End connection if message is BYE */
	if (strcmp(buffer, "BYE") == 0) {
		return 1;
	}

	return 0;
}

int communicate_udp() {
	// TODO
	return -1;
}

int main(int argc, char* argv[]) {
	/* Parse command line options */
	int opt;
	while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
		switch (opt) {
			case 'h':
				client->hostname = optarg;
				break;
			case 'p':
				client->port = atoi(optarg);
				break;
			case 'm':
				client->mode = optarg;
				break;
			default:
				// getopt will print an error message
				exit(EXIT_FAILURE);
		}
	}

	/* Validate command line options */
	if (client->hostname == NULL) {
		cerr << "!ERR! Host not specified!" << endl;
		cerr << "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	}

	if (client->port == 0) {
		cerr << "!ERR! Port not specified!" << endl;
		cerr << "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	}

	if (client->mode == NULL) {
		cerr << "!ERR! Mode not specified!" << endl;
		cerr << "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	} else if (strcmp(client->mode, "tcp") != 0
			 && strcmp(client->mode, "udp") != 0) {
		cerr << "!ERR! Invalid mode specified, expected tcp/udp!" << endl;
		cerr << "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	}

	// TODO: UDP support
	if (strcmp(client->mode, "udp") == 0) {
		cerr << "!ERR! UDP mode not supported yet!" << endl;
		exit(EXIT_FAILURE);
	}

	int type = strcmp(client->mode, "tcp") == 0 ? SOCK_STREAM : SOCK_DGRAM;

	/* Create socket */
	client->fd = socket(AF_INET, type, 0);
	if (client->fd < 0) {
		cerr << "!ERR! Failed to create socket!" << endl;
		exit(EXIT_FAILURE);
	}

	/* Get host address */
	client->host = gethostbyname(client->hostname);
	if (client->host == NULL) {
		cerr << "!ERR! Failed to resolve host!" << endl;
		exit(EXIT_FAILURE);
	}

	bzero((char*)&(client->host_address), client->address_size);
	client->host_address.sin_family = AF_INET;
	bcopy((char*)client->host->h_addr,
		 (char*)&(client->host_address).sin_addr.s_addr,
		 client->host->h_length);
	client->host_address.sin_port = htons(client->port);

	/* Set up timeout */
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
	if (setsockopt(client->fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
				sizeof(timeout))
		   < 0
	    || setsockopt(client->fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
				   sizeof(timeout))
			 < 0) {
		cerr << "!ERR! Failed to set socket timeout!" << endl;
		exit(EXIT_FAILURE);
	}

	/* Connect to server */
	if (connect(client->fd, (struct sockaddr*)&(client->host_address),
			  client->address_size)
	    < 0) {
		cerr << "!ERR! Failed to connect to server!" << endl;
		exit(EXIT_FAILURE);
	} else {
		cout << "Connected to " << client->hostname << ":" << client->port
			<< endl;
	}

	/* Communicate with server */
	int status = 0;
	while (status == 0) {
		status = communicate_tcp();
	}

	/* Close socket */
	close(client->fd);

	return status == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
}