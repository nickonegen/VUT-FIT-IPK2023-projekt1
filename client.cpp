/**
 * @file client.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Client
 * @date 2023-??-??
 *
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

using namespace std;

/**
 * @brief Prints error message, usage and exits with EXIT_FAILURE
 *
 * @param msg - error message
 */
void error(const char* msg) {
	cerr << msg << endl
		<< "Usage: ./ipkcpc -h <host> -p <port> -m <mode>" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	/* Parse command line options */
	int opt;
	int port = 0;
	char* host = NULL;
	char* mode = NULL;

	while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'm':
				mode = optarg;
				break;
			default:
				error("");  // getopt will print an error message
				exit(EXIT_FAILURE);
		}
	}

	/* Validate command line options */
	if (host == NULL) {
		error("Host not specified!");
		exit(EXIT_FAILURE);
	}

	if (port == 0) {
		error("Port not specified!");
		exit(EXIT_FAILURE);
	}

	if (mode == NULL) {
		error("Mode not specified!");
		exit(EXIT_FAILURE);
	} else if (strcmp(mode, "tcp") != 0 && strcmp(mode, "udp") != 0) {
		error("Invalid mode, expected 'tcp' or 'udp'!");
		exit(EXIT_FAILURE);
	}

	/* TODO */
	cout << "Not Implemented" << endl;

	return EXIT_SUCCESS;
}