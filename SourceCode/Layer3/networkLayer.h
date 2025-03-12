#include <avr/io.h>

void setDestinationAddress(uint8_t address);

uint8_t getDestinationAddress();

void setOwnAddress(uint8_t address);

uint8_t getOwnAddress();

int checkReceivedFrame(uint8_t destination, uint8_t source);

int getPerformCrcValidation();
