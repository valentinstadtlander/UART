/*
 * CRC.c
 *
 *  Created on: 26.03.2020
 *      Author: Tobias & Valentin
 */

#include <stdio.h>
#include <stdlib.h>
#include "CRC.h"
#include "UART_io.h"
#include "xparameters.h"
#include "UART_EIVE_Protocol.h"

/*
 * CRC calculation function for one byte
 *
 * start_crc: start value for crc calculation
 * byte: 	  byte for crc calculation
 *
 * returns the calculated 8-bit crc value for the given byte
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
 * CRC calculation function for a message more than one byte
 *
 * bytes[]: message with bytes for crc calculation
 * length:  length of the byte array
 * crc_initval: the start value for calculating crc of the data
 *
 * returns the calculated 8-bit crc value for the whole message
 */
uint8_t calc_crc8(uint8_t bytes[], int length, uint8_t crc_initval)
{
	uint8_t crc_val = crc_initval;
	for(int i = 0; i < length; i++)
	{
		crc_val = calc_crc8_for_one_byte(crc_val, bytes[i]);
	}

	return crc_val;
}

/*
 *
 */
int check_crc(uint8_t crc_val, uint8_t *header, uint8_t *data, uint8_t crc_initval)
{
	uint8_t calculated_crc;

	//temp array for crc check
	uint8_t temp[31] = {header[ID_POS], header[DATA_SIZE_POS], header[FLAGS_POS], *data}; //check


	calculated_crc = calc_crc8(temp, BUFFER_SIZE - 1, crc_initval);

	if(calculated_crc != crc_val)
		return XST_FAILURE;

	return XST_SUCCESS;
}
