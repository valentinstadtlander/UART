/*
 * UART_EIVE_Protocol_Recv.c

 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"

/** Identification numbers for commands **/
#define CAMERA_SS	0b00000000
#define UART_SS		0b00001010
#define BRAM_SS		0b00000101
#define CPU_SS		0b00001111
#define DOWNLINK_SS	0b00001001
#define DAC_SS		0b00000110
#define DATA_SS		0b11110000

uint8_t databuffer[357143];


int recv_cmd(uint8_t *id, u8 *databytes);
int recv_data(uint8_t *header);

/**
 *
 */
int UART_Recv_Data()
{
	uint8_t header[4];
	uint8_t data[28];
	UART_Recv_Buffer();

	extract_header(RecvBuffer, header, data);
	u8 temp[31] = {header[ID_POS], header[DATA_SIZE_POS], header[FLAGS_POS], data}; //check

	if(check_crc(header[CRC_POS], temp) != XST_SUCCESS)
	{
		set_ACK_Flag(header[FLAGS_POS], 0);
		UART_ACK();
		return XST_FAILURE;
	}
	else
	{
		set_ACK_Flag(header[FLAGS_POS], 1);
	}

	if(check_TC_flag(&header[FLAGS_POS]) == 1)
	{
		recv_cmd(header, data);
	}
	else
	{
		recv_data(header);
	}
}

/**
 *
 */
int extract_header(uint8_t *rcvBuffer, uint8_t *header, uint8_t *data)
{

}

/**
 *
 */
int check_ACK_flag(uint8_t *flags)
{
	if((*flags & ACK_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_TC_flag(uint8_t *flags)
{
	if((*flags & TC_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_Req_to_send_flag(uint8_t *flags)
{
	if((*flags & REQ_TO_SEND_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_ready_to_recv_flag(uint8_t *flags)
{
	if((*flags & READY_TO_RECV_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_start_flag(uint8_t *flags)
{
	if((*flags & START_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_end_flag(uint8_t *flags)
{
	if((*flags & END_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int check_end_conn_flag(uint8_t *flags)
{
	if((*flags & END_CONN_MASK) != 0)
		return 1;
	else return 0;
}

/**
 *
 */
int recv_cmd(uint8_t *header, u8 *databytes)
{
	uint8_t id = header[ID_POS];

	switch(id)
	{
		case CAMERA_SS: break;
		case UART_SS: break;
		case CPU_SS: break;
		case BRAM_SS: break;
		case DOWNLINK_SS: break;
		case DAC_SS: break;
		case DATA_SS: break;
		default: return XST_FAILURE;
	}

	UART_ACK();

	return XST_SUCCESS;
}

/**
 *
 */
int recv_data(uint8_t *header)
{
	uint8_t flags = header[FLAGS_POS];

	if(check_Req_to_send_flag(&flags) == 1)
	{
		set_rdy_2_recv(&flags, 1); //noch zu deklarieren

		// -> Send answer and wait for data
		UART_ACK();


		uint8_t new_header[4];
		uint8_t new_data[PACKAGE_DATA_SIZE];

		int datacounter = 0;
		uint8_t end = 0;

		// receiving data
		while(end != 1)
		{
			//Receiving answer
			UART_Recv_Buffer();

			//extracting the header of the package
			extract_header(RecvBuffer, new_header, new_data);

			//temp array for crc check
			u8 temp[31] = {new_header[ID_POS], new_header[DATA_SIZE_POS], new_header[FLAGS_POS], new_data}; //check

			//CRC check
			if(check_crc(new_header[CRC_POS], temp) != XST_SUCCESS)
			{
				set_ACK_Flag(new_header[FLAGS_POS], 0);
				UART_ACK();
				continue;
			}
			else
			{
				set_ACK_Flag(new_header[FLAGS_POS], 1);
			}


			//check if this package was the last data package
			if(check_end_flag(&new_header[FLAGS_POS]) == 1)
				end = 1;


			//data buffer
			for(int bytes = 0; bytes < new_header[DATA_SIZE_POS]; bytes++)
				databuffer[datacounter * PACKAGE_DATA_SIZE + bytes] = new_data[bytes];

			datacounter += new_header[DATA_SIZE_POS];
		}

		set_end_conn_flag(&flags, 1);

		// -> send end connection

	}
	else
	{
		set_ACK_Flag(&flags, 0);
		UART_ACK();

		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


