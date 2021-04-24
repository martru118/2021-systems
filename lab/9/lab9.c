#include <stdio.h>
#include <time.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <stdlib.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

void packetCallback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int main(int argc, char **argv) {
	char *device;
	char error_buffer[PCAP_ERRBUF_SIZE];

	pcap_t *handle;
	int packet_count = 25;
	int packet_timeout = 10000;

	if(argc == 2) {
		handle = pcap_open_offline(argv[1], error_buffer);
		if(handle == NULL) {
			printf("error opening file: %s\n",error_buffer);
			exit(1);
		}
	} else {
		device = pcap_lookupdev(error_buffer);
		if(device == NULL) {
			printf("Can't find device: %s\n", error_buffer);
			exit(1);
		}
		
		handle = pcap_open_live(device, BUFSIZ, 0, packet_timeout, error_buffer);
		if(handle == NULL) {
			printf("can't open device: %s\n", error_buffer);
			exit(1);
		}
	}

	pcap_loop(handle, packet_count, packetCallback, NULL);
	pcap_close(handle);
}

void processIP(const u_char *packet) {
	struct iphdr *ip;
	
	u_char *payload;
	char *addr;
	unsigned int len;

	ip = (struct iphdr*) packet;
	if(ip->version != 4) {
		printf("not version 4\n");
		return;
	}

	len = ip->ihl * 4;
	printf("header length: %d\n", len);
	payload = (unsigned char*)packet+len;

	if(ip->protocol == IPPROTO_TCP) {
		printf("TCP packet\n");
		// call the TCP procedure here
	}

	if(ip->protocol == IPPROTO_UDP) {
		printf("UDP packet\n");
		// call the UDP procedure here
	}

	addr = (char*) &(ip->saddr);
	printf("\tsource: %hhu.%hhu.%hhu.%hhu\n",addr[0], addr[1],addr[2], addr[3]);
	addr = (char*) &(ip->daddr);
	printf("\tdestination: %hhu.%hhu.%hhu.%hhu\n",addr[0], addr[1],addr[2], addr[3]);

	printf("Time alive: %d\n", ip->ttl);
	printf("ID: %d\n", ntohs(ip->id));

	int reserved = (ip->frag_off & IP_RF);
	if (reserved != 0) printf("Reserved fragment flag is %d\n", reserved);
	else printf("Reserve fragment flag not found.\n");

	int dont = (ip->frag_off & IP_DF);
	if (dont != 0) printf("Dont fragment flag is %d\n", dont);
	else printf("Dont fragment flag not found.\n");

	int more = (ip->frag_off & IP_MF);
	if (more != 0) printf("More fragment flag is %d\n", more);
	else printf("More fragment flag not found.\n");
}

void packetCallback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	struct ether_header *eptr;
	short type;
	char* addr;

	printf("time: %s", ctime((const time_t*) &header->ts.tv_sec));
	eptr = (struct ether_header*) packet;

	addr = (char*) &(eptr->ether_shost);
	printf("\tsource: %hhu.%hhu.%hhu.%hhu\n",addr[0], addr[1],addr[2], addr[3]);
	addr = (char*) &(eptr->ether_dhost);
	printf("\tdestination: %hhu.%hhu.%hhu.%hhu\n",addr[0], addr[1],addr[2], addr[3]);

	type = ntohs(eptr->ether_type);
	if(type == ETHERTYPE_IP) {
		printf("IP packet\n");
		processIP(packet+14);
	}

	if(type == ETHERTYPE_ARP) {
		printf("arp packet\n");
	}

	printf("\n");
}