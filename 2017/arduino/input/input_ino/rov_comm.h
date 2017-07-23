#define MSGLEN 10
#define MOTOR_COUNT 3
#define STOP_BYTE 001001100
#define RECV_TIMEOUT_MS 10000000

typedef struct 
    {
    int16_t motor[MOTOR_COUNT];
    uint16_t flags;
    uint8_t check_sum;
    uint8_t stop_byte;
    }
msg;


int checkSum(char raw_msg[]); //checksum check
