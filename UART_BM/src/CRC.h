/*
 * CRC.h
 *
 *  Created on: 26.03.2020
 *      Author: tobi, valentin
 */

#ifndef CRC_H_
#define CRC_H_

/*
 * Generator polynom for cyclic redundancy check
 *
 *
 */
#define CRC_GENERATOR 0x1D;

/*
 * CRC calculation function for a message more than one byte
 *
 * bytes[]: message with bytes for crc calculation
 * length:  length of the byte array
 *
 * returns the calculated 8-bit crc value for the whole message
 */
uint8_t calc_crc8(uint8_t bytes[], int length);

/*
 * CRC calculation function for one byte
 *
 * start_crc: start value for crc calculation
 * byte: 	  byte for crc calculation
 *
 * returns the calculated 8-bit crc value for the given byte
 */
uint8_t calc_crc8_for_one_byte(uint8_t start_crc, uint8_t byte);

#endif /* CRC_H_ */
