#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>

void resetAll();

void initBuffers(uint8_t* payload);

void checkPreambleValue();

void validateReceivedCRC();

void forwardFrame();

void sendBuffer();

void receiveBuffer();
