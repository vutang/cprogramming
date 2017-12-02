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

/*Status define*/
#define IDLE              0x0001 
#define WAIT_ACK          IDLE << 1
#define SEND_FIN_1        IDLE << 2
#define SEND_FIN_2        IDLE << 3
#define GET_EOF           IDLE << 4
#define GET_FIN           IDLE << 5
#define FINISH            IDLE << 6

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
  
  /* For update ack-seq number
  Make sure this numbers are in network-byte order*/
  uint32_t seqno;              /* Current sequence number */
  uint32_t next_seqno;         /* Sequence number of next segment to send */
  uint32_t ackno;              /* Current ack number */

  /*For push payload to stdout*/
  uint32_t recent_seqno_received;
  char data[MAX_SEG_DATA_SIZE];
  int payload_len;

  /*For retransmition*/
  ctcp_segment_t *last_segment;
  char *last_segment_data;
  int last_data_len;

  /*Status of State*/
  int      status;
  int      retransmit_time;
  int      number_in_list;
};

/**
 * Linked list of connection states. Go through this in ctcp_timer() to
 * resubmit segments and tear down connections.
 */
static ctcp_state_t *state_list;

int timer_counter = 0;
int start_timer = 0;
int number_of_state = 0;

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
  // fprintf(stderr, "Initing state: %d .....\n", number_of_state);
  number_of_state++;
  if (state_list)
    state_list->prev = &state->next;
  state_list = state;

  /* Set fields. */
  state->conn = conn;
  
  state->seqno = htonl(1);
  state->next_seqno = htonl(1);
  state->ackno = htonl(1);

  state->recent_seqno_received = 0;

  state->last_segment = calloc(sizeof(ctcp_segment_t),1);
  state->last_segment_data = calloc(MAX_SEG_DATA_SIZE, 1);
  state->last_data_len = 0;

  state->status = IDLE;
  state->retransmit_time = 0;
  return state;
}

void ctcp_destroy(ctcp_state_t *state) {
  /* Update linked list. */
  
  if (state->next)
    state->next->prev = state->prev;

  *state->prev = state->next;
  state->status = IDLE;
  fprintf(stderr, "%s\n", "Free conn");
  conn_remove(state->conn);
  free(state->last_segment);
  free(state->last_segment_data);
  free(state);
  fprintf(stderr, "Destroying state....\n");

  
  /* FIXME: Do any other cleanup here. */
  
  end_client();
  return;
}

/*Print Segment information*/
void print_segment(ctcp_segment_t *segment) {
  fprintf(stderr, "[vINFO] seqno: %d, ", ntohl(segment->seqno));
  fprintf(stderr, "ackno: %d, ",ntohl(segment->ackno));
  fprintf(stderr, "len: %d, ",(int)(ntohs(segment->len)-  sizeof(ctcp_segment_t)));
  fprintf(stderr, "flags: %s, ", (segment->flags == TH_ACK) ? "ACK" : (segment->flags == TH_FIN) 
                                                            ? "FIN" : "OTHERs");
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


/*ctcp_state_t: connection state,
  flags: segment flags
  buf: data to send
  len: length of buf*/

int ctcp_send(ctcp_state_t *state, uint32_t flags, char *buf, int len) {
  /*Build up segment to send*/
  ctcp_segment_t *segment = calloc(sizeof(ctcp_segment_t) + len, 1);
  int r_send_len = 0;

  make_segment(segment, state->next_seqno, state->ackno, len,
               flags, htons(MAX_SEG_DATA_SIZE), buf);

  /*Send segment*/
  if ((r_send_len = conn_send(state->conn, segment, 
                              sizeof(ctcp_segment_t) + len)) < 0) {
    fprintf(stderr, "Cannot send segment\n");
    return -1;
  }
  /*Save last segment has been sent*/
  memcpy(state->last_segment,segment,sizeof(ctcp_segment_t));
  if (len > 0)
    memcpy(state->last_segment_data,buf,len);
  state->last_data_len = len;

  /*Update state*/
  state->seqno = state->next_seqno;
  state->next_seqno = htonl(ntohl(state->seqno) + r_send_len - sizeof(ctcp_segment_t));
  free(segment);
  return r_send_len;
}

void ctcp_read(ctcp_state_t *state) {
  if (((state->status & IDLE) | !(state->status & WAIT_ACK)) && !(state->status & GET_EOF)) {
    char buf[MAX_SEG_DATA_SIZE];
    int r_read_len = 0;
    r_read_len = conn_input(state->conn,buf,MAX_SEG_DATA_SIZE);
    if (r_read_len > 0) {
      /*Send data*/
      if (ctcp_send(state, 0, buf, r_read_len) < 0) {
        fprintf(stderr, "Cannot send segment\n");
        return;
      }
      state->retransmit_time = 0;
      state->status = WAIT_ACK;
    }
    else if (r_read_len < 0) {
      state->status |= GET_EOF;
      start_timer = 0;
    }
  }
  return;
}

void ctcp_send_ack(ctcp_state_t *state, ctcp_segment_t *segment, uint32_t fin) {
  // fprintf(stderr, "Sending ack to state %d.....\n", state->number_in_list);
  uint16_t ackno;
  ctcp_segment_t *segment_to_send = calloc(sizeof(ctcp_segment_t), 1);
  if (fin) 
    ackno =  htonl(ntohl(segment->seqno) + 1);
  else 
    ackno = htonl(ntohs(segment->len) - sizeof(ctcp_segment_t) + ntohl(state->ackno));
  /*Build up ACK*/
  make_segment(segment_to_send, state->next_seqno, ackno, 0, ACK, segment->window, NULL);
  /*Send ACK segment*/
  if ((conn_send(state->conn, segment_to_send, sizeof(ctcp_segment_t))) < 0) {
    fprintf(stderr, "Cannot send segment\n");
    return;
  }

  /*Update state*/  
  state->ackno = htonl(ntohl(state->ackno) + ntohs(segment->len) - sizeof(ctcp_segment_t));
  free(segment_to_send);
  return;
}

void handle_data_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  /*Call ctcp_output to put received data into STDOUT*/
  fprintf(stderr, "Processing a data segment...\n");
  memcpy(state->data,segment->data,ntohs(segment->len)-sizeof(ctcp_segment_t));
  state->payload_len = ntohs(segment->len)-  sizeof(ctcp_segment_t);

  if ((ntohl(segment->seqno) > state->recent_seqno_received)) {
    fprintf(stderr, "Push data to output...\n");
    ctcp_output(state);
  }

  ctcp_send_ack(state, segment, 0);
  state->recent_seqno_received = htonl(segment->seqno);
  return;
}

void handle_ack_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  if (state->status & WAIT_ACK) {
    fprintf(stderr, "Processing a ACK segment......\n");
    state->status &= ~WAIT_ACK;
  }
  return;
}

void handle_fin_segment (ctcp_state_t *state, ctcp_segment_t *segment) {
  ctcp_send_ack(state, segment, FIN);
  fprintf(stderr, "Got FIN segment and Sending FIN ACK\n");
  state->status |= GET_FIN;
  if (state->status & SEND_FIN_1) {
    state->status &= ~WAIT_ACK;
  }
  return;
}

void ctcp_receive(ctcp_state_t *state, ctcp_segment_t *segment, size_t len) {
  if (segment_is_corrupted(segment, len) == 0) {
    fprintf(stderr, "Receive:");
    print_segment(segment);
    /*Detect acknownedge segment*/ 
    if ((segment->flags & TH_ACK)) {
      handle_ack_segment(state, segment);
    }
    else if ((ntohs(segment->len) - sizeof(ctcp_segment_t)) > 0) {
      handle_data_segment(state, segment);
    }
    else if (segment->flags & TH_FIN) {
        handle_fin_segment(state, segment);
    }
    free(segment);
  }
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

void ctcp_output(ctcp_state_t *state) {
  char *data = calloc(state->payload_len,1);
  size_t w_availble_nmb = 0;

  if ((w_availble_nmb = conn_bufspace(state->conn)) == 0) {
    fprintf(stderr, "There are no room in STDOUT\n");
    return;
  }

  memcpy(data, state->data, state->payload_len);
  if (conn_output(state->conn, data, state->payload_len) < 0) {
    fprintf(stderr, "Cannot send to STDOUT\n");
  } 
  free(data);
  return;
}

/*Resending last packet when cannot recceive ack or time out*/
void ctcp_resend(ctcp_state_t *state) {
  fprintf(stderr, "Resending: ");
  int data_to_send_len = state->last_data_len;
  char *segment_to_send = calloc(1,sizeof(ctcp_segment_t) + data_to_send_len);

  /*Copy last segment to resend*/
  memcpy(segment_to_send, state->last_segment, sizeof(ctcp_segment_t));
  memcpy(segment_to_send + sizeof(ctcp_segment_t), state->last_segment_data, data_to_send_len);

  /*Resend last segment*/
  if (conn_send(state->conn, (ctcp_segment_t *) segment_to_send, 
      sizeof(ctcp_segment_t) + data_to_send_len) < 0) {
      fprintf(stderr, "Cannot send segment\n");
      return;
  }
  state->status |= WAIT_ACK;
  state->retransmit_time++;

  free(segment_to_send);
  return;
}

void handle_retransmittion(ctcp_state_t *state) {
  // fprintf(stderr, "Handle retransmition\n");
  // if ((state->retransmit_time < 5) & ((state->status == WAIT_ACK) | (state->status == SENT_FIN))) {
  if ((state->retransmit_time < 5) && ((state->status & (WAIT_ACK)))) {
    /*Resend and Update number of resend*/
    ctcp_resend(state);
  }
  else if ((state->retransmit_time >= 5) | (state->status == FINISH)) {
    /*Terminate transaction*/
    fprintf(stderr, "Call destroy state %d\n", state->number_in_list);
    ctcp_destroy(state);
  }      
  return;
}

int ctcp_send_fin (ctcp_state_t *state) {
  ctcp_segment_t *segment = calloc(sizeof(ctcp_segment_t), 1);
  make_segment(segment, 0, 0, 0, FIN, htons(MAX_SEG_DATA_SIZE), NULL);
  /*Send segment*/
  if (conn_send(state->conn, segment, sizeof(ctcp_segment_t)) < 0) {
    fprintf(stderr, "Cannot send segment\n");
    return -1;
  }
  memcpy(state->last_segment,segment,sizeof(ctcp_segment_t));
  state->last_data_len = 0;
  free(segment);
  return 0;
}

/*When getting an EOF signal, waitting for all segments are ack-ed*/
void handle_fin(ctcp_state_t *state) {
  // fprintf(stderr, "Handle Fin\n");
  /*Destroying state when status is FINISH*/
  if (state->status & FINISH) {
    fprintf(stderr, "Call destroy state %d\n", state->number_in_list);
    ctcp_destroy(state);
    return;
  }
  /*Get EoF, then send FIN*/
  else if ((state->status & GET_EOF) && 
           !(state->status & WAIT_ACK) && !(state->status & (SEND_FIN_1 | SEND_FIN_2 | GET_FIN))) {
    if (start_timer == 0) 
      start_timer = 1;
    else {
      fprintf(stderr, "Sent 1st FIN....\n");
      if (ctcp_send_fin(state) < 0)
        return;
      state->status |= (SEND_FIN_1 | WAIT_ACK);
      start_timer = 0;
    }
  }
  /*Get FIN, and SEND FIN back*/
  else if ((state->status & GET_FIN) && !(state->status & (SEND_FIN_1 | SEND_FIN_2))) {
    fprintf(stderr, "Send 2nd FIN....\n");
    if (ctcp_send_fin(state) < 0)
      return;
    state->status = (SEND_FIN_2 | WAIT_ACK);
    start_timer = 0;
  }
  /*Sent FIN and get FIN back*/
  else if (((state->status & GET_FIN) && (state->status & SEND_FIN_1)) || 
           ((state->status & SEND_FIN_2) && !(state->status & WAIT_ACK))) {
    if (start_timer == 0) 
        start_timer = 1;
    else {
      fprintf(stderr, "Finish, going to call destroying connection...\n");
      state->status = FINISH;
      start_timer = 0;
    }
  }
  return;
}

void ctcp_timer() {
  ctcp_state_t *state_walker = state_list;
  ctcp_state_t *state_next;
  if (timer_counter == 4) {
    while (state_walker) {
      state_next = state_walker->next;
      handle_retransmittion(state_walker);
      handle_fin(state_walker);
      state_walker = state_next;
    }
    timer_counter = 0;
  }
  else 
    timer_counter ++;
  return;
}
