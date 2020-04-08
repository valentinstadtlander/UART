/*
 * UART_io.h
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#ifndef SRC_UART_IO_H_
#define SRC_UART_IO_H_


/************************************Include Files************************************/
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
/*************************************************************************************/

/**
 * Definitions for UART interface
 * UART0_Device
 * UART1_Device
 */

#define UART0_DEVICE_ID              XPAR_XUARTPS_0_DEVICE_ID
#define UART1_DEVICE_ID              XPAR_XUARTPS_1_DEVICE_ID


#define BUFFER_SIZE  32
#define END_OF_TRANSMISSION '\n'

#define NO_DATA_AVAILABLE 2L

static XUartPs Uart_Ps;

static u8 SendBuffer[BUFFER_SIZE];
static u8 RecvBuffer[BUFFER_SIZE];

int Initialize_UART_Device(u16 DeviceID);
int UART_Send_Buffer(u8 SendBuffer[BUFFER_SIZE]);
int UART_Recv_Buffer();

int UART_Send(u8 *data, int dataLength);

#endif /* SRC_UART_IO_H_ */
