#include <Arduino.h>
#define MSGLEN 10
#define MOTOR_COUNT 3
#define STOP_BYTE 0x4C // binarno: 0100 1100 --jlkhlj
#define RECV_TIMEOUT_MS 10000000

typedef struct 
    {
    int16_t motor[MOTOR_COUNT];
    uint16_t flags;
    uint8_t check_sum;
    uint8_t stop_byte;
    }
msg;


uint8_t calcSum(char* raw_msg);                // calculate checksum. to fill with raw data. takes only first 8 messages
int fillMsg(char* raw_msg, msg &message);      // fill structure message with data, received into buffer "raw_msg", of length MSGLEN
int sumOk(uint8_t recvSum, uint8_t calcSum);   // return -1 if sums don't match
int stopOk(uint8_t recvStop, uint8_t trueStop/* = STOP_BYTE*/);
void waitForStop(char byteWait);               // if some error occured (length not matching, sum not matching, stop byte incorrect...)
                                               //drop messages until stopbyte is received, could be more efficiently implemented...
float map_float(int value, int min_, int max_, float min_o, float max_o); // loki's float-map implementation
