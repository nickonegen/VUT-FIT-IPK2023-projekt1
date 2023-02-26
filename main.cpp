/**
 * @file main.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief Main logic of IPKCP Client
 * @date 2023-??-??
 *
 */

#include "main.hpp"

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
	if (!client.connect()) {
		return EXIT_FAILURE;
	}

	/* Communicate */
	// TODO: Put this into separate function
	while (true) {
		/* Read input */
		std::string input;
		std::getline(std::cin, input);

		/* Send input to server */
		if (client.send(input) < 0) {
			return EXIT_FAILURE;
		}

		/* Receive response from server */
		std::string response = client.recv();
		if (response.empty()) {
			return EXIT_FAILURE;
		}

		/* Print response */
		cout << response << endl;
		if (response == "BYE") {
			return EXIT_SUCCESS;
		}

		/* Required for proper signal handling */
		sleep(1);
		if (quit.load()) {
			break;
		}
	}

	return EXIT_SUCCESS;
}