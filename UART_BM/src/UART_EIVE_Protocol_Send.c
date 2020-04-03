/*
 * UART_EIVE_Protocol.c


 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"
#include "UART_io.h"
#include "xparameters.h"

/**Masks for Flags**/
#define ACK_MASK            	0b10000000
#define REQ_TO_SEND_MASK    	0b01000000
#define READY_TO_RECV_MASK  	0b00100000
#define START_MASK          	0b00010000
#define END_MASK            	0b00001000
#define PCKG_LENGTH_MASK       	0b00000100

/**Flags number decimal**/
#define ACK_DEC            		128
#define REQ_TO_SEND_DEC    		64
#define READY_TO_RECV_DEC  		32
#define START_DEC          		16
#define END_DEC            		8
#define PCKG_LENGTH_DEC    		4


int UART_Send_Data(uint8_t ID, uint8_t *databytes[], int dataLength) {

	int packageCount = package_count(dataLength);
	uint8_t temp[BUFFER_SIZE * packageCount];
	XStatus status;

	//Request to send
	request_to_send(ID, temp, packageCount);

	//check ready to receive
	if(ready_to_receive() == XST_SUCCESS)
	{
		//uint8_t flags =
		//fill_packages(ID, dataLength, *databytes, *temp, packageCount, flags);

		//check packetsize
		//

	}

	return status;
}

void request_to_send(uint8_t ID, uint8_t *temp, int packageCount)
{
	uint8_t flags = REQ_TO_SEND_MASK;
	uint8_t databytes = {0};
	fill_packages(ID, 0, databytes, temp, packageCount, flags);
	UART_Send(temp, 1);
}

int ready_to_receive()
{
	uint8_t header[4];
	uint8_t data[28];

	UART_Recv_Buffer();
	extract_header(RecvBuffer, header, data); //extract header -method

	uint8_t flag_rdy_2_rcv = header[2] | READY_TO_RECV_MASK;

	if(flag_rdy_2_rcv != 16) //anpassen!
		return XST_FAILURE;
	else return XST_SUCCESS;

}

int package_count(int dataLength) {
	if (dataLength % PACKAGE_DATA_SIZE > 0)
		return (dataLength / PACKAGE_DATA_SIZE + 1);
	else
		return (dataLength / PACKAGE_DATA_SIZE);
}

void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *flags)  //flags
{
	uint8_t temp28[28];
	uint8_t header[4];

	for (int i = 0; i < packageCount; i++)
		{
			if (i != packageCount - 1)
			{
				for (int j = 0; j < 28; j++) {
					temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];

					temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
				}

				fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i, flags);

				for (int k = 28; k < 32; k++)
				{
					temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
				}
			} else
			{
				int restsize = dataLength - PACKAGE_DATA_SIZE * (packageCount - 1);

				for(int j = 0; j < PACKAGE_DATA_SIZE; j++)
				{
					if(j < restsize)
					{
						temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];
						temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
					}
					else
					{
						temp[i * PACKAGE_DATA_SIZE + j] = 0;
						temp28[j] = 0;
					}
				}

				fill_header(header, ID, temp28, restsize, &i, flags); //valid


				for (int k = 28; k < 32; k++)
				{
					temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
				}

			}
		}

}

void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, int *packageCount, uint8_t *flags) {


	header[0] = ID;

	header[2] = dataLength;

	header[3] = set_Flags(flags);

	uint8_t tempCRC[] = { ID, dataLength, flags, *databytes }; //funktioniert nicht!!
	header[1] = calc_crc8(tempCRC, dataLength);
}

uint8_t set_Flags(uint8_t *flags)
{
	set_ACK_Flag(flags, flags[7]);
	set_Req_to_send_Flag(flags, flags[6]);
	set_Start_Flag(flags, flags[4]);
	set_End_Flag(flags, flags[3]);
	set_Pckg_Length_Flag(flags, flags[1]);
}

int UART_ACK()
{

}

/*
 * Setter Methods for ACK-Flag
 *
 * @*flags 	flags array
 * @ack 	ACK or NACK
 */
void set_ACK_Flag(uint8_t *flags, uint8_t ack)
{
	if(ack == 1)
		*flags |= ACK_MASK;
	else
		*flags &= ~ACK_MASK;
}

/*
 * Setter Methods for Req_to_send-Flag
 *
 * @*flags 			flags array
 * @req_to_send 	bit req_to_send
 */
void set_Req_to_send_Flag(uint8_t *flags, uint8_t req_to_send)
{
	if(req_to_send == 1)
		*flags |= REQ_TO_SEND_MASK;
	else
		*flags &= ~REQ_TO_SEND_MASK;
}

/*
 * Setter Methods for Start-Flag
 *
 * @*flags 	flags array
 * @start 	first package
 */
void set_Start_Flag(uint8_t *flags, uint8_t start)
{
	if(start == 1)
		*flags |= START_MASK;
	else
		*flags &= ~START_MASK;
}

/*
 * Setter Methods for End-Flag
 *
 * @*flags 	flags array
 * @end 	last package
 */
void set_End_Flag(uint8_t *flags, uint8_t end)
{
	if(end == 1)
		*flags |= END_MASK;
	else
		*flags &= ~END_MASK;
}

/*
 * Setter Methods for pckg_length-Flag
 *
 * @*flags 		flags array
 * @pck_length 	package length
 */
void set_Pckg_Length_Flag(uint8_t *flags, uint8_t pckg_length)
{
	if(pckg_length == 1)
		*flags |= PCKG_LENGTH_MASK;
	else
		*flags &= ~PCKG_LENGTH_MASK;
}
