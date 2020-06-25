/**
 *
 *  Hello_world.c
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias

 *
 */

/************************************Include Files************************************/
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "UART_io.h"
#include "UART_EIVE_Protocol.h"
#include "sleep.h"
/*************************************************************************************/

int UART_Send_Hello_World(XUartPs *UartDevice);
int UART_Recv_Hello_World(XUartPs *UartDevice);
int UART_Protocol_Test(XUartPs *UartDevice);

/*
 * Main Method for UART
 */
int main(void)
{
	Initialize_UART_Device(UART0_DEVICE_ID);
	while(1)
	{
		UART_Protocol_Test(&Uart_Ps);
	}
}

/* Method for the Sending UART low level test */
int UART_Send_Hello_World(XUartPs *UartDevice)
{
	XStatus status;

	for(int i = 0; i < 31; i++)
	{
		SendBuffer[i] = i + 'A';
	}

	status = UART_Send_Buffer(SendBuffer);

	if(status != XST_SUCCESS)
		return XST_FAILURE;

	return XST_SUCCESS;
}

/* Method for the Receiving UART low level test */
int UART_Recv_Hello_World(XUartPs *UartDevice)
{
	int status = 0;

	status = UART_Recv_Buffer(RecvBuffer);

	RecvBuffer[BUFFER_SIZE - 1] = '\0';

	for(int i = 0; i < BUFFER_SIZE; i++)
		xil_printf("%c", RecvBuffer[i]);
	return XST_SUCCESS;
}

/* Method for testing the UART EIVE Protocol */
int UART_Protocol_Test(XUartPs *UartDevice)
{
	int status = XST_SUCCESS;
	do{
		status = UART_Recv_Data();
	} while(status == XST_NO_DATA);

	if(status != XST_SUCCESS)
		return XST_FAILURE;
	return XST_SUCCESS;
}
