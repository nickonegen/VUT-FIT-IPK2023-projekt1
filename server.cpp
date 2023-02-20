/**
 * @file server.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Testing-Server
 * @date 2023-??-??
 *
 */

#include "server.hpp"

IPKCPServer* server = new IPKCPServer();

void handle_tcp() {
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_size = sizeof(client_addr);

	/* Accept a new connection */
	client_fd
	    = accept(server->fd, (struct sockaddr*)&client_addr, &client_size);
	if (client_fd < 0) {
		cerr << "!ERR! Problem accepting new connection!" << endl;
		return;
	}

	server->client_fds.push_back(client_fd);
	cout << "New client connected! (#" << server->client_fds.size() << ")"
		<< endl;

	while (true) {
		/* Read from client */
		char buffer[BUFFER_SIZE] = {0};
		ssize_t read_size = read(client_fd, buffer, BUFFER_SIZE - 1);
		if (read_size < 0) {
			cerr << "!ERR! Problem reading from socket!" << endl;
			break;
		} else if (read_size == 0) {
			break;  // Client disconnected
		}

		buffer[read_size] = '\0';
		string message = string(buffer);

		/* End connection if message is BYE */
		if (strcmp(buffer, "BYE") == 0) {
			write(client_fd, "BYE", 3);
			break;
		}

		/* DEBUG: Respond with reversed string */
		string response = message;
		reverse(response.begin(), response.end());
		ssize_t write_size
		    = write(client_fd, response.c_str(), response.size());
		if (write_size < 0) {
			cerr << "!ERR! Problem writing to socket!" << endl;
			break;
		}
	}

	close(client_fd);
	auto vec_fd = find(server->client_fds.begin(), server->client_fds.end(),
				    client_fd);
	if (vec_fd != server->client_fds.end()) {
		server->client_fds.erase(vec_fd);
		cout << "Client disconnected! (#" << server->client_fds.size() << ")"
			<< endl;
	}
}

void handle_udp() {
	// TODO
}

void server_shutdown(int sig) {
	cout << endl << "Shutting down..." << endl;

	/* Close all connections */
	int cnt = 0;
	for (auto const& client_fd : server->client_fds) {
		close(client_fd);
		cnt++;
	}
	server->client_fds.clear();
	cout << "Closed " << cnt << " connections" << endl;

	/* Close server and exit */
	shutdown(server->fd, SHUT_RDWR);
	close(server->fd);
	server->fd = -1;
	delete server;
	exit(EXIT_SUCCESS);
}

int main(int argc, char* const* argv) {
	/* Parse command line options */
	int opt;
	while ((opt = getopt(argc, argv, "p:m:")) != -1) {
		switch (opt) {
			case 'p':
				server->port = atoi(optarg);
				break;
			case 'm':
				server->mode = optarg;
				break;
			default:
				// getopt will print an error message
				exit(EXIT_FAILURE);
		}
	}

	/* Validate command line options */
	if (server->port == 0) {
		cerr << "!ERR! Port not specified!" << endl;
		cerr << "  Usage: ./ipkcps -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	}

	if (server->mode == NULL) {
		cerr << "!ERR! Mode not specified!" << endl;
		cerr << "  Usage: ./ipkcps -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	} else if (strcmp(server->mode, "tcp") != 0
			 && strcmp(server->mode, "udp") != 0) {
		cerr << "!ERR! Invalid mode specified, expected tcp/udp!" << endl;
		cerr << "  Usage: ./ipkcps -p <port> -m <mode>" << endl;
		exit(EXIT_FAILURE);
	}

	// TODO: UDP support
	if (strcmp(server->mode, "udp") == 0) {
		cerr << "!ERR! UDP mode not supported yet!" << endl;
		exit(EXIT_FAILURE);
	}

	int type = strcmp(server->mode, "tcp") == 0 ? SOCK_STREAM : SOCK_DGRAM;

	/* Create socket */
	server->fd = socket(AF_INET, type, 0);
	if (server->fd < 0) {
		cerr << "!ERR! Failed to open " << server->mode << " socket!" << endl;
		exit(EXIT_FAILURE);
	}

	int reuse = 1;
	if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))
	    < 0) {
		cerr << "!ERR! Failed to set socket options!" << endl;
		exit(EXIT_FAILURE);
	}

	bzero((char*)&(server->address), server->address_size);
	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = htons(server->port);

	/* Bind socket */
	if (bind(server->fd, (struct sockaddr*)&(server->address),
		    server->address_size)
	    < 0) {
		cerr << "!ERR! Failed to bind socket to port " << server->port << "!"
			<< endl;
		exit(EXIT_FAILURE);
	} else {
		cout << "Listening on port " << server->port << "..." << endl;
	}

	/* Signal handler for graceful exit on CTRL+C */
	signal(SIGINT, server_shutdown);

	/* Handle connections/diagrams */
	while (true) {
		if (type == SOCK_STREAM) {
			/* Listen for new connections */
			if (listen(server->fd, MAX_CLIENTS) < 0) {
				cerr << "!ERR! Failed to listen on port " << server->port
					<< "!" << endl;
				exit(EXIT_FAILURE);
			}

			/* TCP, use textual data */
			handle_tcp();
		} else {
			/* UDP, use binary data */
			handle_udp();
		}
	}

	return EXIT_SUCCESS;
}
