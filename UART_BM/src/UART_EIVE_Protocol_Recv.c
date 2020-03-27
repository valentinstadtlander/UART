/*
 * UART_EIVE_Protocol_Recv.c

 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"

int UART_Recv_Data()
{
	uint8_t header[4];
	uint8_t data[28];
	UART_Recv_Buffer();

	extract_header(RecvBuffer, header, data);
	uint8_t temp[31] = {header[ID_POS], header[DATA_SIZE_POS], header[FLAGS_POS], data};

	if(check_crc(header[CRC_POS], temp) == XST_SUCCESS)
	{

	}

	if(check_TC_flag(header[FLAGS_POS]) == 1)
	{

	}
}

int extract_header(uint8_t *rcvBuffer, uint8_t *header, uint8_t *data)
{

}

int check_ACK_flag(uint8_t *flags)
{
	if((*flags & ACK_MASK) != 0)
		return 1;
	else return 0;
}

int check_TC_flag(uint8_t *flags)
{
	if((*flags & TC_MASK) != 0)
		return 1;
	else return 0;
}

int check_Req_to_send_flag(uint8_t *flags)
{
	if((*flags & REQ_TO_SEND_MASK) != 0)
		return 1;
	else return 0;
}

int check_ready_to_recv_flag(uint8_t *flags)
{
	if((*flags & READY_TO_RECV_MASK) != 0)
		return 1;
	else return 0;
}

int check_start_flag(uint8_t *flags)
{
	if((*flags & START_MASK) != 0)
		return 1;
	else return 0;
}

int check_end_flag(uint8_t *flags)
{
	if((*flags & END_MASK) != 0)
		return 1;
	else return 0;
}

int check_end_conn_flag(uint8_t *flags)
{
	if((*flags & END_CONN_MASK) != 0)
		return 1;
	else return 0;
}


