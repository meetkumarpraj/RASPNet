/**
* @file dataLinkLayer.c
* @brief This file contains implementation for Layer2 of RASPNet Protocol.
*/

#include "dataLinkLayer.h"

#include "../CRC/CRC.h"
#include "../UART/UART.h"
#include "../Layer3/networkLayer.h"
#include "../Layer1/physicalLayer.h"

int isReceivedFrameToForward = 0; /*!< This variable is used for identifying that received frame need to forward back to ring or not.*/

int sendBitCounter = 0; /*!< This variable is used to keep track of number of bit in byte which was currently in transmissions.*/
int sendByteCounter = 0; /*!< This variable is used to keep track of number of byte which was transmitted.*/
static int isSending = 0; /*!< This variable is used to enable transmission of message frame.*/

int totalBytesToSend = 0; /*!< This variable is used to indicate size of message frame in byte to transmit.*/
int totalBytesToReceived = 0; /*!< This variable is used to indicate size of message frame in byte to receive.*/

int receiveBitCounter = 0; /*!< This variable is used to keep track of number of bit in byte which is currently in receiving.*/
int receiveByteCounter = 0; /*!< This variable is used to keep track of number of byte which is receiving.*/

uint8_t receivePreamble = 0; /*!< This variable is used to store received bytes to compare with static preamble.*/
int isPreambleReceived = 0; /*!< This variable is used to store validation of received bytes with static preamble.*/

uint8_t* sendBufferFrame; /*!< This variable is used to store message frame to transmit in ring.*/
uint8_t* receivedBufferFrame; /*!< This variable is used to store message frame which will be received from ring.*/

/**
 * This function is used for resetting all variables of Layer 1, 2 and 3.
 */
void resetAll(){
	isReceivedFrameToForward = 0;
	sendBitCounter = 0;
	sendByteCounter = 0;
	isSending = 0;
	totalBytesToSend = 0;
	totalBytesToReceived = 0;
	receiveBitCounter = 0;
	receiveByteCounter = 0;
	receivePreamble = 0;
	isPreambleReceived = 0;
	free(sendBufferFrame);
	free(receivedBufferFrame);
}

/**
 * This function will start the transmission of message frame.
 * @param payload Character Array/String to transmit.
 */
void initBuffers(uint8_t* payload){
	
	totalBytesToSend = 10 + strlen(payload);
	sendBufferFrame = (uint8_t*)malloc(totalBytesToSend * sizeof(uint8_t));
	sendBufferFrame = initSendBufferFrame(payload); /*!< This function call is used for initializing message frame for transmitting.*/

	sendStringToUART("\n[=>=>]Sending message\n");
	isSending = 1;
}

/**
 * This function will validate that preamble value is received or not.
 */
void checkPreambleValue(){
	if(PIND & (1<<PD5)){
		receivePreamble |= 1;

	}else{
		receivePreamble &= ~1;
		
	}
	
	if(receivePreamble == 0b01111110){
		sendStringToUART("\n[<=<=]Preamble Received\n");
		
		receiveBitCounter = 0;
		receiveByteCounter=1;
		
		totalBytesToReceived = 10;
		receivedBufferFrame = (uint8_t*)malloc(totalBytesToReceived * sizeof(uint8_t));
		*(receivedBufferFrame + 0) = receivePreamble;
		isPreambleReceived = 1;
		receivePreamble = 0;
	}else{
		receivePreamble <<= 1;
	}
}

/**
 * This function will validate CRC which is received in message frame.
 */
void validateReceivedCRC(){
	sendStringToUART("\n###Validating CRC###\n");
	for(int i = 1;i<=4;i++){
		char str[30];
		sprintf(str,"\nCRC:%lx\n",*(receivedBufferFrame+i));
		sendStringToUART(str);
	}
	
	int sizeOfReceivedPayload = *(receivedBufferFrame + 5); /*!< This variable is used for storing length of payload from received message.*/
	int byteAt = 10;
	
	uint8_t *receivedPayloadBuffer = (uint8_t*)malloc(sizeOfReceivedPayload * sizeof(uint8_t)); /*!< This variable is will store payload from received message.*/
	for(int i = 0;i<sizeOfReceivedPayload;i++){
		*(receivedPayloadBuffer+i) = *(receivedBufferFrame + byteAt);
		byteAt++;
	}
	
	uint8_t* crcBufferPayload = initCRCBuffer(sizeOfReceivedPayload, receivedPayloadBuffer); /*!< This function call is used to initializing uint_8* crcBufferPayload from received payload.*/
	
	if((*(crcBufferPayload) == *(receivedBufferFrame + 1)) && 
		(*(crcBufferPayload+1) == *(receivedBufferFrame + 2)) &&
		(*(crcBufferPayload+2) == *(receivedBufferFrame + 3)) && 
		(*(crcBufferPayload+3) == *(receivedBufferFrame + 4))
		){
			sendStringToUART("\n###Received CRC Matched###\n");
	}else{

		sendStringToUART("\nReceived CRC Not Matched\n");
	}
}

/**
 * This function will forward message frame in ring.
 */
void forwardFrame(){
	totalBytesToSend = 10 + *(receivedBufferFrame + 5);
	sendBufferFrame = (uint8_t*)malloc(totalBytesToSend * sizeof(uint8_t));
	
	for(int i=0;i<totalBytesToSend;i++){
		memcpy(sendBufferFrame+i, receivedBufferFrame+i,1);
	}
	
	char str[40];
	sprintf(str,"\n[=>=>]forwarding Data of Size:%d\n",*(sendBufferFrame+5));
	sendStringToUART(str);	
	
	isSending = 1;	
	isReceivedFrameToForward = 0;
}

/**
 * This function will transmit message frame in ring.
 */
void sendBuffer(){
	
	if(isSending == 1){
		
		if(sendByteCounter < totalBytesToSend){
		
			if(sendBitCounter < 8){
				uint8_t bit = ((*(sendBufferFrame+sendByteCounter)) >> (7-sendBitCounter)) & 1; /*!< This variable is used to point bit from current byte which is currently in transmission.*/
				
				if(bit == 1){
					PORTB |= (1 << PB5);

				}else{
					PORTB &= ~(1 << PB5);

				}
				if(sendBitCounter == 7){
					if(sendByteCounter == 1){
						sendStringToUART("\n[=>=>]Sending CRC\n");
					}
					if(sendByteCounter == 5){
						sendStringToUART("\n[=>=>] Sending Size of payload\n");
					}
					if(sendByteCounter == 6){
						char str[45];
						sprintf(str,"\n[=>=>]Sending Destination Address: %02x.\n",*(sendBufferFrame + sendByteCounter));
						sendStringToUART(str);
						
					}
					if(sendByteCounter == 7){
						char str[45];
						sprintf(str,"\n[=>=>]Sending Source Address: %02x.\n",*(sendBufferFrame + sendByteCounter));
						sendStringToUART(str);
												
					}
					if(sendByteCounter == 8){
						char str[45];
						sprintf(str,"\n[=>=>]Sending Identification: %d.\n",*(sendBufferFrame + sendByteCounter));
						sendStringToUART(str);
					}
					if(sendByteCounter == 9){
						char str[45];
						sprintf(str,"\n[=>=>]Sending Flag: %d.\n[=>=>]Sending payload: ",*(sendBufferFrame + sendByteCounter));
						sendStringToUART(str);
					}
					
					if(sendByteCounter > 9){
						char str[10];
						sprintf(str,"[=>=>]%c",*(sendBufferFrame + sendByteCounter));
						sendStringToUART(str);
					}
					
					sendBitCounter = 0;
					sendByteCounter++;
					
				}else{
					sendBitCounter++;
				}
			
			}
		}
		else if(sendByteCounter == totalBytesToSend){
			sendBitCounter = 0;
			sendByteCounter = 0;
			totalBytesToSend = 0;
			isSending = 0;
			free(sendBufferFrame);
			sendStringToUART("\n###Message Sending completed###\n");
		}
	}
	else{
		PORTB &= ~(1 << PB5);
	}	
}

/**
 * This function will receive message frame from ring.
 */
void receiveBuffer(){

	if(isPreambleReceived == 0){
		checkPreambleValue(); /*!< This function call is used to check if preamble value is received or not.*/
	}else if(isPreambleReceived == 1){
		if(receiveByteCounter < totalBytesToReceived){
				if(PIND & (1<<PD5)){
					*(receivedBufferFrame + receiveByteCounter) |= 1;	
				}else{
					*(receivedBufferFrame + receiveByteCounter) &= ~1;

				}
			
				if(receiveBitCounter == 7){	

					if(receiveByteCounter == 1){
						sendStringToUART("\n[<=<=]Receiving CRC\n");
					}
					if(receiveByteCounter == 4){
						sendStringToUART("\n[<=<=]Receiving payload size\n");
					}
					if(receiveByteCounter == 5){
						char str[25];
						sprintf(str,"\n[<=<=] Size of payload: %d\n",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
						
						totalBytesToReceived += *(receivedBufferFrame + receiveByteCounter);
						receivedBufferFrame = (uint8_t*)realloc(receivedBufferFrame, totalBytesToReceived * sizeof(uint8_t));
					}
					if(receiveByteCounter == 6){
						char str[45];
						sprintf(str,"\n[<=<=]Destination Address: %02x.\n",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
						
					}
					if(receiveByteCounter == 7){
						char str[45];
						sprintf(str,"\n[<=<=]Source Address: %02x.\n",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
						
						isReceivedFrameToForward = checkReceivedFrame(*(receivedBufferFrame + 6), *(receivedBufferFrame + 7)); /*!< This function call is used to check type of received message and to identify it will forward in ring or not.*/
						
					}
					if(receiveByteCounter == 8){
						char str[45];
						sprintf(str,"\n[<=<=]Identification: %d.\n",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
					}
					if(receiveByteCounter == 9){
						char str[45];
						sprintf(str,"\n[<=<=]Flag: %d.\n[<=<=]Receiving payload: ",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
					}
					
					if(receiveByteCounter > 9){
						char str[10];
						sprintf(str,"[<=<=]%c",*(receivedBufferFrame + receiveByteCounter));
						sendStringToUART(str);
					}
					receiveBitCounter = 0;
					receiveByteCounter++;
				}else{
					*(receivedBufferFrame + receiveByteCounter) <<= 1;
					receiveBitCounter++;
				}
			
			}
		else if(receiveByteCounter == totalBytesToReceived){
				sendStringToUART("\n###Message received###\n");
				if(getPerformCrcValidation() == 1){
					validateReceivedCRC(); /*!< This function call will validate CRC which is received in message frame.*/
				}
				
				isPreambleReceived = 0;
				receiveBitCounter = 0;
				receiveByteCounter = 0;
				
				if(isReceivedFrameToForward == 1 ){
					forwardFrame(); /*!< This function call will forward message frame in ring.*/
				}
				totalBytesToReceived = 0;
				free(receivedBufferFrame);
			}
	}
}
