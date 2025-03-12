/**
* @file physicalLayer.c
* @brief This file contains implementation for Layer1 of RASPNet Protocol.
*/

#include "physicalLayer.h"

#include "../CRC/CRC.h"
#include "../UART/UART.h"
#include "../Layer3/networkLayer.h"


uint8_t preamble = 0b01111110; /*!< This variable is used for storing static preamble value.*/
int identification = 0; /*!< This variable is used for storing identification that will use in implementation of Layer4 .*/
int flag = 0; /*!< This variable is used for storing flag that will use in implementation of Layer4 .*/

/**
 * This function is used for initializing uint_8* buffer from calculated uint32_t CRC of payload.
 * @param size Size of payload character Array/String.
 * @param payload Character Array/String for which CRC will be calculated
 */
uint8_t* initCRCBuffer(int size, uint8_t* payload){
	uint8_t* crcBuffer = (uint8_t*)malloc(4 * sizeof(uint8_t)); /*!< This variable is used for storing 32 bit CRC in uint8_t* type.*/
	
	uint32_t crcOfPayload = crc32((uint8_t*)payload, size); /*!< This variable is used for storing calculated 32 bit CRC of payload.*/
	
	*(crcBuffer) = (crcOfPayload);
	*(crcBuffer+1) = (crcOfPayload >> 8);
	*(crcBuffer+2) = (crcOfPayload >> 16);
	*(crcBuffer+3) = (crcOfPayload >> 24);
	
	return crcBuffer;
}

/**
 * This function is used for initializing message frame for transmitting.
 * @param payload Character Array/String to transmit
 */
uint8_t* initSendBufferFrame(uint8_t* payload){
	sendStringToUART("\n...\n###Initializing message frame\n");
	
	int sizeOfPayload = strlen(payload); /*!< This variable is used for storing length of payload message.*/
	uint8_t destAddress = getDestinationAddress(); /*!< This variable is used for storing address of destination node.*/
	uint8_t sourceAddress = getOwnAddress(); /*!< This variable is used for storing address of source (Own Address) node.*/
	
	uint8_t* bufferFrame = (uint8_t*)malloc((10 + sizeOfPayload) * sizeof(uint8_t)); /*!< This variable is used for storing message frame for transmitting.*/
	
	uint8_t* crcBufferPayload = initCRCBuffer(sizeOfPayload, payload); /*!< This function call is used to initializing uint_8* crcBufferPayload from payload to transmit.*/
	
	memcpy(bufferFrame, &preamble,1);
	memcpy(bufferFrame+1, crcBufferPayload,4);
	memcpy(bufferFrame+5, &sizeOfPayload,1);
	memcpy(bufferFrame+6, &destAddress,1);
	memcpy(bufferFrame+7, &sourceAddress,1);
	memcpy(bufferFrame+8, &identification,1);
	memcpy(bufferFrame+9, &flag,1);
	memcpy(bufferFrame+10, (uint8_t*)payload,sizeOfPayload);
	
	free(payload);
	free(crcBufferPayload);

	return bufferFrame;
}

