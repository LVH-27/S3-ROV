#include "rov_comm.h"

void setup()
    {
    Serial.begin(115200);
    Serial.setTimeout(RECV_TIMEOUT_MS);
    }

char raw_msg[MSGLEN];

void loop()
    {
    int len = Serial.readBytes(raw_msg, MSGLEN);
    msg command;
    int errorFlag = 0;
    
    if(len == MSGLEN)
        {
        fillMsg(raw_msg, command);
        uint8_t sum = calcSum(raw_msg);
        
        
        // TODO: implement error check in it's own routine -lkbljkbjhb
        if(sumOk(sum, command.check_sum) == -1)
            {
            Serial.println("[err] incorrect checksum!");
            errorFlag = 1;
            }
        if(!stopOk(command.stop_byte, STOP_BYTE) == -1)
            {
            Serial.println("[err] incorrect stop byte!");
            errorFlag = 1;
            }
        }
    else
        {
        Serial.println("[err] wrong msg length (serial timeout probably occured)");
        errorFlag = 1;
        }
    
    
    if(!errorFlag)
        {
        Serial.println("######################");
        Serial.println(command.motor[0]);
        Serial.println(command.motor[1]);
        Serial.println(command.motor[2]);
        Serial.println(command.flags);
        Serial.println(command.check_sum);
        Serial.println(command.stop_byte);
        //thing to do if all checks passed;
        }
    else
        {
        //else
        waitForStop(STOP_BYTE);
        }
    }
