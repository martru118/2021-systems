/********************************************************
 *
 *                          client.c
 *
 *  Client program to test the data gram based echo server.
 *******************************************************/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
	struct addrinfo hints;
	struct addrinfo *addr;
	struct sockaddr_in *addrinfo;
	int rc;
	int sock;
	char buffer[512];
	char *ret;

	/*
	 *  clear the hints structure to zero
	 */
	memset(&hints, 0, sizeof(hints));

	/*
	 *  want a data gram on a compatible interface
         */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_ADDRCONFIG;
	/*
	 *  localhost is the name of the current computer
	 */
	rc = getaddrinfo("localhost", NULL, &hints, &addr);
	if(rc != 0) {
		printf("Host name lookup failed: %s\n", gai_strerror(rc));
		exit(1);
	}

	/*
	 *  use the first result from getaddrinfo
	 */
	addrinfo = (struct sockaddr_in *) addr->ai_addr;

	sock = socket(addrinfo->sin_family, addr->ai_socktype, addr->ai_protocol);
	if(sock < 0) {
		printf("Can't create socket\n");
		exit(1);
	}

	/*
	 *  specify the port number
	 */
	addrinfo->sin_port = htons(4321);

	/*
	 *  free the results returned by get addrinfo
	 */
	freeaddrinfo(addr);

	/*
	 *  loop reading a line from the user and sending it
	 *  to the echo server, print the line when if comes back
	 */
	while(1) {
		ret = fgets(buffer, 512, stdin);
		/*
		 *  check for end of file
		 */
		if(ret == NULL)
			break;
		sendto(sock, buffer, strlen(buffer), 0, 
			(const struct sockaddr*) addrinfo, addr->ai_addrlen);
		recvfrom(sock, buffer, 512, 0, NULL, NULL);
		printf("%s",buffer);
	}

	close(sock);

	exit(0);
}
