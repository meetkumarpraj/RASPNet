/**
* @file CRC.c
* @brief This file contains implementation for calculating 32 bit CRC.
*/

#include "CRC.h"

uint32_t poly_32 = 0b00000100110000010001110110110111; /*!< This variable is used for storing polynomial value to calculate 32 bit CRC of payload.*/

/**
 * This function is used for calculating 32 bit CRC of payload.
 * @param payload Character Array/String for which CRC will be calculated
 * @param payloadSize Size of payload character Array/String.
 */
uint32_t crc32(uint8_t* payload, int payloadSize){
    int byteAt = 0; /*!< This variable is used for indexing byte in payload*/
    uint32_t crc_reg = 0; /*!< This variable is used to store calculated CRC value*/

    while (payloadSize != 0) {
      	uint8_t byte = payload[byteAt];
      	crc_reg ^= ((uint32_t)byte<<24);

		// bitwise iteration
      	for (uint8_t bit = 0; bit < 8 ; bit++){
			if((crc_reg & 0x80000000) != 0) {
				crc_reg = (uint32_t)((crc_reg<<1)^poly_32); 
			}else{
				crc_reg<<=1;
			}
      	}
      	byteAt = byteAt + 1;
		payloadSize--;
  	}

   return crc_reg;
}
