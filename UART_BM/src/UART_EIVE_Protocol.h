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

#define PACKAGE_DATA_SIZE 28

/** Header position numbers**/
#define ID_POS  		0
#define CRC_POS 		1
#define DATA_SIZE_POS 	2
#define FLAGS_POS 		3


/**Masks for Flags**/
#define ACK_MASK            0b00000010
#define TC_MASK             0b00000100
#define REQ_TO_SEND_MASK    0b00001000
#define READY_TO_RECV_MASK  0b00010000
#define START_MASK          0b00100000
#define END_MASK            0b01000000
#define END_CONN_MASK       0b10000000

/**Functions to send**/
int UART_Send_Data(u8 ID, u8 *databytes[], int dataLength);
int package_count(int dataLength);
void fill_header(u8 *header, u8 ID, u8 *databytes, int dataLength, int *packageCount);

/**Functions to receive**/
int UART_Recv_Data();
int extract_header(uint8_t *rcvBuffer, uint8_t *header, uint8_t *data);
int check_ACK_flag(uint8_t *flags);
int check_TC_flag(uint8_t *flags);
int check_Req_to_send_flag(uint8_t *flags);
int check_ready_to_recv_flag(uint8_t *flags);
int check_start_flag(uint8_t *flags);
int check_end_flag(uint8_t *flags);
int check_end_conn_flag(uint8_t *flags);


#endif /* SRC_UART_EIVE_PROTOCOL_H_ */
