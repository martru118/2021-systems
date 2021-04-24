#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include "lib.h"

#define ADDRESS "graphics.science.uoit.ca"
#define LOCALHOST "localhost"
#define PORT "55555"
#define BUFFER 1024

void sendrecv(int, fd_set*, int, int, char**);
void connAccept(fd_set*, int*, int, struct sockaddr_in*, char**);
void connRequest(int*);

void sendrecv(int i, fd_set* allfds, int sock, int maxfd, char** names) {
	int j;
	char received[BUFFER/2], message[BUFFER];

	char* username = names[i];
	int nbytes = readn(i, received, BUFFER/2);

	if (nbytes < 0) {
		perror("readn");
		exit(1);
	} else if (nbytes == 0) {
		printf("User '%s' has left the chat!\n", username);

		close(i);
		FD_CLR(i, allfds);
	} else {
		//format message
		strncpy(message, username, BUFFER);
		strcat(message, ": ");
		strcat(message, received);
		printf("%s", message);

		for (j = 0; j <= maxfd; j++) {
			if (FD_ISSET(j, allfds)) {
				if (j!=sock && j!=i) {
					//send message
					nbytes = writen(j, message, BUFFER);

					if (nbytes == -1)
						perror("writen");
				}
			}
		}
	}
}

void connAccept(fd_set* allfds, int* maxfd, int sock, struct sockaddr_in* client, char** names) {
	socklen_t addrlen = sizeof (struct sockaddr_in);
	int conn = accept(sock, (struct sockaddr*) client, &addrlen);

	if(conn == -1) {
		perror("accept");
		exit(1);
	} else {
		//handle incoming connections
		FD_SET(conn, allfds);
		if(conn > *maxfd)
			*maxfd = conn;

		//read name of client
		names[conn] = readString(conn);
		printf("User '%s' has joined the chat!\n", names[conn]);
	}
}

void connRequest(int* sock) {
	int rc;
	int conn = 1;

	struct addrinfo hints;
	struct addrinfo* addr;

	//get connection stream
	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;

	//find address that would accept all connections
	rc = getaddrinfo(LOCALHOST, PORT, &hints, &addr);
	if (rc < 0) {
		perror("getaddrinfo");
		exit(1);
	}

	//use the first address found
	*sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (*sock < 0) {
		perror("socket");
		exit(1);
	}

	//make address reusable after socket closes
	rc = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &conn, sizeof conn);
	if (rc < 0) {
		perror("setsockopt");
		exit(1);
	}

	rc = bind(*sock, addr->ai_addr, addr->ai_addrlen);
	if (rc < 0) {
		perror("bind");
		exit(1);
	}

	freeaddrinfo(addr);

	rc = listen(*sock, 10);
	if (rc < 0) {
		perror("listen");
		exit(1);
	}

	printf("Waiting for users...\n\n");
	fflush(stdout);
}

int main(int argc, char** argv) {
	fd_set allfds, readfds;

	int maxfd, i;
	int sock = 0;

	struct sockaddr_in client;
	char* names[FD_SETSIZE] = {0};

	//initialize file descriptors
	FD_ZERO(&allfds);
	FD_ZERO(&readfds);

	//request connection to network
	connRequest(&sock);

	FD_SET(sock, &allfds);
	maxfd = sock;

	while (1) {
		readfds = allfds;

		//wait for an action
		if(select(maxfd+1, &readfds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		//check if client connection is waiting
		for (i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i==sock && names[i]==0)
					connAccept(&allfds, &maxfd, sock, &client, names);
				else
					sendrecv(i, &allfds, sock, maxfd, names);
			}
		}
	}

	return 0;
}