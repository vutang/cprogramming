#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* IP addresses are 4 bytes*/
#define IP_ADDR_LEN     4 

/* Ethernet header */
typedef struct eth_hdr {
    u_char  dst_mac[ETHER_ADDR_LEN];    /* destination mac address */
    u_char  src_mac[ETHER_ADDR_LEN];    /* source mac address */
    u_short ether_type;                 /* IP? ARP? RARP? etc */
} eth_hdr_t;

/* ARP Header, (assuming Ethernet+IPv4)            */ 
#define ARP_REQUEST 	1   		/* ARP Request             */ 
#define ARP_REPLY 		2   		/* ARP Reply               */
#define ARPHRD_ETHER	1
typedef struct arp_hdr { 
    u_int16_t htype;    			/* Hardware Type           */ 
    u_int16_t ptype;    			/* Protocol Type           */ 
    u_char hlen;        			/* Hardware Address Length */ 
    u_char plen;        			/* Protocol Address Length */ 
    u_int16_t oper;     			/* Operation Code          */ 
    u_char sha[ETHER_ADDR_LEN];     /* Sender hardware address */ 
    u_char spa[IP_ADDR_LEN];      	/* Sender IP address       */ 
    u_char tha[ETHER_ADDR_LEN];     /* Target hardware address */ 
    u_char tpa[IP_ADDR_LEN];      	/* Target IP address       */ 
} arp_hdr_t;


/* IP header */

#define IP_ICMP       1    /*ICMP*/
#define IP_TCP        6    /*TCP*/
#define IP_UDP        17   /*UDP*/
typedef struct ip_hdr {
    u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off;                 /* fragment offset field */
    #define IP_RF 0x8000            /* reserved fragment flag */
    #define IP_DF 0x4000            /* dont fragment flag */
    #define IP_MF 0x2000            /* more fragments flag */
    #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src,ip_dst;  /* source and dest address */
} ip_hdr_t;
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4) 

/*ICMP Header */
#define ICMP_ECHOREPLY      0       /* Echo Reply           */
#define ICMP_ECHO           8       /* Echo Request         */

//typedef struct icmp_hdr{
//    u_char type;
//    u_char code;
//    u_int16_t checksum;
//    union{
//        struct{
//            u_int16_t id;
//            u_int16_t sequence;
//        } echo;         /* echo datagram */
//        u_int32_t   gateway;    /* gateway address */
//        struct{
//            u_int16_t __unused;
//            u_int16_t mtu;
//        } frag;         /* path mtu discovery */
//    } un;
//} icmp_hdr_t;

typedef struct icmp_hdr {
    u_char type;
    u_char code;
    u_int16_t checksum;
    u_int16_t id;
    u_int16_t seq;
} icmp_hdr_t;

#define MAXBYTES2CAPTURE 		2048

typedef struct tcp_hdr {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        u_int32_t th_seq;               /* sequence number */
        u_int32_t th_ack;               /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
        #define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
} tcp_hdr_t;