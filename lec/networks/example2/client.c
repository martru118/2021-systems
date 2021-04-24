/********************************************************
 *
 *                          client.c
 *
 *  A more robust client example.  In this example the
 *  client has two arguments, integers.  These integers
 *  are sent to the server, which returns the sum of the
 *  two integers.
 *******************************************************/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "lib.h"

int main(int argc, char **argv) {
	struct addrinfo hints;
	struct addrinfo *addr;
	struct sockaddr_in *addrinfo;
	int rc;
	int sock;
	char buffer[512];
	int len;
	int arg1, arg2;
	int result;
	int ret;

	if(argc != 3) {
		printf("Usage: client num1 num2\n");
		exit(1);
	}

	arg1 = atoi(argv[1]);
	arg2 = atoi(argv[2]);

	/*
	 *  clear the hints structure to zero
	 */
	memset(&hints, 0, sizeof(hints));

	/*
	 *  want a stream on a compatible interface
         */
	hints.ai_socktype = SOCK_STREAM;
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

	rc = connect(sock, (struct sockaddr *) addrinfo, addr->ai_addrlen);
	if(rc != 0) {
		printf("Can't connect to server\n");
		exit(1);
	}

	/*
	 *  free the results returned by get addrinfo
	 */
	freeaddrinfo(addr);

	/*
	 *  send a message to the server and echo the response
	 */
	ret = writen(sock, (char*) &arg1, sizeof(arg1));
	if(ret != sizeof(arg1)) {
		printf("Error sending first argument\n");
		close(sock);
		exit(1);
	}
	ret = writen(sock, (char*) &arg2, sizeof(arg2));
	if(ret != sizeof(arg2)) {
		printf("Error sending second argument\n");
		close(sock);
		exit(1);
	}
	ret = readn(sock, (char*) &result, sizeof(result));
	if(ret != sizeof(result)) {
		printf("Error reading result\n");
		close(sock);
		exit(1);
	}
	printf("The sum of %d and %d is %d\n", arg1, arg2, result);
	ret = writeString(sock, "Thank You");
	if(ret) {
		printf("Error sending thank you\n");
		close(sock);
		exit(1);
	}

	close(sock);

	exit(0);
}
