/**
* @file ISR.c
* @brief This file contains Interrupt Service Routine handling methods.
*/

#define delay 0.060
#define OCR_VALUE (delay*(F_CPU/1024))-1


#include "ISR.h"
#include "../Layer2/dataLinkLayer.h"


int phaseShifter = 0; /*!< This variable is used for implementing phase shifter.*/


/**
* This function initialized clock-signal and pin-change Interrupt.
*/
void initInterrupt(){
	OCR1A = OCR_VALUE; /*!< Calculated Time frequency with delay of 1 second.*/
	TCCR1B |= (1 << WGM12); /*!< CTC Mode on OCR1A.*/
	
	TIMSK1 |= (1 << OCIE1A); /*!< Interrupt on compare match.*/
	
	TCCR1B |= ((1 << CS12) | (1 << CS10)); /*!< prescaler of 1024.*/
	
	/***********************************************
     *  ... Pin-Change Interrupt.
     ***********************************************/
	PCICR |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT20);
}


/**
* Interrupt Service Routine for the clock-signal which toggle PORTB in every clock cycle.
*/
ISR(TIMER1_COMPA_vect){
	if(phaseShifter == 1){
		PORTB ^= (1 << PB4);
		phaseShifter = 0;
	}else{
		sendBuffer(); /*!< This function call is used to transmit message frame from network.*/
		phaseShifter = 1;
	}
}

/**
*Interrupt Service Routine for the pin-change interrupt which will receive data at every clock cycle.
*/
ISR(PCINT2_vect){
	receiveBuffer(); /*!< This function call is used to receive message frame from network.*/
}
