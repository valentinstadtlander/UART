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



/**Functions to send**/
int UART_Send_Data(u8 ID, u8 *databytes[], int dataLength);
void request_to_send(uint8_t ID, uint8_t *temp, int packageCount);
int package_count(int dataLength);
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *flags);
void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, int *packageCount, uint8_t *flags);
uint8_t set_Flags(uint8_t *flags);
void set_ACK_Flag(uint8_t *flags, uint8_t val);

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
