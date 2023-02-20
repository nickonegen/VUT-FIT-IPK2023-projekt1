// IPKCP Client header file

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#define BUFFER_SIZE 512
#define TIMEOUT 5  // in seconds

struct IPKCPClient {
	int fd;
	int port;
	char* mode = NULL;
	char* hostname = NULL;
	struct hostent* host = NULL;
	struct sockaddr_in host_address;
	socklen_t address_size = sizeof(host_address);
};

int communicate_tcp();

using namespace std;

#endif  // CLIENT_HPP