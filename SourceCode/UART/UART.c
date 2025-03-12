/**
* @file UART.c
* @brief This file contains UART handling methods.
*/

#define BAUD 9600
#define BAUD_PRESCALLER (((F_CPU/(BAUD*16UL)))-1)

#include <stdlib.h>
#include <avr/io.h>
#include <util/setbaud.h>


/**
 * This function is used for UART initialization.
 */
void initUART (void){
	UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /*!< 8-bit data*/
    UCSR0B = _BV(RXEN0) | _BV(TXEN0); /*!< Enable RX and TX*/
}

/**
 * This function is used for transmitting character to the MINICOM console.
 * @param data Character to transmit
 */
void sendCharToUART(char data){
	while(!(UCSR0A & (1 << UDRE0)));
	
	UDR0 = data;
}

/**
 * This function is used for transmitting character array/string to the MINICOM console.
 * @param *str Character Array/String to transmit
 */
void sendStringToUART(char *str){
	while(*str){
		sendCharToUART(*str);
		str++;
	}
}

/**
 * This function is used for receiving character from the MINICOM console.
 */
char getCharFromUART(){
	char character; /*!< This variable is used for storing character input by user from MINICOM console.*/
	while((UCSR0A &(1 << RXC0)) ==0 ){};
	character = UDR0;
	return character;
}

/**
 * This function is used for receiving character array/string from the MINICOM console.
 */
char* getStringFromUART(){
	char* inputString = (char*)malloc(50* sizeof(char)); /*!< This variable is used for storing character array/String input by user from MINICOM console.*/
	int inputCharCounter = 0; /*!< This variable is used for counting character array/String input by user from MINICOM console.*/
	char inputChar = getCharFromUART();
	
	while(inputChar != 13){
		*(inputString + inputCharCounter) = inputChar;
		inputChar = getCharFromUART();

		inputCharCounter++;
	}
	*(inputString + inputCharCounter) = '\0';
	return inputString;
}
