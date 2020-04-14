/*
 * UART_EIVE_PROTOCOL.h
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#ifndef SRC_UART_EIVE_PROTOCOL_H_
#define SRC_UART_EIVE_PROTOCOL_H_



/************************************Include Files************************************/
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "UART_EIVE_Protocol_Flags.h"

/*************************************************************************************/

/** Size of package data and header **/
#define PACKAGE_DATA_SIZE 	28
#define HEADER_SIZE 		4

#define EMPTY_DATA_LENGTH 	0

#define MAX_TIMER 			100000

/** Identification mask for TM/TC **/
#define TC_MASK				0b00000000
#define TM_MASK				0b11110000

/** Identification numbers for TM/TC **/
#define CAMERA_TC			0b00000000
#define CAMERA_TM			0b11110000

#define UART_TC				0b00001010
#define UART_TM				0b11111010

#define BRAM_TC				0b00000101
#define BRAM_TM				0b11110101

#define CPU_TC				0b00001111
#define CPU_TM				0b11111111

#define DOWNLINK_TC			0b00001001
#define DOWNLINK_TM			0b11111001

#define DAC_TC				0b00000110
#define DAC_TM				0b11110110


/** Header position numbers**/
#define ID_POS  			0
#define CRC_POS 			1
#define DATA_SIZE_POS 		2
#define FLAGS_POS 			3

//Long buffer for receiving data
uint8_t databuffer[357143];

/******************** Functions to send ********************/

int UART_Send_Data(uint8_t ID, uint8_t *databytes[], int dataLength);
void request_to_send(uint8_t ID, uint8_t *temp, uint8_t *lastCRC);
int package_count(int dataLength);
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *last_CRC);
void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *flags, uint8_t *lastCRC);
int UART_ACK();

/******************** Functions to receive ********************/

/*
 * Receive data if its available.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 * 			XST_NO_DATA if no data was received.
 *
 */
int UART_Recv_Data();

/*
 * In this method the protocol for receiving data is implemented, which requires a connection
 * to the sender. A connection is established to receive packets longer than 28 bytes.
 * First, it is indicated that some data is to be transmitted, to which it responds that data
 * can be received. Then the actual data transmission takes place, where each received packet
 * is acknowledged positively or negatively.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 */
int receive();

/*
 * To connect with the sender, this method is used. This method checks the
 * received data for the send request flag and answers with the ready to receive flag.
 *
 * @param: *last_crc_send: 	The pointer to the last sent crc value.
 * 		   *new_flags:		The pointer to the flag value which will be send.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 */
int connection_establishment(uint8_t *last_crc_send, uint8_t *new_flags, uint8_t *conn_id);

/*
 * With this method data will be received from the connected sender.
 *
 * @param: *crc_rcv: The pointer to the last received crc value.
 * 		   *crc_send: The pointer to the last sent crc value.
 * 		   rcvd_id: the message-id in the header
 * 		   last_sent_flags: the last sent flags
 */
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t rcvd_id, uint8_t last_sent_flags);

/*
 * This method splits the received data into header and payload data.
 *
 * @param: *rcvBuffer: 	The pointer to the Buffer with the received data.
 * 		   *header: 	The pointer to the header array to store the extracted header.
 * 		   *data:		The pointer to the data array to store the extracted payload data.
 *
 */
int extract_header(const uint8_t *rcvBuffer, uint8_t *header, uint8_t *data);

/*
 * Sends an answer without set the ACK flag
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   *old_id:	  the address of the id of the received package
 *
 * @return: The success or failure of sending the answer
 */
int send_failure(uint8_t *last_crc, uint8_t old_id);

/*
 * Sends an answer with set ACK flag and the wanted flags
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   id:	  	  the id of the received package
 * 		   flags: 	  the flags to send
 *
 * @return: The success or failure of sending the answer
 */
int send_success(uint8_t *last_crc, uint8_t id, uint8_t flags);

/*
 * Sends the answer package with empty data
 *
 * @param: *header: the filled header array
 *
 * @return: The success or failure of sending the answer.
 */
int UART_answer(uint8_t *header);

/*
 * Data callback for received TC. Every subsystem gets his own command.
 *
 * @param: *header: pointer to the header array
 * 		   *databytes: pointer to the data array
 *
 * @return: Success or failure of the data callback
 *
 */
int recv_TC(uint8_t *header, u8 *databytes);

/*
 * This method is used to store the received TM.
 *
 */
int recv_TM();

/*
 *
 */
void default_operation();



#endif /* SRC_UART_EIVE_PROTOCOL_H_ */
