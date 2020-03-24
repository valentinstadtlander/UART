/**
 *
 *
 *
 *
 *
 */

/************************************Include Files************************************/
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
/*************************************************************************************/

/**
 * Definitions
 */

#define UART0_DEVICE_ID              XPAR_XUARTPS_0_DEVICE_ID
#define UART1_DEVICE_ID              XPAR_XUARTPS_1_DEVICE_ID

XUartPs Uart_Ps;		/* Instance of the UART Device */

#define BUFFER_SIZE 32
#define END_OF_TRANSMISSION '\n'

static u8 SendBuffer[BUFFER_SIZE];
static u8 RecvBuffer[BUFFER_SIZE];

int Initialize_UART_Device(u16 DeviceID);
int UART_Send_Buffer(XUartPs *UartDevice, u8 SendBuffer[BUFFER_SIZE]);
int UART_Send_Hello_World(XUartPs *UartDevice);
int UART_Recv_Hello_World(XUartPs *UartDevice);

/**
 * Main Method
 */
int main(void)
{
	Initialize_UART_Device(UART0_DEVICE_ID);
	while(1)
	{
		UART_Recv_Hello_World(&Uart_Ps);
	}
}



int Initialize_UART_Device(u16 DeviceID)
{
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


/**
 * UART_Hello_World Method
 */

int UART_Send_Hello_World(XUartPs *UartDevice)
{
	if(UartDevice->IsReady != XIL_COMPONENT_IS_READY){
		return XST_FAILURE;
	}

	for(int i = 0; i < 31; i++)
	{
		SendBuffer[i] = i + 'A';
	}

	UART_Send_Buffer(UartDevice, SendBuffer);

	return XST_SUCCESS;
}

int UART_Recv_Hello_World(XUartPs *UartDevice)
{
	if(UartDevice->IsReady != XIL_COMPONENT_IS_READY){
			return XST_FAILURE;
		}

	for (u32 Index = 0; Index < BUFFER_SIZE; Index++) {
				RecvBuffer[Index] = 0;
			}

	/* Wait until there is data */
	while (!XUartPs_IsReceiveData(UartDevice->Config.BaseAddress));

	/* Block receiving the buffer. */
	u32 ReceivedCount = 0;
	while (ReceivedCount < BUFFER_SIZE) {
		ReceivedCount +=
			XUartPs_Recv(&Uart_Ps, &RecvBuffer[ReceivedCount],
				      (BUFFER_SIZE - ReceivedCount));

		if(RecvBuffer[ReceivedCount-1 ] == END_OF_TRANSMISSION) break;
		}

	UART_Send_Buffer(UartDevice, RecvBuffer);
	return XST_SUCCESS;
}

int UART_Send_Buffer(XUartPs *UartDevice, u8 SendBuffer[BUFFER_SIZE])
{
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


