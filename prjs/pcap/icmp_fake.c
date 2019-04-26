#include <stdio.h>
#include <string.h>

#include "pktHdr.h"

/*Source and Destination Address*/
static unsigned char source_ip_addr[IP_ADDR_LEN] = {10, 61, 61, 99};
static unsigned char target_ip_addr[IP_ADDR_LEN];
static unsigned char source_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0x6d, 0x1f, 0xce};
static unsigned char target_mac_addr[ETHER_ADDR_LEN];
// static unsigned char target_mac_addr[ETHER_ADDR_LEN] = {0x34, 0x17, 0xeb, 0xce, 0xd2, 0x58};     // quandv8
// static unsigned char target_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0x81, 0xdb, 0xf8};     // khanhnp1
// static unsigned char target_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0xbb, 0x3d, 0xb4};     // sangnt11: b0:83:fe:bb:3d:b4
// static unsigned char target_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0x6d, 0x23, 0x40};     // linhtt15
// static unsigned char target_mac_addr[ETHER_ADDR_LEN] = {0xb0, 0x83, 0xfe, 0xbb, 0x36, 0x0b};    // fpga

// static unsigned char attk_ip_addr[IP_ADDR_LEN] = {10, 61, 61, 234};
struct in_addr target_ip;

// static u_char icmp_packet_tem[98];

/* Global variable*/
int got_arp_req_done = 0;
int got_arp_rep_done = 0;

typedef struct thread_arg {
    pcap_t *handle;
    u_char *packet;
    u_char faked_ip_addr[IP_ADDR_LEN];
} thread_arg_t;

void posix_error(int code, char *msg) /* posix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    exit(0);
}

void Pthread_detach(pthread_t tid) {
    int rc;

    if ((rc = pthread_detach(tid)) != 0)
    posix_error(rc, "Pthread_detach error");
}

void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
            void * (*routine)(void *), void *argp) 
{
    int rc;

    if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
    posix_error(rc, "Pthread_create error");
}

void Pthread_join(pthread_t tid, void **thread_return) {
    int rc;

    if ((rc = pthread_join(tid, thread_return)) != 0)
    posix_error(rc, "Pthread_join error");
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

void send_arp_packet(pcap_t *handle, u_char *sender_ip, u_char oper){

    eth_hdr_t eth;
    arp_hdr_t arp;
    u_char buffer[sizeof(eth_hdr_t) + sizeof(arp_hdr_t)];
    unsigned char gate_way_ip[4] = {10, 61, 61, 246};
    int i = 0;
    
    /* Construct Ethernet header (except for source MAC address) */
    eth.ether_type = htons(ETH_P_ARP);
    memcpy(&eth.src_mac, &source_mac_addr, ETHER_ADDR_LEN);     /* Source MAC */
    if (oper == 1)
        memset(&eth.dst_mac, 0xff, ETHER_ADDR_LEN);                 /* Destination MAC FF:FF:FF:FF:FF:FF for broadcast */
    else 
        memcpy(&eth.dst_mac, &target_mac_addr, ETHER_ADDR_LEN);


    /* Construct ARP request (except for MAC and IP addresses) */
    arp.htype   = htons(ARPHRD_ETHER);
    arp.ptype   = htons(ETH_P_IP);
    arp.hlen    = ETHER_ADDR_LEN;
    arp.plen    = sizeof(in_addr_t);
    arp.oper    = htons(oper);

    /*Target mac is broadcast address if arp is request, 
    in case of arp reply, target mac is attk mac*/
    if (oper == 1) {
        memset(&arp.tha, 0xff, sizeof(arp.tha));
    }
    else {
        memcpy(&arp.tha, &target_mac_addr, sizeof(arp.tha));
    }
    memcpy(&arp.tpa, &target_ip_addr, sizeof(arp.tpa));         /* Set target IP */ 
    memcpy(&arp.sha, &source_mac_addr, sizeof(arp.sha));        /* Set source MAC */
    memcpy(&arp.spa, sender_ip, sizeof(arp.spa));               /* Set source IP */

    /* Combine the Ethernet header and ARP request into a contiguous block */
    memcpy(buffer                    , &eth, sizeof(eth_hdr_t));
    memcpy(buffer + sizeof(eth_hdr_t), &arp, sizeof(arp_hdr_t));
    inject_packet(handle, buffer, sizeof(buffer));
    
    return;
}

/*Thread routine*/
void *process_arp_thread(void *vargv) {
    thread_arg_t *arg = (thread_arg_t *) vargv;
    Pthread_detach(pthread_self());
    // pcap_t *handle = arg->handle;
    pcap_t *handle;
    char *dev = "eth0";
    char errbuf[PCAP_ERRBUF_SIZE];      /* error buffer */

    u_char *packet = arg->packet;
    // u_char *faked_ip_addr = arg->faked_ip_addr;

    u_char faked_ip_addr[IP_ADDR_LEN];
    memcpy(faked_ip_addr, arg->faked_ip_addr, IP_ADDR_LEN);

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

    printf("process_arp_thread: send_arp_packet to %d.%d.%d.%d\n", faked_ip_addr[0], faked_ip_addr[1],
                                            faked_ip_addr[2], faked_ip_addr[3]);
    int i = 0;
    for (i=0; i < 5000; i++) {
        send_arp_packet(handle, faked_ip_addr, ARP_REPLY);
        usleep(500);
    }
    pcap_close(handle);
    return NULL;
}

void process_arp_packet(pcap_t *handle, u_char *packet) {
    arp_hdr_t arp;
    thread_arg_t thread_arg_1;
    pthread_t tid;

    memcpy(&arp, packet + SIZE_ETHERNET, sizeof(arp_hdr_t));
    if (ntohs(arp.oper) == ARP_REQUEST) {
        if (!memcmp(&arp.spa,target_ip_addr,IP_ADDR_LEN)) {
            // printf("\t\t\t: arp->spa %d.%d.%d.%d\n", arp->spa[0], arp->spa[1],
                                  // arp->spa[2], arp->spa[3]);
            // printf("\t\t\t: arp->tpa %d.%d.%d.%d\n", arp->tpa[0], arp->tpa[1],
                                  // arp->tpa[2], arp->tpa[3]);
            thread_arg_1.handle = handle;
            thread_arg_1.packet = packet;
            thread_arg_1.faked_ip_addr[0] = arp.tpa[0];
            thread_arg_1.faked_ip_addr[1] = arp.tpa[1];
            thread_arg_1.faked_ip_addr[2] = arp.tpa[2];
            thread_arg_1.faked_ip_addr[3] = arp.tpa[3];
            Pthread_create(&tid, NULL, process_arp_thread, &thread_arg_1);
            // Pthread_join(tid, NULL);
        }
    }
    return;
}

void process_arp_reply_packet (pcap_t *handle, u_char *packet) {
    arp_hdr_t arp;
    memcpy(&arp, packet + SIZE_ETHERNET, sizeof(arp_hdr_t));
    if (ntohs(arp.oper) == ARP_REPLY) {
        if (arp.spa[0] == target_ip_addr[0] && arp.spa[1] == target_ip_addr[1] &&
            arp.spa[2] == target_ip_addr[2] && arp.spa[3] == target_ip_addr[3]) {
            memcpy(&target_mac_addr, &arp.sha, ETHER_ADDR_LEN);
            got_arp_rep_done = 1;
        }
        else
            got_arp_rep_done = 0;
    }
    else 
        got_arp_rep_done = 0;
    return;
}

void send_icmp_reply_packet (pcap_t *handle, u_char *packet) {
    eth_hdr_t eth; 
    ip_hdr_t ip;
    icmp_hdr_t icmp;

    u_char mac_tmp[ETHER_ADDR_LEN];
    struct in_addr ip_add_tmp;

    time_t rawtime;
    time(&rawtime);

    memcpy(&eth, packet, sizeof(eth_hdr_t));
    memcpy(&ip, packet + SIZE_ETHERNET, sizeof(ip_hdr_t));
    memcpy(&icmp, packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), sizeof(icmp_hdr_t));

    int ip_pkt_len = ntohs(ip.ip_len);

    int icmp_pkt_len = ip_pkt_len - sizeof(ip_hdr_t);
    u_char icmp_pkt_buf[icmp_pkt_len];

    u_char ip_pkt_buf[ip_pkt_len];

    int eth_pkt_len = sizeof(eth_hdr_t) + ip_pkt_len;
    u_char eth_pkt_buf[eth_pkt_len];

    /*Build up icmp packet*/
    icmp.type = ICMP_ECHOREPLY;
    icmp.checksum = 0;
    memcpy(icmp_pkt_buf, &icmp, sizeof(icmp_hdr_t));
    memcpy(icmp_pkt_buf + sizeof(icmp_hdr_t), 
           packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t) + sizeof(icmp_hdr_t),
           icmp_pkt_len - sizeof(icmp_hdr_t));
    memcpy(icmp_pkt_buf + sizeof(icmp_hdr_t), &rawtime, sizeof(time_t));
    icmp.checksum = compute_ip_checksum(icmp_pkt_buf,icmp_pkt_len);
    memcpy(icmp_pkt_buf, &icmp, sizeof(icmp_hdr_t));

    /*Build up ip packet*/
    ip.ip_sum = 0;
    memcpy(&ip_add_tmp,&ip.ip_dst,sizeof(struct in_addr));
    memcpy(&ip.ip_dst,&ip.ip_src,sizeof(struct in_addr));
    memcpy(&ip.ip_src,&ip_add_tmp,sizeof(struct in_addr));
    memcpy(ip_pkt_buf, &ip, sizeof(ip_hdr_t));
    memcpy(ip_pkt_buf + sizeof(ip_hdr_t), icmp_pkt_buf, ip_pkt_len - sizeof(ip_hdr_t));
    ip.ip_sum = compute_ip_checksum(ip_pkt_buf,ip_pkt_len);
    memcpy(ip_pkt_buf, &ip, sizeof(ip_hdr_t));

    /*Interchange MAC*/
    memcpy(mac_tmp, eth.dst_mac, ETHER_ADDR_LEN);
    memcpy(eth.dst_mac, eth.src_mac, ETHER_ADDR_LEN);
    memcpy(eth.src_mac, mac_tmp, ETHER_ADDR_LEN);

    /*Completed Ethernet Packet*/
    memcpy(eth_pkt_buf, &eth, sizeof(eth_hdr_t));
    memcpy(eth_pkt_buf + sizeof(eth_hdr_t), ip_pkt_buf, ip_pkt_len);

    inject_packet(handle, eth_pkt_buf, sizeof(eth_pkt_buf));
    usleep(500);

    return;
}

void process_icmp_packet(pcap_t *handle, u_char *packet) {
    icmp_hdr_t icmp;

    memcpy(&icmp, packet + sizeof(eth_hdr_t) + sizeof(ip_hdr_t), sizeof(icmp_hdr_t));
    if (icmp.type == ICMP_ECHO) {
        printf("Call send icmp reply function\n");
        send_icmp_reply_packet(handle, packet);
    }

    return;
}

void process_ip_packet(pcap_t *handle, u_char *packet) {
    ip_hdr_t ip;

    memcpy(&ip, packet + SIZE_ETHERNET, sizeof(ip_hdr_t));
    if (ip.ip_p == IP_ICMP) {
        if (!memcmp(&ip.ip_src.s_addr,target_ip_addr,4)) {
            process_icmp_packet(handle, packet);
        }
    }
    return;
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, u_char *packet){
    int ether_type; 
    eth_hdr_t eth;

    /*Define the ethernet header*/
    memcpy(&eth, packet, sizeof(eth_hdr_t));
    ether_type = ntohs(eth.ether_type);
    // printf("ethernet\n");
    if (!got_arp_rep_done) {
        if (ether_type == ETH_P_ARP)
            process_arp_reply_packet((pcap_t *)args, packet);
    }
    else if (!memcmp(&eth.src_mac, target_mac_addr, ETHER_ADDR_LEN)) {
        switch(ether_type) {
            case ETH_P_IP:
                process_ip_packet((pcap_t *)args, packet);
                break;
            case ETH_P_ARP:
                process_arp_packet((pcap_t *)args, packet);
                break;
            default:
                break;
        }
    }
    return;
}

int main(int argc, char **argv) {

    /*For pcap*/
    char *dev = NULL;           /* capture device name */
    char errbuf[PCAP_ERRBUF_SIZE];      /* error buffer */
    pcap_t *handle;             /* packet capture handle */
    struct pcap_pkthdr pkthdr;
    unsigned char *packet = NULL;
    // struct bpf_program fp;            /* compiled filter program (expression) */
    // char filter[] = "ether";
    // int num_packets = 100;
    bpf_u_int32 mask;           /* subnet mask */
    bpf_u_int32 net;            /* ip */

    /*For counter*/
    int i = 0;

    /*For pthread*/
    pthread_t tid;
    thread_arg_t *thread_arg_1;

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

    /* compile the filter expression */
    // if (pcap_compile(handle, &fp, filter, 0, net) == -1) {
    //  fprintf(stderr, "Couldn’t parse filter \"%s\": %s\n",
    //  filter, pcap_geterr(handle));
    //  exit(EXIT_FAILURE);
    // }

    /* apply the compiled filter */
    // if (pcap_setfilter(handle, &fp) == -1) {
    //  fprintf(stderr, "Couldn’t install filter %s: %s\n",
    //  filter, pcap_geterr(handle));
    //  exit(EXIT_FAILURE);
    // }

    /*Got ip target from argument*/
    struct in_addr target_ip = {0};
    if (!inet_aton(argv[2], &target_ip)) {
        printf("%s is not a valid ip address\n", argv[2]);
        return 0;
    }
    memcpy(&target_ip_addr, &target_ip.s_addr, IP_ADDR_LEN);
    printf("Got target ip address: %d.%d.%d.%d\n", target_ip_addr[0], target_ip_addr[1], 
                                            target_ip_addr[2], target_ip_addr[3]);

    /*Send ARP for got target mac address*/
    printf("Send ARP REQUEST to target\n");
    for (i = 0; i < 5; i++) {
        send_arp_packet(handle, source_ip_addr, ARP_REQUEST);
    }
    printf("Wait for ARP REPLY\n");
    while (!got_arp_rep_done) {

        if( (packet = (u_char *) pcap_next(handle,&pkthdr)) == NULL){  /* Get one packet */ 
            printf("ERROR: Error getting the packet.\n");
            exit(1);
        }   
        got_packet((u_char *) handle,&pkthdr,packet);
    }
    printf("Got target mac address: %x:%x:%x:%x:%x:%x\n", target_mac_addr[0], target_mac_addr[1],
                                                   target_mac_addr[2], target_mac_addr[3],
                                                   target_mac_addr[4], target_mac_addr[5]);

    while (1) {
        if( (packet = (u_char *) pcap_next(handle,&pkthdr)) == NULL){  /* Get one packet */ 
            printf("ERROR: Error getting the packet.\n");
            exit(1);
        }   
        got_packet((u_char *) handle,&pkthdr,packet);
    }

    pcap_close(handle); 
    return 0;
}

