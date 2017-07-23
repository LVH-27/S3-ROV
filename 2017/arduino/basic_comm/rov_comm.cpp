#include "rov_comm.h"

int fillMsg(char* raw_msg, msg &message)
    {
    int i;
    message.motor[0] = (int16_t) 0;
    message.motor[1] = (int16_t) 0;
    message.motor[2] = (int16_t) 0;
    message.flags = (uint16_t) 0;
    message.check_sum = (uint8_t) 0;
    message.stop_byte = (uint8_t) 0;
        
    for(i = 0; i < MOTOR_COUNT; i++)
        {
        message.motor[i] |= (raw_msg[2*i] & 0b11111111);
        message.motor[i] = message.motor[i] << 8;
        message.motor[i] |= (raw_msg[2*i + 1] & 0b11111111);
        }
        
    message.flags |= (raw_msg[2*i] & 0b11111111);
    message.flags = message.flags << 8; 
    message.flags |= (raw_msg[2*i + 1] & 0b11111111);
    i++;
        
    message.check_sum |= raw_msg[2*i];
    message.stop_byte |= raw_msg[2*i + 1];    
    }

uint8_t calcSum(char* raw_msg)
    {
    int i, j;
    uint8_t sum = 0;
    for(i = 0; i < 8; i++)
        {
        //Serial.println((uint8_t)raw_msg[i]);
        for(j = 0; j < 8; j++)
            {
            sum += (01 & (raw_msg[i] >> j));
            }
        }
    return sum;
    }

int sumOk(uint8_t recvSum, uint8_t calcsum)
    {
    if(recvSum == calcsum)
        {
        return 1;
        }
    return -1;
    }

int stopOk(uint8_t recvStop, uint8_t trueStop = STOP_BYTE)
    {
    if(recvStop == trueStop)
        {
        return 1;
        }
    return -1;
    }

void waitForStop(char byteWait = STOP_BYTE)
    {
    char recvByte[1];
    recvByte[0] = (char) 0;
    while(recvByte[0] != STOP_BYTE)
        {
        Serial.readBytes(recvByte, 1);
        }
    return;
    }

float map_float(int value, int min_, int max_, float min_o, float max_o){
  return (float) (value - min_) * (max_o - min_o) / (max_ - min_) + min_o;
}