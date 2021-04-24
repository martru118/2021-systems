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

#define PORT 1024

void packetCallback(u_char*, const struct pcap_pkthdr*, const u_char*);
void processIP(const u_char*);
void getPort(int, char*);

int main(int argc, char **argv) {
	char *device;
	char error_buffer[PCAP_ERRBUF_SIZE];

	pcap_t *handle;
	int packet_count = 0;
	int packet_timeout = 10000;

	//determine where the packets are coming from
	if(argc == 2) {
		handle = pcap_open_offline(argv[1], error_buffer);
		if(handle == NULL) {
			printf("error opening file: %s\n",error_buffer);
			exit(1);
		}
	} else {
		device = pcap_lookupdev(error_buffer);
		if(device == NULL) {
			printf("can't find device: %s\n", error_buffer);
			exit(1);
		}
		
		handle = pcap_open_live(device, BUFSIZ, 0, packet_timeout, error_buffer);
		if(handle == NULL) {
			printf("can't open device: %s\n", error_buffer);
			exit(1);
		}
	}

	//start the capture loop
	pcap_loop(handle, packet_count, packetCallback, NULL);
	pcap_close(handle);
}

void processIP(const u_char *packet) {
	struct iphdr* ip = (struct iphdr*) packet;
	struct tcphdr* tcp;
	struct udphdr* udp;

	u_char* payload;
	char* addr;
	unsigned int len;

	//check that we have an IPv4 packet
	if(ip->version != 4) {
		printf("not version 4\n");
		return;
	}

	len = ip->ihl * 4;
	printf("\tHeader is %d bytes\n", len);
	payload = (unsigned char*) packet + len;

	//call the TCP procedure here
	if(ip->protocol == IPPROTO_TCP) {
		printf("TCP packet\n");
		tcp = (struct tcphdr*) payload;
		
		//print flags
		int ack = (tcp->th_flags & TH_ACK);
		if(ack != 0) printf("\tACK flag is %d\n", ntohl(ack));
		else printf("\tACK flag not found\n");

		int syn = (tcp->th_flags & TH_SYN);
		if(syn != 0) printf("\t\tSYN flag is %d\n", ntohl(syn));
		else printf("\tSYN flag not found\n");

		int fin = (tcp->th_flags & TH_FIN);
		if(fin != 0) printf("\tFIN flag is %d\n", ntohl(fin));
		else printf("\tFIN flag not found\n");

		int rst = (tcp->th_flags & TH_RST);
		if(rst != 0) printf("\tRST flag is %d\n", ntohl(rst));
		else printf("\tRST flag not found\n");

		printf("Sequence number: %lu\n", ntohl(tcp->th_seq));
		printf("Acknowledgement number: %lu\n", ntohl(tcp->th_ack));

		//print source and destination ports
		getPort(tcp->th_sport, "Source");
		getPort(tcp->th_dport, "Destination");
	}

	//call the UDP procedure here
	if(ip->protocol == IPPROTO_UDP) {
		printf("UDP packet\n");
		udp = (struct udphdr*) payload;

		//print source and destination ports
		getPort(udp->uh_sport, "Source");
		getPort(udp->uh_dport, "Destination");
	}

	//print the source and destination addresses
	addr = (char*) &(ip->saddr);
	printf("\tSource address: %hhu.%hhu.%hhu.%hhu\n", addr[0], addr[1],addr[2], addr[3]);
	addr = (char*) &(ip->daddr);
	printf("\tDestination address: %hhu.%hhu.%hhu.%hhu\n", addr[0], addr[1],addr[2], addr[3]);
}

void packetCallback(u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {
	struct ether_header* eptr;
	short type;
	char* addr;

	printf("time: %s", ctime((const time_t*) &header->ts.tv_sec));
	eptr = (struct ether_header*) packet;

	type = ntohs(eptr->ether_type);
	if(type == ETHERTYPE_IP) {
		printf("IP packet\n");
		processIP(packet + 14);
	}

	if(type == ETHERTYPE_ARP) {
		printf("ARP packet\n");
	}

	printf("\n");
}

void getPort(int h_port, char* sd) {
	unsigned int p = ntohl(h_port);

	if(p < PORT) {
		printf("\t%s port: %d\n", sd, ntohs(h_port));
		struct servent* service = getservbyport(h_port, NULL);

		if(service != NULL) printf("\tService port %s\n", service->s_name);
		else printf("\tService port not found\n");
	} else {
		printf("\t%s port: %d\n", sd, ntohs(h_port));
	}
}