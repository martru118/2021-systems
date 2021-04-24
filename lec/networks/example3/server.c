/***********************************************
 *
 *                server.c
 *
 *  This is an echo server that uses the select
 *  architecture for servers.  This server just
 *  echos everthing that is sent to it.
 **********************************************/

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

int main(int argc, char **argv) {
	int sock, conn;
	int i;
	int rc;
	struct sockaddr address;
	socklen_t addrLength = sizeof(address);
	struct addrinfo hints;
	struct addrinfo *addr;
	char *message;
	fd_set readfds, allfds;
	int client[FD_SETSIZE];
	int maxfd, maxi;
	int nready;

	/*
	 *  set the hints structure to zero
	 */
	memset(&hints, 0, sizeof(hints));

	/*
	 *  want a stream, also address that will accept all
	 *  connections on this host
	 */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	if((rc = getaddrinfo(NULL, "4321", &hints, &addr))) {
		printf("host name lookup failed: %s\n", gai_strerror(rc));
		exit(1);
	}

	/*
	 *  use the first entry returned by getaddrinfo
	 */
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if(sock < 0) {
		printf("Can't create socket\n");
		exit(1);
	}

	/*
	 *  want to be able to reuse the address right after
	 *  the socket is closed.  Otherwise must wait for 2 minutes
	 */
	i = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

	rc = bind(sock, addr->ai_addr, addr->ai_addrlen);
	if(rc < 0) {
		printf("Can't bind socket\n");
		exit(1);
	}

	/*
	 *  free results returned by getaddrinfo
	 */
	freeaddrinfo(addr);

	rc = listen(sock, 5);
	if(rc < 0) {
		printf("Listen failed\n");
		exit(1);
	}

	/*
	 *  start with just the accept socket and no clients
	 */
	maxfd = sock;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allfds);
	FD_SET(sock, &allfds);

	/*
	 * loop processing requests
	 */
	while(1) {
		readfds = allfds;
		/*
		 *  wait until something happens
		 */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		/*
		 *  check to see if a client connection is waiting
		 */
		if(FD_ISSET(sock, &readfds)) {
			conn = accept(sock, (struct sockaddr*) &address, &addrLength);
			/*
			 * find an entry in the client table for it
			 */
			for(i=0; i<FD_SETSIZE; i++) {
				if(client[i] < 0) {
					client[i] = conn;
					break;
				}
			}
			FD_SET(conn, &allfds);
			if(conn > maxfd)
				maxfd = conn;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}
		/*
		 * now check all the existing clients
		 * to see if they have something to read
		 */
		for(i=0; i<=maxi; i++) {
			conn = client[i];
			if(conn < 0)
				continue;
			if(FD_ISSET(conn, &readfds)) {
				message = readString(conn);
				/*
				 * has the client disconnected
				 */
				if(message == NULL) {
					close(conn);
					client[i] = -1;
					FD_CLR(conn, &allfds);
				} else {
					writeString(conn, message);
					free(message);
				}
			}
		}
	}

	close(sock);
	exit(0);
}
