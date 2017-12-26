typedef enum msmType {REQUEST = 100, ACK} msmType_t;
#define TEST_SIZE 1024

struct message {
	msmType_t type;
	unsigned length;
	char buffer[TEST_SIZE];
};
typedef struct message msg_t;