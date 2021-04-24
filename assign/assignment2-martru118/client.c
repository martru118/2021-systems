#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "lib.h"

#define ADDRESS "graphics.science.uoit.ca"
#define LOCALHOST "localhost"
#define PORT 55555
#define BUFFER 1024

void sendrecv(int, int);
void connRequest(int*, char*);

void sendrecv(int i, int sock) {
	char sent[BUFFER/2], received[BUFFER];

	if (i == 0) {
		fgets(sent, BUFFER/2, stdin);

		if (strcmp(sent, "exit\n") == 0) {
			//disconnect from server
			close(sock);
			exit(0);
		} else {
			//send input to server
			writen(sock, sent, BUFFER/2);
		}
	} else {
		//read message from server
		int nbytes = readn(sock, received, BUFFER);
		received[nbytes] = '\0';

		printf("%s" , received);
		fflush(stdout);
	}
}

void connRequest(int* sock, char* name) {
	int rc;
	
	struct addrinfo hints;
	struct addrinfo* addr;
	struct sockaddr_in* addrinfo;

	//get connection stream
	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;

	//find address that would accept all connections
	rc = getaddrinfo(LOCALHOST, NULL, &hints, &addr);
	if (rc != 0) {
		perror("getaddrinfo");
		exit(1);
	}

	addrinfo = (struct sockaddr_in*) addr->ai_addr;

	//use the first address found
	*sock = socket(addrinfo->sin_family, addr->ai_socktype, addr->ai_protocol);
	if(*sock < 0) {
		perror("socket");
		exit(1);
	}

	//specify port number
	addrinfo->sin_port = htons(PORT);

	rc = connect(*sock, (struct sockaddr*) addrinfo, addr->ai_addrlen);
	if (rc != 0) {
		perror("connect");
		exit(1);
	}

	//send name to server
	freeaddrinfo(addr);
	writeString(*sock, name);
	printf("Successfully connected to server.\n\n");
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("USAGE: ./client name\n");
		exit(0);
	}

	int sock, maxfd, i;
	fd_set allfds, readfds;

	//request a connection to server
	connRequest(&sock, argv[1]);

	//initialize file descriptors
	FD_ZERO(&allfds);
	FD_ZERO(&readfds);

	FD_SET(0, &allfds);
	FD_SET(sock, &allfds);
	maxfd = sock;

	while (1) {
		readfds = allfds;

		//wait for an action
		if (select(maxfd+1, &readfds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		//check if other clients are on server
		for (i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &readfds))
				sendrecv(i, sock);
		}
	}

	return 0;
}