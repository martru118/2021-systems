#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT "4321"
#define SIZE 512

int main(int argc, char **argv) {
	int sock, i, rc;
	char buffer[SIZE];

	struct sockaddr address;
	socklen_t addrLength = sizeof(address);
	struct addrinfo hints;
	struct addrinfo *addr;

	memset(&hints, 0, sizeof(hints));

	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	if((rc = getaddrinfo(NULL, PORT, &hints, &addr))) {
		printf("host name lookup failed: %s\n", gai_strerror(rc));
		exit(1);
	}

	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if(sock < 0) {
		printf("Can't create socket\n");
		exit(1);
	}

	i = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

	rc = bind(sock, addr->ai_addr, addr->ai_addrlen);
	if(rc < 0) {
		printf("Can't bind socket\n");
		exit(1);
	}

	freeaddrinfo(addr);

	while(1) {
		int first = 0;
		int second = 0;

		rc = recvfrom(sock, buffer, SIZE, 0, (struct sockaddr*) &address, &addrLength);
		sscanf(buffer, "%d", &first);
		rc = recvfrom(sock, buffer, SIZE, 0, (struct sockaddr*) &address, &addrLength);
		sscanf(buffer, "%d", &second);

		printf("%d %d", second, first);
		second += first;
		printf("%d", second);

		sprintf(buffer, "%d", second);
		sendto(sock, buffer, rc, 0, (const struct sockaddr*) &address, addrLength);
	}

	close(sock);
	exit(0);
}