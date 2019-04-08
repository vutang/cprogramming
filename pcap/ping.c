#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <time.h>

#include "pktHdr.h"

/*Source and Destination Address*/
static unsigned char source_ip_addr[IP_ADDR_LEN];
static unsigned char target_ip_addr[IP_ADDR_LEN];
static unsigned char source_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0x6d, 0x1f, 0xce};
static unsigned char target_mac_addr[ETHER_ADDR_LEN];

struct in_addr target_ip;

static u_char icmp_packet_tem[98];

/* Global variable*/
int got_target_mac_done = 0;
int got_an_icmp_rep_packet_done = 0;
int icmp_seq = 0;


/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const u_char *payload, int len, int offset) {

	int i;
	int gap;
	const u_char *ch;

	/* offset */
	printf("%05d   ", offset);
	
	/* hex */
	ch = payload;
	for(i = 0; i < len; i++) {
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid */
		if (i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8 bytes */
	if (len < 8)
		printf(" ");
	
	/* fill hex gap with spaces if not full line */
	if (len < 16) {
		gap = 16 - len;
		for (i = 0; i < gap; i++) {
			printf("   ");
		}
	}
	printf("   ");
	
	/* ascii (if printable) */
	ch = payload;
	for(i = 0; i < len; i++) {
		if (isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");
		ch++;
	}

	printf("\n");

return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void print_payload(const u_char *payload, int len) {

	int len_rem = len;
	int line_width = 16;			/* number of bytes per line */
	int line_len;
	int offset = 0;					/* zero-based offset counter */
	const u_char *ch = payload;

	if (len <= 0)
		return;

	/* data fits on one line */
	if (len <= line_width) {
		print_hex_ascii_line(ch, len, offset);
		return;
	}

	/* data spans multiple lines */
	for ( ;; ) {
		/* compute current line length */
		line_len = line_width % len_rem;
		/* print line */
		print_hex_ascii_line(ch, line_len, offset);
		/* compute total remaining */
		len_rem = len_rem - line_len;
		/* shift pointer to remaining bytes to print */
		ch = ch + line_len;
		/* add offset */
		offset = offset + line_width;
		/* check if we have line width chars or less */
		if (len_rem <= line_width) {
			/* print last line and get out */
			print_hex_ascii_line(ch, len_rem, offset);
			break;
		}
	}

return;
}

/* Inject Packet Procedure*/
void inject_packet(pcap_t *handle, char *packet, int len) {

	if (pcap_inject(handle, packet, len) == -1) {
    	    pcap_perror(handle,0);
    	    pcap_close(handle);
    	    return;
    }
    return;
}

unsigned compute_ip_checksum(const void *vptr, unsigned nbytes) {

	int sum, oddbyte;
	const unsigned short *ptr = vptr;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1) {
		oddbyte = 0;
		((u_char *)&oddbyte)[0] = *(u_char *)ptr;
		((u_char *)&oddbyte)[1] = 0;
		sum += oddbyte;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	sum = ~sum & 0xffff;

	return (sum);
}

void process_arp_packet(pcap_t *handle, const u_char *packet) {
	arp_hdr_t *arp = NULL;

	arp = (arp_hdr_t *) (packet + SIZE_ETHERNET);
	if (ntohs(arp->oper) == ARP_REPLY) {
		printf("Process ARP\t: got a reply packet\n");
		printf("\t\t: spa is %d.%d.%d.%d\n", arp->spa[0], arp->spa[1],
													arp->spa[2], arp->spa[3]);
		if (strcmp(arp->spa, target_ip_addr)) {
			printf("\t\t: matched spa %d.%d.%d.%d\n", arp->spa[0], arp->spa[1],
								  arp->spa[2], arp->spa[3]);
			strcpy(target_mac_addr, arp -> sha);
			got_target_mac_done = 1;
		}
		else
			got_target_mac_done = 0;
	}
	else 
		got_target_mac_done = 0;
	return;
}

void process_icmp_request_packet(pcap_t *handle, int seq, time_t rawtime){

	eth_hdr_t eth;
	ip_hdr_t ip;
	icmp_hdr_t icmp;
	u_char mac_tmp[ETHER_ADDR_LEN];
	// u_char icmp_payload[48] = {0x32,0xa9,0x64,0x57,0x00,0x00,0x00,0x00,
	// 						   0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	// 						   0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	// 					  	   0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
	// 					  	   0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	// 					  	   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37};

	u_char icmp_payload[48];
	int len__ = sizeof(icmp_hdr_t) + 48;
	u_char buf_icmp_packet[len__];

	int len_ = len__ + sizeof(ip_hdr_t);
	u_char buf_ip_packet[len_];

	
	int len = len_ + sizeof(eth_hdr_t);
	// int len =sizeof(eth_hdr_t) + sizeof(ip_hdr_t) + sizeof(icmp_hdr_t);
	u_char buf[len];	

	// process icmp packet	
	// memcpy(buf_icmp_packet, packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), len__);

	icmp.type = 8;
	icmp.checksum = 0;
	icmp.seq = htons(seq);
	icmp.id = htons(0x2146);
	icmp.code = 0;

	memcpy(icmp_payload,&rawtime,sizeof(time_t));
	memcpy(buf_icmp_packet,&icmp,sizeof(icmp_hdr_t));
	memcpy(buf_icmp_packet + sizeof(icmp_hdr_t), icmp_payload, 48);
	icmp.checksum = (compute_ip_checksum(buf_icmp_packet,len__));
	memcpy(buf_icmp_packet, &icmp, sizeof(icmp_hdr_t));

	// process ip packet
	ip.ip_vhl = 0x45;
	ip.ip_tos = 0x00;
	ip.ip_len = htons(sizeof(ip_hdr_t) + sizeof(icmp_hdr_t) + 48);
	ip.ip_id = htons(0x10c5);
	ip.ip_off = 0;
	ip.ip_ttl = 64;
	ip.ip_p = 1;
	ip.ip_sum = 0;
	// change IP src and dst
	// memcpy(buf_ip_packet, packet + sizeof(eth_hdr_t), len_);
	struct in_addr ip_addr; 
	if(!inet_aton("10.61.61.99", &ip_addr)){
		printf("s is not a valid ip address\n");
		// return 0;
	}
	ip.ip_src = ip_addr;
	// ip.ip_dst = target_ip;
	memcpy(&ip.ip_dst, target_ip_addr, IP_ADDR_LEN);

	memcpy(buf_ip_packet, &ip, sizeof(ip_hdr_t));
	memcpy(buf_ip_packet + sizeof(ip_hdr_t), buf_icmp_packet, len_ - sizeof(ip_hdr_t));
	// memcpy(buf_ip_packet + sizeof(ip_hdr_t), buf_icmp_packet, len_ - sizeof(ip_hdr_t));

	ip.ip_sum = (compute_ip_checksum(buf_ip_packet,len_));
	memcpy(buf_ip_packet, &ip, sizeof(ip_hdr_t));
	// memcpy(buf_ip_packet + sizeof(ip_hdr_t), packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), len_ - sizeof(ip_hdr_t));
	
	// Process etherner header 
	memcpy(eth.dst_mac,target_mac_addr,ETHER_ADDR_LEN);
	memcpy(eth.src_mac,source_mac_addr,ETHER_ADDR_LEN);	
	eth.ether_type = htons(0x0800);

	memcpy(buf, &eth, sizeof(eth_hdr_t));
	memcpy(buf + sizeof(eth_hdr_t), buf_ip_packet, len_);
	// memcpy(buf + sizeof(eth_hdr_t), &ip, sizeof(ip_hdr_t));
	// memcpy(buf + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), &icmp, sizeof(icmp_hdr_t));
	
	inject_packet(handle, buf, len);

	usleep(500);
	// got_an_icmp_req_packet_done ++;
	// return;
}

void process_icmp_reply_packet(pcap_t *handle, const u_char *packet, int seq) {
	eth_hdr_t eth;
	ip_hdr_t ip;
	icmp_hdr_t icmp;
	time_t rawtime;
	time(&rawtime);

	// u_char reply_src_ip[IP_ADDR_LEN];

	memcpy(&ip, packet + sizeof(eth_hdr_t), sizeof(ip_hdr_t));
	memcpy(&icmp, packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), sizeof(icmp_hdr_t));
	// reply_src_ip = inet_ntoa(ip.ip_src);
	if (strcmp(target_ip_addr, inet_ntoa(ip.ip_src)) 
		&& (icmp.type == ICMP_ECHOREPLY)
		&& (icmp.seq == htons(seq))) {
		printf("\tReply\t\t: seq=%d/%d\ttime=%s", ntohs(icmp.seq), icmp.seq, ctime(&rawtime));
		got_an_icmp_rep_packet_done = 1;
	}

}

void process_ip_packet(pcap_t *handle, const u_char *packet) {
	ip_hdr_t ip;
	icmp_hdr_t icmp;
	memcpy(&ip, packet + sizeof(eth_hdr_t), sizeof(ip_hdr_t));

	int len = ntohs(ip.ip_len);
	u_char buf[len];

	u_char ip_protocol = ip.ip_p;

	switch(ip_protocol) {
		case IP_ICMP: 
			process_icmp_reply_packet(handle, packet, icmp_seq);
			break;
		default:
			break;
	}
	
	// printf("procesip_ip\t: ip packet protocol is %d\n",ip.ip_p);
	// printf("\t\tsrc ip is %s\n", inet_ntoa(ip.ip_src));
	// printf("\t\tdst ip is %s\n", inet_ntoa(ip.ip_dst));
	// printf("\t\tcur chksum is \t0x%x\n", ntohs(ip.ip_sum));
	// printf("%d\n", ntohs(ip.ip_len));
	
	// memcpy(&icmp, packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), sizeof(icmp_hdr_t));
	// printf("\t\ticmp type: %x\n", (icmp.type));
	// memcpy(&icmp_packet_tem, packet, 98);
	// got_an_icmp_req_packet_done = 1;

	// ip.ip_sum = 0;	
	// memcpy(buf, &ip, sizeof(ip_hdr_t));
	// memcpy(buf + sizeof(ip_hdr_t),packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), len - sizeof(ip_hdr_t));
	// printf("\t\tmy chksum is \t0x%x\n",ntohs(compute_ip_checksum(buf,len)));
	return;
}

void got_packet(pcap_t *handle, const u_char *packet) {
	int ether_type; 
	eth_hdr_t *eth = NULL;

	/*Define the ethernet header*/
	eth = (eth_hdr_t *) (packet);
	ether_type = ntohs(eth->ether_type);

	switch(ether_type) {
		case ETH_P_IP:
			process_ip_packet(handle, packet);
			break;
		case ETH_P_ARP:
			if (!got_target_mac_done) {
				process_arp_packet(handle, packet);
			}
			else
				break;
			break;
		default:
			break;
	}
	return;
}

void send_arp_packet(pcap_t *handle, u_char *target_ip){

	eth_hdr_t eth;
	arp_hdr_t arp;
	u_char buffer[sizeof(eth_hdr_t) + sizeof(arp_hdr_t)];
	unsigned char gate_way_ip[4] = {10, 61, 61, 246};
	int i = 0;
	
	/* Construct Ethernet header (except for source MAC address) */
	eth.ether_type = htons(ETH_P_ARP);
	memcpy(&eth.src_mac, &source_mac_addr, ETHER_ADDR_LEN);		/* Source MAC */
	memset(&eth.dst_mac, 0xff, ETHER_ADDR_LEN);					/* Destination MAC FF:FF:FF:FF:FF:FF for broadcast */

	/* Construct ARP request (except for MAC and IP addresses) */
	arp.htype 	= htons(ARPHRD_ETHER);
	arp.ptype 	= htons(ETH_P_IP);
	arp.hlen	= ETHER_ADDR_LEN;
	arp.plen 	= sizeof(in_addr_t);
	arp.oper 	= htons(ARP_REQUEST);
	// arp.oper 	= htons(ARP_REPLY);
	memset(&arp.tha, 0, sizeof(arp.tha));

	memcpy(&arp.tpa, target_ip 	 , sizeof(arp.tpa));			/* Set target IP  */
	memcpy(&arp.spa, &gate_way_ip, sizeof(arp.spa));
	//memcpy(&arp.spa, &source_ip_addr , sizeof(arp.spa));		/* Set source IP  */
	memcpy(&arp.sha, &source_mac_addr, sizeof(arp.sha));		/* Set source MAC */

	/* Combine the Ethernet header and ARP request into a contiguous block */
	memcpy(buffer					 , &eth, sizeof(eth_hdr_t));
    memcpy(buffer + sizeof(eth_hdr_t), &arp, sizeof(arp_hdr_t));

    inject_packet(handle, buffer, sizeof(buffer));
    
    return;
}

int main(int argc, char **argv) {

	char *dev = NULL;			/* capture device name */
	char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
	pcap_t *handle;				/* packet capture handle */
	struct pcap_pkthdr pkthdr;
	const unsigned char *packet = NULL;

	bpf_u_int32 mask;			/* subnet mask */
	bpf_u_int32 net;			/* ip */
	int i;

	time_t rawtime; 
	struct tm *timeinfo;
	// u_char rawtime_a[4];

	if (argc != 3) {
		printf("Usage: vping [interface] [target ip address]\n");
		exit(EXIT_FAILURE);
	}
	/* get network number and mask associated with capture device */
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
		    dev, errbuf);
		net = 0;
		mask = 0;
	}

	/* open capture device */
	dev = argv[1];
	handle = pcap_open_live(dev, SNAP_LEN, 0, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		exit(EXIT_FAILURE);
	}

	/* make sure we're capturing on an Ethernet device [2] */
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "%s is not an Ethernet\n", dev);
		exit(EXIT_FAILURE);
	}
	/*Got ip target from argument*/
	// struct in_addr target_ip = {0};
	if (!inet_aton(argv[2], &target_ip)) {
		printf("%s is not a valid ip address\n", target_ip);
		return 0;
	}
	memcpy(&target_ip_addr, &target_ip.s_addr, IP_ADDR_LEN);
	/*Begin sending arp request*/
	// for(i = 0; i < IP_ADDR_LEN; i++){
	// 	send_arp_packet(handle, target_ip_addr);
	// 	usleep(100);
	// }

	/*Send ARP for got target mac address*/
	while (!got_target_mac_done) {
		for(i = 0; i < 2; i++){
			send_arp_packet(handle, target_ip_addr);
			usleep(50);
		}

		if( (packet = pcap_next(handle,&pkthdr)) == NULL){  /* Get one packet */ 
		    printf("ERROR: Error getting the packet.\n");
		    exit(1);
		}	

		got_packet(handle, packet);	
	}
	printf("Got Target MAC done!\n");
	printf("Target MAC is %x:%x:%x:%x:%x:%x\n", 
		   target_mac_addr[0],target_mac_addr[1],target_mac_addr[2],
		   target_mac_addr[3],target_mac_addr[4],target_mac_addr[5]);

	for (i = 0; i < 5; i ++) {
		time(&rawtime);
		icmp_seq = i;
		// printf("Sending icmp request\n");
		process_icmp_request_packet(handle, icmp_seq, htonl(rawtime));
		printf("Ping %s\t: seq=%d/%d\ttime=%s", argv[2],icmp_seq,htons(icmp_seq),ctime(&rawtime));
		while (got_an_icmp_rep_packet_done == 0) {
			if( (packet = pcap_next(handle,&pkthdr)) == NULL){  /* Get one packet */ 
			    printf("ERROR: Error getting the packet.\n");
			    exit(1);
			}	

			got_packet(handle, packet);
		}
		got_an_icmp_rep_packet_done = 0;		
	}	
	pcap_close(handle); 
	return 0;
}
