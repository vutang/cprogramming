/******************************************************************************
 * ctcp.c
 * ------
 * Implementation of cTCP done here. This is the only file you need to change.
 * Look at the following files for references and useful functions:
 *   - ctcp.h: Headers for this file.
 *   - ctcp_iinked_list.h: Linked list functions for managing a linked list.
 *   - ctcp_sys.h: Connection-related structs and functions, cTCP segment
 *                 definition.
 *   - ctcp_utils.h: Checksum computation, getting the current time.
 *
 *****************************************************************************/

#include "ctcp.h"
#include "ctcp_linked_list.h"
#include "ctcp_sys.h"
#include "ctcp_utils.h"
#include <wait.h>
#include <time.h>

/*Status define*/
#define IDLE              0x0001 
#define SEND_DATA         0x0002 
#define RESENDING         0x0004 
#define SEND_FIN_1        0x0008 
#define SEND_FIN_2        0x0010
#define GET_ACK           0x0020
#define GET_EOF           0x0040
#define GET_FIN           0x0080
#define FINISH            0x0100

/*Others macro*/
#define SEGMENT_IS_EXPECTED(A, B) (A == B) ? 1 : 0

/**
 * Connection state.
 *
 * Stores per-connection information such as the current sequence number,
 * unacknowledged packets, etc.
 *
 * You should add to this to store other fields you might need.
 */

struct ctcp_state {
  struct ctcp_state *next;  /* Next in linked list */
  struct ctcp_state **prev; /* Prev in linked list */

  conn_t *conn;             /* Connection object -- needed in order to figure
                               out destination when sending */
  linked_list_t *segments;  /* Linked list of segments sent to this connection.
                               It may be useful to have multiple linked lists
                               for unacknowledged segments, segments that
                               haven't been sent, etc. Lab 1 uses the
                               stop-and-wait protocol and therefore does not
                               necessarily need a linked list. You may remove
                               this if this is the case for you */
  
  /* For update ack-seq number
  Make sure this numbers are in network-byte order*/
  uint32_t seqno;              /* Current sequence number */
  uint32_t ackno;              /* Current ack number */

  /*For push payload to stdout*/
  char data[MAX_SEG_DATA_SIZE];
  uint16_t payload_len;

  /*Status of State*/
  uint16_t      status;
  uint16_t      retransmit_time;
  uint16_t      number_in_list;

  /*For Go-Back-N*/
  uint32_t received_ackno;
  uint32_t base_seqno;
  uint16_t free_sent_seg;

  uint16_t recv_window;
  uint16_t send_window;
};

/**
 * Linked list of connection states. Go through this in ctcp_timer() to
 * resubmit segments and tear down connections.
 */
static ctcp_state_t *state_list;

/*Struct and linked list for sliding window*/
struct segment_node {
  uint32_t seqno;
  ctcp_segment_t segment;
  char data[MAX_SEG_DATA_SIZE];
  int data_len;
};
typedef struct segment_node segment_node_t;

/*General variables*/
int timer_counter = 0;
int start_timer =0;
int number_of_state = 0;

clock_t begin, end;

ctcp_state_t *ctcp_init(conn_t *conn, ctcp_config_t *cfg) {
  /* Connection could not be established. */
  if (conn == NULL) {
    return NULL;
  }

  /* Established a connection. Create a new state and update the linked list
     of connection states. */
  ctcp_state_t *state = calloc(sizeof(ctcp_state_t), 1);
  state->next = state_list;
  state->prev = &state_list;
  state->number_in_list = number_of_state;
  fprintf(stderr, "Initing state: %d .....\n", state->number_in_list);
  number_of_state++;
  if (state_list)
    state_list->prev = &state->next;
  state_list = state;

  /* Set fields. */
  state->conn = conn;  
  state->seqno = htonl(1);
  state->ackno = htonl(1);
  state->status = IDLE;
  state->retransmit_time = 0;

  /*For Go Back N*/
  state->received_ackno = htonl(1);
  state->base_seqno = 1;
  state->free_sent_seg = 0;
  state->recv_window = cfg->recv_window;
  state->send_window = cfg->send_window;

  /*Creating a linked list for store sent segments*/
  state->segments = ll_create();
  begin = clock();
  return state;
}

void ctcp_destroy(ctcp_state_t *state) {
  /* Update linked list. */
  fprintf(stderr, "Destroying state %d\n", state->number_in_list);
  if (state->next)
    state->next->prev = state->prev;
  *state->prev = state->next;
  conn_remove(state->conn);
  ll_destroy(state->segments);
  free(state);
  fprintf(stderr, "[INFO] Runtime: %lf\n", (double)(end - begin)/CLOCKS_PER_SEC);
  end_client();
}

/*Print Segment information*/
void print_segment(ctcp_segment_t *segment) {
  fprintf(stderr, "[vINFO] seqno: %d, ", ntohl(segment->seqno));
  fprintf(stderr, "ackno: %d, ",ntohl(segment->ackno));
  fprintf(stderr, "len: %d, ",(int)(ntohs(segment->len)-  sizeof(ctcp_segment_t)));
  fprintf(stderr, "flags: %s, ", (segment->flags == TH_ACK) ? "ACK" : "OTHERs");
  fprintf(stderr, "window: %d, ", ntohs(segment->window));
  fprintf(stderr, "cksum: %x\n",ntohs(segment->cksum));
}

void make_segment(ctcp_segment_t *segment_to_send, 
                  uint32_t seqno, uint32_t ackno, uint16_t len, uint32_t flags, uint16_t window, char* buf) {
  /*Build up ACK*/
  segment_to_send->seqno = seqno;
  segment_to_send->ackno = ackno;
  segment_to_send->flags |= htonl(flags);
  segment_to_send->len = htons(sizeof(ctcp_segment_t) + len);
  segment_to_send->window = window;
  if (len > 0)
    memcpy(segment_to_send->data, buf, len);
  segment_to_send->cksum = 0;
  segment_to_send->cksum = cksum(segment_to_send,ntohs(segment_to_send->len));
}

void free_sent_segment(ctcp_state_t *state) {
  if (state->segments->head->next != NULL) {
    ll_node_t *node_tmp = state->segments->head;
    segment_node_t *data_node = calloc(sizeof(segment_node_t), 1);
    memcpy(data_node, (segment_node_t *) node_tmp->object, sizeof(segment_node_t));
    // fprintf(stderr, "freeing state->segments->head: seqno %d\n", ntohl(data_node->seqno));
    state->segments->head = state->segments->head->next;
    free(node_tmp);

    while ((ntohl(data_node->seqno) < state->base_seqno - 1440) && (state->segments->head->next != NULL)) {
      ll_node_t *node_tmp = state->segments->head;
      memcpy(data_node, (segment_node_t *) node_tmp->object, sizeof(segment_node_t));
      // fprintf(stderr, "freeing state->segments->head: seqno %d\n", ntohl(data_node->seqno));
      state->segments->head = state->segments->head->next;
    }
    free(data_node);
  }
  return;
}

/*ctcp_state_t: connection state,
  flags: segment flags
  buf: data to send
  len: length of buf*/

int ctcp_send(ctcp_state_t *state, uint32_t flags, char *buf, int len) {
  /*Building up segment to send*/
  ctcp_segment_t *segment = calloc(sizeof(ctcp_segment_t) + len, 1);
  int r_send_len = 0;

  /*Making segment to send*/
  make_segment(segment, state->seqno, state->ackno, len,
               flags, htons(MAX_SEG_DATA_SIZE), buf);

  /*Sending segment*/
  if ((r_send_len = conn_send(state->conn, segment, 
                              sizeof(ctcp_segment_t) + len)) < 0) {
    fprintf(stderr, "Cannot send segment\n");
    return -1;
  }

  state->status &= ~GET_ACK;

  /*Adding sent segment to linked list*/
  segment_node_t *node = calloc(sizeof(segment_node_t), 1);
  node->seqno = segment->seqno;
  memcpy(&node->segment, segment, sizeof(ctcp_segment_t));
  memcpy(node->data, buf, len);
  node->data_len = len;
  if (ll_add(state->segments, (void *) node) == NULL) {
    fprintf(stderr, "Cannot add node to linked list\n");
  }

  /*Freeing segment*/
  free(segment);
  return r_send_len;
}

void ctcp_read(ctcp_state_t *state) {
  if (!(state->status & (RESENDING | GET_EOF | SEND_FIN_1 | SEND_FIN_2)) && 
        (htonl(state->seqno) < (state->base_seqno + state->send_window))) {
    state->retransmit_time = 0;
    char buf[MAX_SEG_DATA_SIZE];
    int r_read_len = 0, r_send_len = 0;
    
    /*Reading data from STDIN*/
    r_read_len = conn_input(state->conn, buf, MAX_SEG_DATA_SIZE);

    /*Send data*/
    if (r_read_len > 0) {
      if ((r_send_len = ctcp_send(state, 0, buf, r_read_len)) < 0) {
        fprintf(stderr, "Cannot send segment\n");
      }
      state->seqno = htonl(ntohl(state->seqno) + r_send_len - sizeof(ctcp_segment_t));
      /*Updating state*/  
      state->status |= SEND_DATA;
    }
    else if (r_read_len < 0) {
      state->status |= GET_EOF;
      state->status &= ~SEND_DATA;
      start_timer = 0;
    }
  }
  return;
}

void ctcp_send_ack(ctcp_state_t *state, ctcp_segment_t *segment, uint32_t fin) {
  // fprintf(stderr, "Sending ack to state %d.....\n", state->number_in_list);
  int r_send_len = 0;
  uint32_t ackno;
  ctcp_segment_t *segment_to_send = calloc(sizeof(ctcp_segment_t), 1);
  if (fin) 
    ackno =  htonl(ntohl(segment->seqno) + 1);
  else 
    ackno = state->ackno;
  /*Build up ACK*/
  make_segment(segment_to_send, state->seqno, ackno,
              0, ACK, segment->window, NULL);
  /*Send ACK segment*/
  if ((r_send_len = conn_send(state->conn, segment_to_send, 
                              sizeof(ctcp_segment_t))) < 0) {
    fprintf(stderr, "Cannot send segment\n");
  }
  free(segment_to_send);
  return;
}

int segment_is_corrupted(ctcp_segment_t *segment, size_t len) {
  ctcp_segment_t *segment_to_cksum = calloc(sizeof(ctcp_segment_t) + len, 1);
  int ret;
  memcpy(segment_to_cksum, segment, len);
  segment_to_cksum->cksum = 0;
  ret = ((segment->cksum) != cksum(segment_to_cksum, ntohs(segment_to_cksum->len)));
  free(segment_to_cksum);
  return ret;
}

void handle_ack_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  /*Detect acknownedge segment*/ 
  if ((state->status & (SEND_DATA | GET_EOF | RESENDING))) {
    // fprintf(stderr, "Handling ack segment, seqno (received_ackno): %d, ackno: %d, base: %d\n", 
            // ntohl(segment->seqno), ntohl(segment->ackno), state->base_seqno);
    // print_segment(segment);
    
    if ((state->base_seqno < ntohl(segment->ackno))) {
      state->base_seqno = ntohl(segment->ackno);
      start_timer = 0;
      state->free_sent_seg = 1;
      state->status |= GET_ACK;
    }

    state->received_ackno = segment->ackno;
  }
  else if (state->status & SEND_FIN_1) {
    fprintf(stderr, "Get ACK for SEND_FIN_1....\n");
    // print_segment(segment);
    state->status |= GET_ACK;
  }
  else if (state->status & SEND_FIN_2) {
    fprintf(stderr, "Get ACK for SEND_FIN_2....\n");
    state->status |= FINISH;
  }
  return;
}

void handle_data_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  /*Call ctcp_output to put received data into STDOUT
  Before put data out, need to check checksum is right or not*/
  // fprintf(stderr, "Handling data segment, state->ackno: %d, state->base_seqno: %d\n", ntohl(state->ackno), state->base_seqno);   
  // print_segment(segment);
  
  /*Discard any segments with lower seqno than recent recent segment*/
  if (SEGMENT_IS_EXPECTED(segment->seqno, state->ackno) == 1) {
    /* Got data and push into a memory to print out in conn_output*/
    memcpy(state->data,segment->data,ntohs(segment->len)-sizeof(ctcp_segment_t));
    state->payload_len = ntohs(segment->len) -  sizeof(ctcp_segment_t);
    
    /*Update state*/  
    state->ackno = htonl(ntohl(state->ackno) + ntohs(segment->len) 
                   - sizeof(ctcp_segment_t));

    /*Printing to STDOUT*/
    ctcp_output(state);
  }
  ctcp_send_ack(state, segment, 0);
}

void handle_fin_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  ctcp_send_ack(state, segment, FIN);
  // fprintf(stderr, "Got FIN segment and Sending FIN ACK\n");
  state->status |= GET_FIN;
}

void ctcp_receive(ctcp_state_t *state, ctcp_segment_t *segment, size_t len) {
  if (segment_is_corrupted(segment, len) == 0) {
    // fprintf(stderr, "Receiving a segment......\n");
    if ((segment->flags & TH_ACK)) {
      handle_ack_segment(state, segment);
    }

    if ((ntohs(segment->len) - sizeof(ctcp_segment_t)) > 0) { 
      handle_data_segment(state, segment);
    }

    if (segment->flags & TH_FIN) {
      handle_fin_segment(state, segment);
    }
  }
  free(segment);
  return;
}

void ctcp_output(ctcp_state_t *state) {
  char *data = calloc(state->payload_len,1);
  size_t w_availble_nmb = 0;
  int r_output_len;
  // fprintf(stderr, "--------------------------------------------------------\n");
  if ((w_availble_nmb = conn_bufspace(state->conn)) == 0) {
    fprintf(stderr, "There are no room in STDOUT\n");
    return;
  }

  memcpy(data, state->data, state->payload_len);
  if ((r_output_len = conn_output(state->conn, data, state->payload_len) < 0)) {
    fprintf(stderr, "Cannot send to STDOUT\n");
  }     
  // fprintf(stderr, "--------------------------------------------------------\n");
  return;
}

/*Resending last packet when cannot recceive ack or time out*/
void ctcp_resend(ctcp_state_t *state) {
  // fprintf(stderr, "Resending (retransmit time: %d) .....\n", state->retransmit_time);
  int r_send_len;
  ll_node_t *node = state->segments->head;
  segment_node_t *node_data = calloc(sizeof(segment_node_t), 1);
  memcpy(node_data, (segment_node_t *) node->object, sizeof(segment_node_t));
  while ((state->base_seqno != ntohl(node_data->seqno)) && (node->next != NULL)){
    // fprintf(stderr, "Scanning %d\n", ntohl(node_data->seqno));
    node = node->next;
    memcpy(node_data, (segment_node_t *) node->object, sizeof(segment_node_t));
  }

  while (node != NULL) {
    memcpy(node_data, (segment_node_t *) node->object, sizeof(segment_node_t));
    // fprintf(stderr, "Resend seqno %d\n", ntohl(node_data->seqno));
    ctcp_segment_t *segment_to_send = calloc(sizeof(ctcp_segment_t) + node_data->data_len, 1);
    make_segment(segment_to_send, node_data->seqno, node_data->segment.ackno,
                node_data->data_len,0,node_data->segment.window, node_data->data);
    // print_segment(segment_to_send);
    /*Resend last segment*/
    if ((r_send_len = conn_send(state->conn, (ctcp_segment_t *) segment_to_send, 
                               sizeof(ctcp_segment_t) + node_data->data_len) < 0)) {
        fprintf(stderr, "Cannot send segment\n");
        return;
    }
    free(segment_to_send);
    node = node->next;
  }
  free(node_data);
  state->retransmit_time++;
  state->status &= ~GET_ACK;
  return;
}

int ctcp_send_fin (ctcp_state_t *state) {
  ctcp_segment_t *segment = calloc(sizeof(ctcp_segment_t), 1);
  make_segment(segment, 0, 0, 0, FIN, htons(MAX_SEG_DATA_SIZE), NULL);
  /*Send segment*/
  if (conn_send(state->conn, segment, 
                              sizeof(ctcp_segment_t)) < 0) {
    fprintf(stderr, "Cannot send segment\n");
    return -1;
  }
  return 0;
}

void handle_retransmittion(ctcp_state_t *state) {
  if (((state->base_seqno == ntohl(state->received_ackno)) && (state->base_seqno < ntohl(state->seqno))) 
      && !(state->status & FINISH)) {
    if ((state->retransmit_time >= 5)) {
      /*Terminate transaction*/
      fprintf(stderr, "Call destroy state because of over retransmit_time %d\n", state->number_in_list);
      state->status |= FINISH;
    } 
    else {
      if (start_timer == 0) 
        start_timer = 1;
      else {
        state->status |= RESENDING;
        ctcp_resend(state);  
        state->status &= ~RESENDING;
        start_timer = 0;
      }
    }
  }
  else if ((state->status & (SEND_FIN_2 | SEND_FIN_1)) && !(state->status & GET_ACK)) {
    if (start_timer == 0) 
        start_timer = 1;
    else {
      fprintf(stderr, "Resend %s\n", (state->status & SEND_FIN_1) ? "SEND_FIN_1" : 
                                     (state->status & SEND_FIN_2) ? "SEND_FIN_2" :"???");
      if (ctcp_send_fin(state) < 0) {
        fprintf(stderr, "Canot send FIN again\n");
      }
      start_timer = 0;
    }
  }
  return;
}

/*When getting an EOF signal, waitting for all segments are ack-ed*/
void handle_fin(ctcp_state_t *state) {
  /*Destroying state when status is FINISH*/
  if (state->status & FINISH) {
    end = clock();
    ctcp_destroy(state);
  }
  else if ((state->status & GET_EOF) && !(state->status & (SEND_FIN_1 | SEND_FIN_2 | GET_FIN))) {
    if ((state->base_seqno == ntohl(state->seqno)) && (state->base_seqno == ntohl(state->received_ackno))) {
      if (start_timer == 0) 
        start_timer = 1;
      else {
        fprintf(stderr, "SEND_FIN_1 (base_seqno: %d, seqno: %d)\n", state->base_seqno, ntohl(state->seqno));
        if (ctcp_send_fin(state) < 0)
          return;
        state->status = SEND_FIN_1;
        // state->status &= ~(GET_EOF | SEND_DATA | GET_ACK);
        start_timer = 0;
      }
    }
  }
  else if ((state->status & GET_FIN) && !(state->status & (SEND_FIN_1 | SEND_FIN_2))) {
    if ((state->base_seqno == ntohl(state->seqno)) && (state->base_seqno == ntohl(state->received_ackno))) {
      fprintf(stderr, "SEND_FIN_2....\n");
      if (ctcp_send_fin(state) < 0)
        return;
      state->status = SEND_FIN_2;
      start_timer = 0;
      // state->status &= ~GET_FIN;
    }
  }
  else if ((state->status & GET_FIN) && (state->status & SEND_FIN_1)) {
    fprintf(stderr, "SEND_FIN_1 is done and get FIN -> FINISH\n");
    if (start_timer == 0) 
        start_timer = 1;
    else {
      state->status |= FINISH;
      start_timer = 0;
    }
  }
  return;
}

void ctcp_timer() {
  ctcp_state_t *state = state_list, *next;
  if (timer_counter == 4) {
    while (state) {
      next = state->next;
      handle_retransmittion(state);
      if ((state->free_sent_seg == 1) && !(state->status & RESENDING)) {
        free_sent_segment(state);
        state->free_sent_seg = 0;
      }
      handle_fin(state);
      state = next;
    }
    timer_counter = 0;
  }
  else 
    timer_counter ++;
  return;
}
