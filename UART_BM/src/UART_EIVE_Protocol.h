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

/*************************************************************************************/

/** Size of package data and header **/
#define PACKAGE_DATA_SIZE 28
#define HEADER_SIZE 4

/** ACK and NACK **/
#define ACK 1
#define NACK 0


#define EMPTY_DATA_LENGTH 0

/** Identification mask for TM/TC **/
#define TC_MASK		0b00000000
#define TM_MASK		0b11110000

/** Identification numbers for TM/TC **/
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


/** Header position numbers**/
#define ID_POS  		0
#define CRC_POS 		1
#define DATA_SIZE_POS 	2
#define FLAGS_POS 		3

/**Masks for Flags**/
#define ACK_MASK            	0b10000000
#define REQ_TO_SEND_MASK    	0b01000000
#define READY_TO_RECV_MASK  	0b00100000
#define START_MASK          	0b00010000
#define END_MASK            	0b00001000

/**Flags number decimal**/
#define ACK_DEC            		128
#define REQ_TO_SEND_DEC    		64
#define READY_TO_RECV_DEC  		32
#define START_DEC          		16
#define END_DEC            		8

//Long buffer for receiving data
uint8_t databuffer[357143];

/**Functions to send**/
int UART_Send_Data(uint8_t ID, uint8_t *databytes[], int dataLength);
void request_to_send(uint8_t ID, uint8_t *temp, uint8_t *lastCRC);
int package_count(int dataLength);
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *last_CRC);
void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *flags, uint8_t *lastCRC);
int UART_ACK();
void set_ACK_Flag(uint8_t *flags, uint8_t ack);
void set_Req_to_send_Flag(uint8_t *flags, uint8_t req_to_send);
void set_Req_to_send_Flag(uint8_t *flags, uint8_t rdy_to_rcv);
void set_Start_Flag(uint8_t *flags, uint8_t start);
void set_End_Flag(uint8_t *flags, uint8_t end);

/**Functions to receive**/
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



#endif /* SRC_UART_EIVE_PROTOCOL_H_ */
