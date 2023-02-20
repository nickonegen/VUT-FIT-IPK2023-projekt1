/**
 * @file server.cpp
 * @author Onegen Something <xonege99@vutbr.cz>
 * @brief IPKCP Testing-Server
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
	cerr << msg << endl << "Usage: ./ipkcps -p <port>" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	/* Parse command line options */
	int opt;
	int port = 0;

	while ((opt = getopt(argc, argv, "p:")) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				break;
			default:
				error("");  // getopt will print an error message
				exit(EXIT_FAILURE);
		}
	}

	/* Validate command line options */
	if (port == 0) {
		error("Port not specified!");
		exit(EXIT_FAILURE);
	}

	/* TODO */
	cout << "Not Implemented" << endl;

	return EXIT_SUCCESS;
}
