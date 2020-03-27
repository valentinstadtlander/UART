/*
 * UART_EIVE_Protocol.c

 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
#include "UART_io.h"
#include "CRC.h"

int UART_Send_Data(u8 ID, u8 *databytes[], int dataLength) {

	uint8_t header[4];
	int packageCount;
	packageCount = package_count(dataLength);
	uint8_t temp28[28];
	uint8_t temp[BUFFER_SIZE * packageCount];

	for (int i = 0; i < packageCount; i++)
	{
		if (i != packageCount - 1)
		{
			for (int j = 0; j < 28; j++) {
				temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];

				temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
			}

			fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i);

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

			fill_header(header, ID, temp28, restsize, &i);


			for (int k = 28; k < 32; k++)
			{
				temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
			}

		}
	}

	XStatus status = UART_Send(temp, packageCount);

	return status;
}

int package_count(int dataLength) {
	if (dataLength % PACKAGE_DATA_SIZE > 0)
		return (dataLength / PACKAGE_DATA_SIZE + 1);
	else
		return (dataLength / PACKAGE_DATA_SIZE);
}

void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, int *packageCount) {
	uint8_t flags;


	header[0] = ID;

	header[2] = dataLength;

	header[3] = flags;

	uint8_t tempCRC[] = { ID, dataLength, flags, *databytes };
	header[1] = calc_crc8(tempCRC, dataLength);
}

uint8_t set_Flags(uint8_t ID, int *packageCount, )
{

}

int UART_ACK()
{

}

void set_ACK_Flag(uint8_t *flags, uint8_t val)
{
	if(val == 1)
		*flags |= ACK_MASK;
	else
		*flags &= ~ACK_MASK;
}
