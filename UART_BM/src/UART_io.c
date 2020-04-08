/*
 * UART_io.c

 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

/************************************Include Files************************************/
#include "UART_io.h"
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
/*************************************************************************************/

/*
 *
 */
int Initialize_UART_Device(u16 DeviceID) {
	XStatus Status;
	XUartPs_Config *Config;
	u16 Index;

	Config = XUartPs_LookupConfig(DeviceID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Check hardware build. */
	Status = XUartPs_SelfTest(&Uart_Ps);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Change operation mode. */
	XUartPs_SetOperMode(&Uart_Ps, XUARTPS_OPER_MODE_NORMAL);

	for (Index = 0; Index < BUFFER_SIZE; Index++) {
		RecvBuffer[Index] = 0;
	}

	return XST_SUCCESS;
}

/*
 *
 */
int UART_Send_Buffer(u8 SendBuffer[BUFFER_SIZE]) {

	if (Uart_Ps.IsReady != XIL_COMPONENT_IS_READY) {
		return XST_FAILURE;
	}

	u32 SentCount;
	u32 LoopCount = 0;

	/* Block sending the buffer. */
	SentCount = XUartPs_Send(&Uart_Ps, SendBuffer, BUFFER_SIZE);
	if (SentCount != BUFFER_SIZE) {
		return XST_FAILURE;
	}

	while (XUartPs_IsSending(&Uart_Ps)) {
		LoopCount++;
	}

	return XST_SUCCESS;
}

/*
 *
 */
int UART_Recv_Buffer() {
	if (Uart_Ps.IsReady != XIL_COMPONENT_IS_READY) {
		return XST_FAILURE;
	}

	for (u32 Index = 0; Index < BUFFER_SIZE; Index++) {
		RecvBuffer[Index] = 0;
	}

	/* Wait until there is data */
	/*while (!XUartPs_IsReceiveData(Uart_Ps.Config.BaseAddress));*/

	/* Break if no data  */
	 if(!XUartPs_IsReceiveData(Uart_Ps.Config.BaseAddress))
		 return NO_DATA_AVAILABLE;


	/* Block receiving the buffer. */
	u32 ReceivedCount = 0;
	while (ReceivedCount < BUFFER_SIZE) {
		ReceivedCount += XUartPs_Recv(&Uart_Ps, &RecvBuffer[ReceivedCount],
				(BUFFER_SIZE - ReceivedCount));

		/*if (RecvBuffer[ReceivedCount - 1] == END_OF_TRANSMISSION)
			break;*/
	}
	return XST_SUCCESS;
}

/*
 *
 */
int UART_Send(u8 *data, int packageCount)
{
	for(int packageNumb = 0; packageNumb < packageCount; packageNumb++)
	{
		for(int byteNumb = 0; byteNumb < 31; byteNumb++)
		{
			SendBuffer[byteNumb] = *data[packageNumb*BUFFER_SIZE + byteNumb];

			UART_Send_Buffer(SendBuffer);
		}
	}
}

