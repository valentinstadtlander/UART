/*
 * CRC.c
 *
 *  Created on: 26.03.2020
 *      Author: tobi, valentin
 */

#include <stdio.h>
#include <stdlib.h>
#include "CRC.h"

/*
 * CRC calculation function for a message more than one byte
 *
 * bytes[]: message with bytes for crc calculation
 * length:  length of the byte array
 *
 * returns the calculated 8-bit crc value for the whole message
 */
uint8_t calc_crc8_for_one_byte(uint8_t start_crc, uint8_t byte)
{
	uint8_t crc = start_crc ^ byte;
	for(int i = 0; i < 8; i++)
	{
		if((crc & 0x80) != 0)
		{
			crc = (crc<<1) ^ CRC_GENERATOR;
		} else
		{
			crc <<= 1;
		}
	}

	return crc;
}


/*
 * CRC calculation function for one byte
 *
 * start_crc: start value for crc calculation
 * byte: 	  byte for crc calculation
 *
 * returns the calculated 8-bit crc value for the given byte
 */
uint8_t calc_crc8(uint8_t bytes[], int length)
{
	uint8_t crc_val = 0x00;
	for(int i = 0; i <length; i++)
	{
		crc_val = calc_crc8_for_one_byte(crc_val, bytes[i]);
	}

	return crc_val;
}