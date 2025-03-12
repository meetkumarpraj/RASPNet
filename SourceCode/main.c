/**
* @file main.c
* @brief This file contains main() from which program execution will start.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "UART/UART.h"
#include "IRQ/ISR.h"
#include "Layer2/dataLinkLayer.h"
#include "Layer3/networkLayer.h"


char* payload; /*!< This variable contains payload which was received from the MINICOM console.*/
uint8_t address; /*!< This variable contains source or destination address which was received from the MINICOM console.*/

/**
 * This function is used for receiving inputs data from user using MINICOM console:
 * - B to set user input as destination address.
 * - D to set user input as payload.
 * - P to send message frame.
 * - R to reset all variables.
 */
void getInputsFromUser(){
	char *string = getStringFromUART(); /*!< This variable is used to store user input data as string.*/
	char str[50]; /*!< This variable is used for showing debug messages on MINiCOM console.*/
	
	if(*string == 'B'){
		sscanf(string,"B %d",&address);
		sprintf(str,"\nDestination Address: %d\n",address);
		sendStringToUART(str);

		setDestinationAddress(address); /*!< This function call is used for setting user input data as destination address of message frame.*/
	}
	else if(*string == 'D' && *(string + 1) == ' '){
		int dataCounter = 0; /*!< This variable is used for storing actual size of payload data.*/
		int size = strlen(string); /*!< This variable is used for storing size of user input data.*/

		payload = (char*)malloc(size * sizeof(char));
		for(int i=2;i<size;i++){
			*(payload + dataCounter) = *(string+i);
			dataCounter++;
		}
		*(payload + dataCounter) = '\0';
	}
	else if(*string == 'P'){
		initBuffers(payload); /*!< This function call is used to start the transmission of message frame.*/

	}else if(*string == 'R'){
		resetAll(); /*!< This function call is used to reset all variables of Layer 1, 2 and 3.*/
	}else{
		sendStringToUART("\nEnter Valid Command!!");
	}
	free(string);
}

/**
 * This is a main function of this project.
 */
int main(void)
{
	DDRB |= ((1 << PB4) | (1 << PB5)); /*!< Setting DDRB for transmitting clock signal.*/
	DDRD &= (~(1 << PD4) | ~(1 << PD5)); /*!< Setting DDRD for receiving clock signal.*/
	

	initUART(); /*!< This function call is used for initialize UART.*/
	initInterrupt(); /*!< This function call is used for initialize interrupts.*/
	
	sei(); /*!< This function call is used for enabling interrupts.*/
	_delay_ms(2000);
	sendStringToUART("\n[=>=>]FRESH START[<=<=]\n");
	
	
	while(1){
		getInputsFromUser(); /*!< This function call is used for receiving inputs data from user using MINICOM console.*/
	}
	return 0;
}
