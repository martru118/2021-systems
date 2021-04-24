#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4321
#define SIZE 512

int main(int argc, char **argv) {
	struct addrinfo hints;
	struct addrinfo *addr;
	struct sockaddr_in *addrinfo;

	int rc, sock;
	char buffer[SIZE];
	char* ret;

	memset(&hints, 0, sizeof(hints));

	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_ADDRCONFIG;

	rc = getaddrinfo("localhost", NULL, &hints, &addr);
	if(rc != 0) {
		printf("Host name lookup failed: %s\n", gai_strerror(rc));
		exit(1);
	}

	addrinfo = (struct sockaddr_in *) addr->ai_addr;
	sock = socket(addrinfo->sin_family, addr->ai_socktype, addr->ai_protocol);
	if(sock < 0) {
		printf("Can't create socket\n");
		exit(1);
	}

	addrinfo->sin_port = htons(PORT);
	freeaddrinfo(addr);

	while(1) {
		for (int i = 0; i < 2; i++) {
			ret = fgets(buffer, SIZE, stdin);
			if(ret == NULL) break;
			sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr*) addrinfo, addr->ai_addrlen);
		}

		recvfrom(sock, buffer, SIZE, 0, NULL, NULL);
		printf("sum = %s\n", buffer);
	}

	close(sock);
	exit(0);
}