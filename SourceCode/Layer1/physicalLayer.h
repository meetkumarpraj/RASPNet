#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>

uint8_t* initCRCBuffer(int size, uint8_t* payload);

uint8_t* initSendBufferFrame(uint8_t* payload);
