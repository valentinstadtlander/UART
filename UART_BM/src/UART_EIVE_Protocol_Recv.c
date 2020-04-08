/*
 * UART_EIVE_Protocol_Recv.c

 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"

/*
#define HEADER_SIZE 4
#define ACK 1
#define NACK 0

#define EMPTY_DATA_LENGTH 0
*/

/** Identification mask for TM/TC **/
/*
#define TC_MASK		0b00000000
#define TM_MASK		0b11110000
*/

/** Identification numbers for TM/TC **/
/*
#define CAMERA_TC	0b00000000
#define CAMERA_TM	0b11110000

#define UART_TC		0b00001010
#define UART_TM		0b11111010

#define BRAM_TC		0b00000101
#define BRAM_TM		0b11110101

#define CPU_TC		0b00001111
#define CPU_TM		0b11111111

#define DOWNLINK_TC	0b00001001
#define DOWNLINK_TM	0b11111001

#define DAC_TC		0b00000110
#define DAC_TM		0b11110110
*/

//Long buffer for receiving data
uint8_t databuffer[357143];

//Functions for receiving
int UART_Recv_Data();
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t *old_id, uint8_t *flags);
int extract_header(uint8_t *rcvBuffer, uint8_t *header, uint8_t *data);
int check_ACK_flag(uint8_t *flags);
int check_Req_to_send_flag(uint8_t *flags);
int check_ready_to_recv_flag(uint8_t *flags);
int check_start_flag(uint8_t *flags);
int check_end_flag(uint8_t *flags);
int send_failure(uint8_t *last_crc, uint8_t *old_id);
int send_success(uint8_t *last_crc, uint8_t *old_id, uint8_t *flags);
int UART_answer();
int recv_TC(uint8_t *header, u8 *databytes);
int recv_TM();
void default_operation();




/**
 *
 */
int UART_Recv_Data()
{
	uint8_t header[4];

	uint8_t new_flags = 0x00;

	uint8_t data[28];

	uint8_t last_crc_send = 0x00;

	UART_Recv_Buffer();

	//extract header from buffer
	extract_header(RecvBuffer, header, data);

	/*
	 * crc_check
	 *
	 */
	if(check_crc(header[CRC_POS], header, data, INIT_CRC) != XST_SUCCESS)
	{

		send_failure(&last_crc_send, &header[ID_POS]);

		return XST_FAILURE;
	}

	//set acknowledge flag
	set_ACK_Flag(&header[FLAGS_POS], ACK);

	/*
	 * check request 2 send
	 *
	 *
	 */
	if(check_Req_to_send_flag(&header[FLAGS_POS]) == 0)
	{
		send_failure(&last_crc_send, &header[ID_POS]);

		return XST_FAILURE;
	}

	set_rdy_2_recv(&new_flags, 1); //noch zu deklarieren


	int status = receive_data(&header[CRC_POS], &last_crc_send, &header[ID_POS], &header[FLAGS_POS]);

	if(status == XST_FAILURE)
		return XST_FAILURE;

	return XST_SUCCESS;
}

/**
 *
 */
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t *old_id, uint8_t *flags)
{
	/* Variable for next received header */
	uint8_t next_header[4];

	/* Variable for flags */
	uint8_t flags_to_send = 0x00;

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

	// -> Send answer and wait for data
	send_success(&last_crc_send, old_id, flags); //?


	/*
	 * while not_end_flag
	 *
	 * loop for receiving long data until end_flag is set
	 */
	while(end != 1)
	{
		flags_to_send = 0x00;

		//Receiving answer
		UART_Recv_Buffer();

		//extracting the header of the package
		extract_header(RecvBuffer, next_header, new_data);

		//CRC check
		if(check_crc(next_header[CRC_POS], next_header, new_data, last_crc_rcv) != XST_SUCCESS)
		{
			//failure
			send_failure(&last_crc_send, &next_header[ID_POS]);
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
		if(check_ACK_flag(&next_header[FLAGS_POS]) != ACK)
		{
			send_failure(&last_crc_send, old_id); //checken
			continue;
		}

		//store the last received CRC value for next calculating
		last_crc_rcv = next_header[CRC_POS];

		//check if this package was the last data package
		if(check_end_flag(&next_header[FLAGS_POS]) == 1)
			end = 1;

		//data buffer
		for(int bytes = 0; bytes < next_header[DATA_SIZE_POS]; bytes++)
			databuffer[datacounter * PACKAGE_DATA_SIZE + bytes] = new_data[bytes];

		datacounter += next_header[DATA_SIZE_POS];

		//send success
		send_success(&last_crc_send, old_id, &flags_to_send);

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


/**
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
int send_failure(uint8_t *last_crc, uint8_t *old_id)
{
	uint8_t failure_flags;

	uint8_t header[HEADER_SIZE];

	uint8_t empty_data[] = {0};

	//set negative-acknowledge flag
	set_ACK_Flag(&failure_flags, NACK);

	fill_header(&header, old_id, &empty_data, EMPTY_DATA_LENGTH, &failure_flags, last_crc); //Package_Count wurde entfernt

	//*last_crc = header[CRC_POS];

	//send answer
	int status = UART_answer(); //Noch zu deklarieren

	return status;
}

/**
 *
 */
int send_success(uint8_t *last_crc, uint8_t *old_id, uint8_t *flags)
{
	uint8_t header[HEADER_SIZE];

	uint8_t empty_data[] = {0};

	fill_header(&header, old_id, &empty_data, EMPTY_DATA_LENGTH, flags, last_crc); //Package_Count wurde entfernt

	int status = UART_answer(); //Noch zu deklarieren

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
