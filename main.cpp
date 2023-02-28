/**
 * @file main.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief Main logic of IPKCP Client
 * @date 2023-02-28
 *
 */

#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "client.hpp"

/**
 * @brief SIGINT Flag
 *
 * A flag indicating whether SIGINT was received, used to gracefully
 * terminate the client.
 */
std::atomic<bool> quit(false);

/**
 * @brief Signal handling function
 *
 * Sets the quit flag to true, which is used to gracefully terminate the
 * client.
 *
 * @param signal - Signal number
 */
void signal_handler(int signal) {
	(void)signal;
	quit.store(true);
}

/**
 * @brief Main function
 *
 * Parses command line arguments, creates a client and connects to the
 * server. Then it reads input from stdin and sends it to the server.
 *
 * @return int - Exit code
 */
int main(int argc, char* argv[]) {
	using std::cerr;
	using std::cout;
	using std::endl;

	/* Set up signal handler */
	struct sigaction sig_act {};
	memset(&sig_act, 0, sizeof(sig_act));
	sig_act.sa_handler = signal_handler;
	sigfillset(&sig_act.sa_mask);
	sigaction(SIGINT, &sig_act, NULL);

	/* Parse command line options */
	int opt;
	int port = 0;
	std::string hostname;
	std::string protocol;
	while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
		switch (opt) {
			case 'h':
				hostname = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'm':
				protocol = optarg;
				break;
			default:
				cerr << "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>"
					<< endl;
				return EXIT_FAILURE;
		}
	}

	/* Validate options */
	if (hostname.empty()) {
		cerr << "!ERR! Host not specified!" << endl
			<< "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		return EXIT_FAILURE;
	}

	if (port == 0) {
		cerr << "!ERR! Port not specified!" << endl
			<< "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		return EXIT_FAILURE;
	}

	if (protocol.empty()) {
		cerr << "!ERR! Mode not specified!" << endl
			<< "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		return EXIT_FAILURE;
	}

	if (protocol != "tcp" && protocol != "udp") {
		cerr << "!ERR! Invalid mode, expected tcp or udp!" << endl
			<< "  Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
		return EXIT_FAILURE;
	}

	/* Create client */
	IPKCPClient client(port, hostname,
				    protocol == "tcp" ? SOCK_STREAM : SOCK_DGRAM);

	/* Connect to server */
	if (!client.connect() || client.state == IPKCPCState::ERROR) {
		cerr << "!ERR! " << client.error_msg << endl;
		return EXIT_FAILURE;
	}

	/* Communicate */
	while (client.state == IPKCPCState::UP) {
		/* Read input */
		std::string input;
		std::getline(std::cin, input);

		/* SIGINT handler */
		if (quit.load()) {
			// This is just to inform the user that BYE message
			// will be sent to the server
			if (protocol == "tcp") {
				cout << "BYE" << endl;
			}

			/* Disconnect from server */
			cout << client.disconnect() << endl;
			break;
		}

		/* Send input to server */
		if (client.send(input) < 0) {
			break;
		}

		/* Receive response from server */
		std::string response = client.recv();
		if (response.empty()) {
			break;
		}

		/* Print response */
		cout << response << endl;
	}

	/* Print error message */
	if (client.state != IPKCPCState::DOWN) {
		cout << "!ERR! " << client.error_msg << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}