// IPKCP Testing-Server header file

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 512
#define MAX_CLIENTS 5

struct IPKCPServer {
	int fd;
	int port;
	char* mode = NULL;
	struct sockaddr_in address;
	socklen_t address_size = sizeof(address);
	std::vector<int> client_fds;
};

void handle_tcp();
void handle_udp();
void server_shutdown(int sig);

using namespace std;

#endif  // SERVER_HPP