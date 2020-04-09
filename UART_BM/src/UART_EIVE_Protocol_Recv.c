/*
 * UART_EIVE_Protocol_Recv.c
 *
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"

/*
 * Receive data if its available.
 *
 *
 * @return:
 *
 */
int UART_Recv_Data()
{
	/* variable for the status of the receive buffer: SUCCESS, FAILED, NO_DATA_AVAILABLE */
	int status;

	//Check the receive buffer if data is available
	status = UART_Recv_Buffer();

	//if data available continue, else return
	if(status != XST_SUCCESS)
	{
		if(status == XST_NO_DATA)
			return XST_NO_DATA;
		else
			return XST_FAILURE;
	}

	receive();

	return XST_SUCCESS;
}

/*
 * In this method the protocol for receiving data is implemented, which requires a connection
 * to the sender. A connection is established to receive packets longer than 28 bytes.
 * First, it is indicated that some data is to be transmitted, to which it responds that data
 * can be received. Then the actual data transmission takes place, where each received packet
 * is acknowledged positively or negatively.
 *
 * @return
 */
int receive()
{
	int status;

	/* variable for the new flags which will be set */
	uint8_t new_flags = 0x00;

	/* variable to store the last sent CRC value for calculating next CRC */
	uint8_t last_crc_send = 0x00;

	uint8_t conn_id = 0x00;

	/* variable to store the last sent CRC value for calculating next CRC */
	uint8_t last_crc_rcv = 0x00;

	status = connection_establishment(&last_crc_rcv, &new_flags, &conn_id);

	if(status == XST_FAILURE)
		return XST_FAILURE;

	//receive the tm/tc
	status = receive_data(&last_crc_rcv, &last_crc_send, &conn_id, &new_flags);

	if(status == XST_FAILURE)
		return XST_FAILURE;
}

/*
 *
 */
int connection_establishment(uint8_t *last_crc_send, uint8_t *new_flags, uint8_t *conn_id)
{
	/* variable for header of the first received package */
	uint8_t header[HEADER_SIZE];

	/* variable for the received data from the package */
	uint8_t data[PACKAGE_DATA_SIZE];

	//extract header from buffer
	extract_header(RecvBuffer, header, data);

	//
	*conn_id = header[ID_POS];

	/* crc_check */
	if(check_crc(header[CRC_POS], header, data, INIT_CRC) != XST_SUCCESS)
	{
		//Send answer without set ACK flag
		send_failure(last_crc_send, &header[ID_POS]);

		return XST_FAILURE;
	}

	//set acknowledge flag
	set_ACK_Flag(new_flags, ACK);

	/* check request 2 send */
	if(get_Req_to_send_flag(&header[FLAGS_POS]) == 0)
	{
		//Send answer without set ACK flag
		send_failure(last_crc_send, &header[ID_POS]);

		return XST_FAILURE;
	}

	//set the ready_to_receive flag
	set_Rdy_to_rcv_Flag(new_flags, 1);

	int status;

	status = send_success(last_crc_send, conn_id, new_flags);

	return status;
}

/*
 *
 */
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t old_id, uint8_t *flags) //header datei ändern
{
	/* Variable for next received header */
	uint8_t next_header[4];

	/* Variable for flags */
	uint8_t flags_to_send = *flags;

	/* Variable for next received data */
	uint8_t new_data[PACKAGE_DATA_SIZE];

	/* variable for last received CRC value */
	uint8_t last_crc_rcv = *crc_rcv;

	/* variable for last sended CRC value */
	uint8_t last_crc_send = *crc_send;

	/* variable for paketsize */
	int datacounter = 0;

	/* variable for loop */
	uint8_t end = 0;

	//status of the receiving buffer
	int status = XST_NO_DATA;

	//timer for receiving data
	int timer = 1;

	int success = 1;

	/*
	 * while not_end_flag
	 *
	 * loop for receiving long data until end_flag is set
	 */
	while(end != 1)
	{
		//Receiving answer
		while(status == XST_NO_DATA)
		{
			//timeout for receiving, reset timer for new sending
			if(timer == MAX_TIMER)
				timer = 0;

			// -> Send answer and wait for data
			if(timer == 0)
			{
				if(success == 1)
					// success == 1 -> send success
					send_success(&last_crc_send, &old_id, &flags_to_send);
				else
					//success == 0 -> send failure
					send_failure(&last_crc_send, &next_header[ID_POS]);
			}

			//increase timer
			timer++;

			//receive data if available
			status = UART_Recv_Buffer();

			//check status of receiving
			if(status != XST_NO_DATA || status != XST_SUCCESS)
				return XST_FAILURE;
		}

		//reset flags
		flags_to_send = 0x00;

		//extracting the header of the package
		extract_header(RecvBuffer, next_header, new_data);

		//CRC check
		if(check_crc(next_header[CRC_POS], next_header, new_data, last_crc_rcv) != XST_SUCCESS)
		{
			//failure
			success = 0;
			timer = 0;
			status = XST_NO_DATA;
			//send_failure(&last_crc_send, &next_header[ID_POS]);
			continue;
		}

		//
		set_ACK_Flag(&flags_to_send, 1);

		/*
		 * check ACK_flag in header
		 * ack = 0:	-> send last answer again
		 * 			-> go to top of the loop
		 *
		 * ack = 1: -> continue
		 *
		 */
		if(get_ACK_flag(&next_header[FLAGS_POS]) != ACK)
		{
			//failure
			success = 0;
			timer = 0;
			status = XST_NO_DATA;
			//send_failure(&last_crc_send, &old_id);
			continue;
		}

		//store the last received CRC value for next calculating
		last_crc_rcv = next_header[CRC_POS];

		//check if this package was the last data package
		if(get_end_flag(&next_header[FLAGS_POS]) == 1)
			end = 1;

		//data buffer
		for(int bytes = 0; bytes < next_header[DATA_SIZE_POS]; bytes++)
			databuffer[datacounter * PACKAGE_DATA_SIZE + bytes] = new_data[bytes];

		datacounter += next_header[DATA_SIZE_POS];

		//reset timer
		timer = 0;

		status = XST_NO_DATA;

		success = 1;

	}// end while

	//check type
	uint8_t id = next_header[ID_POS] & TM_MASK;

	uint8_t data[datacounter];

	for(int byte = 0; byte < datacounter; byte++)
		data[byte] = databuffer[byte];
	//
	switch(id)
	{
		//received data is tc
		case TC_MASK:	recv_TC(next_header, data); break;

		//received data is tm
		case TM_MASK:	recv_TM(); break;

		//...
		default: 		default_operation();
	}

	return XST_SUCCESS;
}

/*
 *
 */
int extract_header(uint8_t *rcvBuffer, uint8_t *header, uint8_t *data)
{
	for(int header_pos = 0; header_pos < HEADER_SIZE; header_pos++)
		header[header_pos] = rcvBuffer[header_pos];

	for(int data_byte = HEADER_SIZE; data_byte < BUFFER_SIZE; data_byte++)
		data[data_byte - HEADER_SIZE] = rcvBuffer[data_byte];

	return XST_SUCCESS;
}

/*
 * Sends an answer without set the ACK flag
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   *old_id:	  the address of the id of the received package
 *
 * return: The success or failure of sending the answer
 */
int send_failure(uint8_t *last_crc, uint8_t *old_id)
{
	//all flags are set to zero
	uint8_t failure_flags = UNSET_ALL_FLAGS;

	//header array
	uint8_t header[HEADER_SIZE];

	//empty data array
	uint8_t empty_data[] = {0};

	//set negative-acknowledge flag
	set_ACK_Flag(&failure_flags, NACK);

	//fill the header for the package to send
	fill_header(header, *old_id, empty_data, EMPTY_DATA_LENGTH, &failure_flags, last_crc);

	//send answer package
	int status = UART_answer();

	return status;
}

/**
 * Sends an answer with set ACK flag and the wanted flags
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   *old_id:	  the address of the id of the received package
 * 		   *flags: 	  the address of the flags to send
 *
 * return: The success or failure of sending the answer
 */
int send_success(uint8_t *last_crc, uint8_t *old_id, uint8_t *flags)
{
	//header array
	uint8_t header[HEADER_SIZE];

	//empty data array
	uint8_t empty_data[] = {0};

	//fill the header for the package to send
	fill_header(header, *old_id, empty_data, EMPTY_DATA_LENGTH, flags, last_crc);

	//send answer package
	int status = UART_answer();

	return status;

}

/*
 * !!! Noch zu deklarieren !!!
 */
int UART_answer()
{

	return XST_SUCCESS;
}

/**
 * !!! Überarbeiten !!!
 */
int recv_TC(uint8_t *header, u8 *databytes)
{
	uint8_t id = header[ID_POS];

	switch(id)
	{
		case CAMERA_TC: break;
		case UART_TC: break;
		case CPU_TC: break;
		case BRAM_TC: break;
		case DOWNLINK_TC: break;
		case DAC_TC: break;
		default: return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * !!! Noch zu deklarieren !!!
 */
int recv_TM()
{

}

void default_operation()
{

}
