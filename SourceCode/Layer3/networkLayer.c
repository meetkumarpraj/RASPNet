/**
* @file networkLayer.c
* @brief This file contains implementation for Layer3 of RASPNet Protocol.
*/

#include "networkLayer.h"
#include "../UART/UART.h"

int performCrcValidation = 0; /*!< This variable will used to identify that CRC validation will perform on received payload or not.*/
uint8_t ownAddress = 0x0D; /*!< This variable will store current device address.*/
uint8_t destinationAddress = 0x0A; /*!< This variable will store destination device address.*/

/**
 * This function will set destination node address.
 */
void setDestinationAddress(uint8_t address){
	destinationAddress = address;
}

/**
 * This function will return saved destination node address.
 */
uint8_t getDestinationAddress(){
	return destinationAddress;
}

/**
 * This function will set current node address.
 */
void setOwnAddress(uint8_t address){
	ownAddress = address;
}

/**
 * This function will return saved current node address.
 */
uint8_t getOwnAddress(){
    return ownAddress;
}

/**
 * This function will validate received frame weather its an broadcast message or its an self message or just need to forward in ring.
 * @param destination address of receiving node.
 * @param source address of transmitting node.
 */
int checkReceivedFrame(uint8_t destination, uint8_t source){
	if(destination == ownAddress){
		sendStringToUART("\nSelf Message\n");
		
		performCrcValidation = 1;
		return 0;
	}else if(destination != 0x00 && destination != ownAddress && source != ownAddress){
		sendStringToUART("\nForward\n");
		
		performCrcValidation = 0;
		return 1;
	}else if(destination != 0x00 && destination != ownAddress && source == ownAddress){
		sendStringToUART("\nOwn message destination address not found\n");
		
		performCrcValidation = 1;
		return 0;
	}else if(destination == 0x00 && source == ownAddress){
		sendStringToUART("\nOwn Broadcast message.\n");
		
		performCrcValidation = 1;
		return 0;
	}else if(destination == 0x00 && source != ownAddress){
		sendStringToUART("\nForwarding broadcast message in ring\n");
		
		performCrcValidation = 1;
		return 1;
	}
}

/**
 * This function will return saved result of validation.
 */
int getPerformCrcValidation(){
	return performCrcValidation;
}
