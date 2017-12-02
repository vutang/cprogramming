/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <assert.h>


#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"
#include "vnscommand.h"

#include <stdlib.h>
#include <string.h>
#include <netinet/if_ether.h> //for ETH_P_ARP, ETH_P_IP
#include <net/if_arp.h>

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/
#define IP_ICMP       1    /*ICMP*/
#define IP_TCP        6    /*TCP*/
#define IP_UDP        17   /*UDP*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* Add initialization code here! */
} /* -- sr_init -- */

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);

  printf("*** -> Received packet of length %d, interface: %s \n",len, interface);

  sr_ethernet_hdr_t *ether_hdr = (sr_ethernet_hdr_t *) packet;

  // print_hdr_eth((uint8_t *) ether_hdr);
  switch(ntohs(ether_hdr->ether_type)) {
    case ETH_P_IP:
      sr_handle_ip_packet(sr, packet, len, interface);
      break;
    case ETH_P_ARP:
      sr_handle_arp_packet(sr, packet, len, interface);
      break;
    default:
      break;
  }
} /* end sr_ForwardPacket */

void sr_handle_arp_packet(struct sr_instance* sr,
                          uint8_t * packet, 
                          unsigned int len, 
                          char* interface) {

  sr_arp_hdr_t *arp_hdr = calloc(1, sizeof(sr_arp_hdr_t));  

  memcpy(arp_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_arp_hdr_t));
  // print_hdr_arp((uint8_t *) arp_hdr);

  switch(ntohs(arp_hdr->ar_op)) {
    case ARPOP_REQUEST:
      sr_handle_arp_req(sr, packet, len, interface);
      break;
    case ARPOP_REPLY:
      sr_handle_arp_rep(sr, packet, len, interface);
      break;
    default:
      break;
  }
  
}

void sr_handle_arp_req(struct sr_instance* sr,
                          uint8_t * packet, 
                          unsigned int len, 
                          char* interface) {

  struct sr_if *iface = sr->if_list;

  sr_arp_hdr_t *arp_hdr = calloc(1, sizeof(sr_arp_hdr_t));  
  memcpy(arp_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_arp_hdr_t));

  /*Send ARP Reply when a host asks router interface MAC*/
  while (iface) {
      if (arp_hdr->ar_tip == iface->ip) {
          /*Send reply*/
          sr_send_arp(sr, iface, ARPOP_REPLY, arp_hdr->ar_sip, arp_hdr->ar_sha);
          return;
      }
      iface = iface->next;
  }
  return;
}

/*
  Handle ARP Reply
  # When servicing an arp reply that gives us an IP->MAC mapping
  req = arpcache_insert(ip, mac)

  if req:
    send all packets on the req->packets linked list
    arpreq_destroy(req)
 */
void sr_handle_arp_rep(struct sr_instance* sr,
                          uint8_t * packet, 
                          unsigned int len, 
                          char* interface) {
  struct sr_arpreq *entry_ret;
  sr_arp_hdr_t *arp_hdr = calloc(1, sizeof(sr_arp_hdr_t));  
  memcpy(arp_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_arp_hdr_t));

  struct sr_packet *ret_pkt;
  uint8_t *ip_buf;
  sr_ip_hdr_t ip_hdr;

  struct sr_rt *routing_table;
  struct sr_if *outgoing_iface; 

  /*Add MAC to ARP Cache*/
  if ((entry_ret = sr_arpcache_insert(&sr->cache, (unsigned char *) &arp_hdr->ar_sha[0], 
                                arp_hdr->ar_sip)) != NULL) {
    /*Send all packet in this request*/
    ret_pkt = entry_ret->packets;
    while (ret_pkt) {   
      routing_table = sr_lookup_rtable(sr, entry_ret->ip);
      outgoing_iface = sr_get_interface(sr, (char *) &routing_table->interface[0]);
      ip_buf = calloc(1, ret_pkt->len - sizeof(sr_ethernet_hdr_t));

      memcpy(ip_buf, ret_pkt->buf + sizeof(sr_ethernet_hdr_t), 
             ret_pkt->len - sizeof(sr_ethernet_hdr_t));
      memcpy(&ip_hdr, ip_buf, sizeof(sr_ip_hdr_t));
      ip_hdr.ip_sum = 0;
      ip_hdr.ip_ttl--;

      memcpy(ip_buf,&ip_hdr,sizeof(sr_ip_hdr_t));
      // ip_hdr.ip_sum = cksum(ip_buf,ret_pkt->len-sizeof(sr_ethernet_hdr_t));
      ip_hdr.ip_sum = cksum(ip_buf,sizeof(sr_ip_hdr_t));
      memcpy(ip_buf,&ip_hdr,sizeof(sr_ip_hdr_t));

      printf("Resend---------------\n");
      // print_hdr_ip(ip_buf);
      sr_send_ether_packet(sr, outgoing_iface, &arp_hdr->ar_sha[0],
                          ETH_P_IP, ip_buf,
                          ret_pkt->len-sizeof(sr_ethernet_hdr_t));
      ret_pkt = ret_pkt->next;
      free(ip_buf);
    }
  sr_arpreq_destroy(&sr->cache, entry_ret);
  }
}

void sr_handle_ip_packet(struct sr_instance* sr,
                          uint8_t* packet, 
                          unsigned int len, 
                          char* interface) {
  sr_ip_hdr_t *ip_hdr = (sr_ip_hdr_t *) (packet + sizeof(sr_ethernet_hdr_t));
  // print_hdr_ip((uint8_t *) ip_hdr);
  uint16_t ip_sum = ip_hdr->ip_sum;
  ip_hdr->ip_sum = 0;

  struct sr_if *iface_walker = sr->if_list;

  /*Check packet: is large enough and has a correct checksum*/
  if (len < sizeof(sr_ip_hdr_t) + sizeof(sr_ethernet_hdr_t)) {
    printf("Packet is not large enough...\n");
    free(packet);
    return;
  }

  uint8_t *buf = calloc(1, len - sizeof(sr_ethernet_hdr_t));
  memcpy(buf, packet + sizeof(sr_ethernet_hdr_t), len - sizeof(sr_ethernet_hdr_t));
  memcpy(buf, ip_hdr, sizeof(sr_ip_hdr_t));

  // if (ip_sum != cksum(buf, ntohs(ip_hdr->ip_len))) {
  if (ip_sum != cksum(buf, sizeof(sr_ip_hdr_t))) {
    printf("Checksum is incorrect..\n");
    fprintf(stderr, "\tchecksum: 0x%04x\n", cksum(buf, len - sizeof(sr_ethernet_hdr_t)));
    return;
  }

  /*Decrease TTL*/
  ip_hdr->ip_ttl = ip_hdr->ip_ttl - 1;
  printf("IP Packet Time to Live: %d\n", (int) ip_hdr->ip_ttl);
  memcpy(packet + sizeof(sr_ethernet_hdr_t), ip_hdr, sizeof(sr_ip_hdr_t));

  /*Check TTL, if TTL == 0, send ICMP Time Exceeded (type 11, code 0)*/
  if (ip_hdr->ip_ttl == 0) {
    sr_handle_ttl_expired(sr, interface, packet, len);
    return;
  }

  while (iface_walker) {
    if (ip_hdr->ip_dst == iface_walker->ip) {
      /*Handle internal interface IP packet*/
      sr_handle_internal_packet(sr, packet, len, interface, iface_walker);
      return;
    }
    iface_walker = iface_walker->next;
  }

  /*Forwarding*/
  struct sr_arpentry* entry;
  if ((entry = sr_arpcache_lookup(&sr->cache, ip_hdr->ip_dst)) != NULL) {
    printf("Entry found()....\n");
    print_addr_eth((uint8_t *) entry->mac);
    /*Forward Packet*/
    sr_ip_forwarding(sr, packet, len, entry);
  }
  else {
    printf("Entry not found()....\n");
    sr_arpcache_queuereq(&sr->cache, ip_hdr->ip_dst, packet, len, interface);
  }
}

void sr_handle_internal_packet(struct sr_instance* sr,
                               uint8_t* packet, 
                               unsigned int len, 
                               char* incomming_iface,
                               struct sr_if *iface) {
  sr_ip_hdr_t *ip_hdr = (sr_ip_hdr_t *) (packet + sizeof(sr_ethernet_hdr_t));
  switch(ip_hdr->ip_p) {
    case IP_ICMP:
      sr_handle_internal_icmp_packet(sr, packet, len, incomming_iface, iface);
      break;
    default:
      break;
  }
}

void sr_handle_internal_icmp_packet(struct sr_instance* sr,
                                    uint8_t* packet,  
                                    unsigned int len, 
                                    char* incomming_iface_name,
                                    struct sr_if *iface) {
  
  printf("ICMP packet send to interface: %s\n", (char *) &iface->name[0]);
  sr_ethernet_hdr_t *ether_hdr = calloc(1, sizeof(sr_ethernet_hdr_t));
  sr_ip_hdr_t *ip_hdr = calloc(1, sizeof(sr_ip_hdr_t));
  sr_icmp_hdr_t *icmp_hdr = calloc(1, sizeof(sr_icmp_hdr_t));
  
  memcpy(ether_hdr, packet, sizeof(sr_ethernet_hdr_t));
  memcpy(ip_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_ip_hdr_t));
  memcpy(icmp_hdr, packet + sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t), 
         sizeof(sr_icmp_hdr_t));  

  struct sr_if *incomming_iface = sr_get_interface(sr, incomming_iface_name);
  uint8_t *payload;
  time_t rawtime;

  switch (icmp_hdr->icmp_type) {
    case (0): /*Echo Reply*/
      break; 
    case (8): /*Echo Request*/ 
      /*Reply*/
      time(&rawtime);
      memcpy(&ether_hdr->ether_dhost[0], &ether_hdr->ether_shost[0], ETHER_ADDR_LEN);
      memcpy(&ether_hdr->ether_shost[0], &incomming_iface->addr[0], ETHER_ADDR_LEN);
      ether_hdr->ether_type = htons(ETH_P_IP);

      ip_hdr->ip_dst = ip_hdr->ip_src;
      ip_hdr->ip_src = iface->ip;
      ip_hdr->ip_sum = 0;
      ip_hdr->ip_ttl = 64;

      icmp_hdr->icmp_type = 0; /*Reply*/
      icmp_hdr->icmp_sum = 0;

      payload = calloc(1, len - sizeof(sr_ethernet_hdr_t) - sizeof(sr_ip_hdr_t) - sizeof(sr_icmp_hdr_t));
      memcpy(payload, packet + (sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t) + sizeof(sr_icmp_hdr_t)), 
            len - sizeof(sr_ethernet_hdr_t) - sizeof(sr_ip_hdr_t) - sizeof(sr_icmp_hdr_t));
      memcpy(payload, &rawtime, sizeof(time_t));

      sr_send_icmp_packet(sr, incomming_iface, ether_hdr, ip_hdr, icmp_hdr, payload, 
                          len - sizeof(sr_ethernet_hdr_t) - sizeof(sr_ip_hdr_t) - sizeof(sr_icmp_hdr_t));
      free(payload);
      break;
    default:
      printf("Default\n");
      break;
  }

  free(ether_hdr);
  free(ip_hdr);
  free(icmp_hdr);
}

void sr_send_icmp_packet(struct sr_instance *sr,
                         struct sr_if *interface,
                         sr_ethernet_hdr_t *ether_hdr,
                         sr_ip_hdr_t *ip_hdr,
                         sr_icmp_hdr_t *icmp_hdr,
                         uint8_t *payload,
                         int len) { 

  /*Build up icmp packet: header + payload, calculate checksum*/
  uint8_t *icmp_buf = calloc(1, sizeof(sr_icmp_hdr_t) + len);
  memcpy(icmp_buf, icmp_hdr, sizeof(sr_icmp_hdr_t));
  memcpy(icmp_buf + sizeof(sr_icmp_hdr_t), payload, len);

  icmp_hdr->icmp_sum = cksum(icmp_buf, sizeof(sr_icmp_hdr_t) + len);
  memcpy(icmp_buf, icmp_hdr, sizeof(sr_icmp_hdr_t));

  /*Build up ip packet*/
  uint8_t *ip_buf = calloc(1, sizeof(sr_ip_hdr_t) + sizeof(sr_icmp_hdr_t) + len);
  memcpy(ip_buf, (uint8_t *) ip_hdr, sizeof(sr_ip_hdr_t));
  memcpy(ip_buf + sizeof(sr_ip_hdr_t), (uint8_t *) icmp_buf, sizeof(sr_icmp_hdr_t) + len);
  ip_hdr->ip_sum = cksum(ip_buf, sizeof(sr_ip_hdr_t));
  memcpy(ip_buf, (uint8_t *) ip_hdr, sizeof(sr_ip_hdr_t));

  /*Build ip ethernet packet*/
  uint8_t *ether_buf = calloc(1, sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t) 
                                 + sizeof(sr_icmp_hdr_t) + len);
  memcpy(ether_buf, (uint8_t *) ether_hdr, sizeof(sr_ethernet_hdr_t));
  memcpy(ether_buf + sizeof(sr_ethernet_hdr_t), (uint8_t *) ip_buf, sizeof(sr_ip_hdr_t) 
          + sizeof(sr_icmp_hdr_t) + len);

  print_hdr_eth((uint8_t *) ether_hdr);
  print_hdr_ip((uint8_t*) ip_hdr);
  print_hdr_icmp((uint8_t*) icmp_hdr);

  sr_send_ether_packet(sr, interface, &ether_hdr->ether_dhost[0], ETH_P_IP, ip_buf, 
                       sizeof(sr_ip_hdr_t) + sizeof(sr_icmp_hdr_t) + len);
}
/* Ip-forwarding
    - Sanity-check the packet (meets minimum length and has correct checksum)
    - Decrement the TTL by 1, and recompute the packet checksum over the modified header
    - Find out which entry in the routing table has the longest prefix match with 
    the destination IP address
    - Check the ARP cache for the next-hop MAC address corresponding to the next-hop IP. 
    If it's there, send it. Otherwise, send an ARP request for the next-hop IP (if one 
    hasn't been sent within the last second), and add the packet to the queue of packets 
    waiting on this ARP request
*/
void sr_ip_forwarding(struct sr_instance* sr,
                      uint8_t * packet, 
                      unsigned int len, 
                      struct sr_arpentry *entry) {
  
  struct sr_rt *routing_table = calloc(1, sizeof(struct sr_rt));
  routing_table = sr->routing_table;
  struct sr_if* interface; 

  /*Rebuild packet*/
  sr_ip_hdr_t ip_hdr;

  uint8_t *ip_buf = calloc(1, len - sizeof(sr_ethernet_hdr_t));

  memcpy(ip_buf, packet + sizeof(sr_ethernet_hdr_t), len - sizeof(sr_ethernet_hdr_t));
  memcpy(&ip_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_ip_hdr_t));

  ip_hdr.ip_sum = 0; /*Set ip_sum to 0 to calculate ip header chksum*/
  // ip_hdr.ip_ttl--; /*Decrease TTL by 1*/
  memcpy(ip_buf, &ip_hdr, sizeof(sr_ip_hdr_t));
  ip_hdr.ip_sum = cksum(ip_buf, sizeof(sr_ip_hdr_t));
  memcpy(ip_buf, &ip_hdr, sizeof(sr_ip_hdr_t));

  routing_table = sr_lookup_rtable(sr, entry->ip);
  interface = sr_get_interface(sr, (char *) &routing_table->interface[0]);

  sr_send_ether_packet(sr, interface, &entry->mac[0], ETH_P_IP,
                       ip_buf, len - sizeof(sr_ethernet_hdr_t));  
  free(ip_buf);
}

void sr_send_ether_packet(struct sr_instance* sr, 
                          struct sr_if* interface,
                          uint8_t *ether_dhost,
                          uint16_t ether_type,
                          uint8_t *packet,
                          unsigned int len) {

  sr_ethernet_hdr_t ether_hdr;
  memcpy(&ether_hdr.ether_shost[0], &interface->addr[0], ETHER_ADDR_LEN);
  memcpy(&ether_hdr.ether_dhost[0], ether_dhost, ETHER_ADDR_LEN);
  ether_hdr.ether_type = ntohs(ether_type);
  int ret;

  uint8_t *ether_buf = calloc(1, sizeof(sr_ethernet_hdr_t) + len);
  memcpy(ether_buf, &ether_hdr, sizeof(sr_ethernet_hdr_t));
  memcpy(ether_buf + sizeof(sr_ethernet_hdr_t), packet, len);

  if ((ret = sr_send_packet(sr, ether_buf, 
                            sizeof(sr_ethernet_hdr_t) + len, &interface->name[0])) < 0) {
    printf("Can not send packet....\n");
  }
  free(ether_buf);
}

/*Send ARP packet*/
void sr_send_arp(struct sr_instance *sr, 
                 struct sr_if *iface, 
                 unsigned short ar_op,
                 uint32_t target_ip,
                 uint8_t dhost[ETHER_ADDR_LEN]) {
    
    /*Build up ARP Header*/
    sr_arp_hdr_t arp_hdr;
    arp_hdr.ar_hrd = htons(1);
    arp_hdr.ar_pro = htons(0x0800);
    arp_hdr.ar_hln = ETHER_ADDR_LEN;
    arp_hdr.ar_pln = 4;
    arp_hdr.ar_op = htons(ar_op);
    memcpy(&arp_hdr.ar_sha, &iface->addr[0], ETHER_ADDR_LEN);
    arp_hdr.ar_sip = iface->ip;
    memcpy(&arp_hdr.ar_tha, &dhost[0], ETHER_ADDR_LEN);
    arp_hdr.ar_tip = target_ip;

    /*Prepare packet*/
    sr_send_ether_packet(sr, iface, &dhost[0], ETH_P_ARP,
                          (uint8_t *)&arp_hdr, sizeof(sr_arp_hdr_t));
}

void sr_handle_ttl_expired (struct sr_instance *sr,
                            char *iface_name,
                            uint8_t *packet,
                            unsigned int len) {

  sr_ethernet_hdr_t *ether_hdr = calloc(1, sizeof(sr_ethernet_hdr_t));
  sr_ip_hdr_t *ip_hdr = calloc(1, sizeof(sr_ip_hdr_t));
  sr_icmp_hdr_t *icmp_hdr = calloc(1, sizeof(sr_icmp_hdr_t));

  memcpy(ether_hdr, packet, sizeof(sr_ethernet_hdr_t));
  memcpy(ip_hdr, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_ip_hdr_t));
  memcpy(icmp_hdr, packet + sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t), 
         sizeof(sr_icmp_hdr_t));

  uint8_t ether_addr_tmp[ETHER_ADDR_LEN];
  uint8_t *payload = calloc(1, 4 + sizeof(sr_ip_hdr_t) + 8);

  struct sr_if *incomming_iface = sr_get_interface(sr, iface_name);

  memcpy(&ether_addr_tmp[0], ether_hdr->ether_dhost, ETHER_ADDR_LEN);
  memcpy(ether_hdr->ether_dhost, ether_hdr->ether_shost, ETHER_ADDR_LEN);
  memcpy(ether_hdr->ether_shost, &ether_addr_tmp[0], ETHER_ADDR_LEN);

  ip_hdr->ip_dst = ip_hdr->ip_src;
  ip_hdr->ip_src = incomming_iface->ip;
  ip_hdr->ip_p = 1;
  ip_hdr->ip_sum = 0;
  ip_hdr->ip_ttl = 64;

  icmp_hdr->icmp_type = 11; /*ICMP Time Exceeded*/
  icmp_hdr->icmp_code = 0;
  icmp_hdr->icmp_sum = 0;

  memcpy(payload + 4, packet + sizeof(sr_ethernet_hdr_t), sizeof(sr_ip_hdr_t) + 8);
  memcpy(payload + 4, ip_hdr, sizeof(sr_ip_hdr_t));
  printf("Send ICMP Time Exceeded\n");
  sr_send_icmp_packet(sr, incomming_iface, ether_hdr, ip_hdr, icmp_hdr, payload, 4 + sizeof(sr_ip_hdr_t) + 8);
}