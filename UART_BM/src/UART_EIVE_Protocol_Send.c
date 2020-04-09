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

#define MAX_TIMER		10000


int UART_Send_Data(uint8_t ID, uint8_t *databytes, int dataLength)
{

	int packageCount = package_count(dataLength);
	uint8_t temp[BUFFER_SIZE * packageCount];
	uint8_t temp32[BUFFER_SIZE];
	uint8_t header[4];
	uint8_t data[PACKAGE_DATA_SIZE];

	/*
	 * lastCRC_send saves the calculated CRC for the last send package
	 * submittedCRC always saves the new received CRC
	 * lastCRC_rcv saves the last received CRC for the initval for checking the received package
	 */
	uint8_t lastCRC_send, lastCRC_rcvd = 0x00, submittedCRC;

	uint8_t flags = 0x00;
	XStatus status = XST_NO_DATA;
	int timer = 0;

	//Request to send, CRC initval = 0x00
	lastCRC_send = 0x00;
	request_to_send(ID, &temp, &lastCRC_send, &flags);

	//send request to send
	UART_Send(temp, 1);

	while(wait_on_answer(temp) != XST_SUCCESS);

	//fill header, data, flags and submittedCRC with the received values
	get_received_data(&header, &data, &flags, &submittedCRC);

	//check received CRC
	if(check_rcvd_CRC(submittedCRC, &header, &data, &lastCRC_rcvd) != XST_SUCCESS)
	{
		send_NACK(); //define method
		uint8_t
		while(wait_on_answer() != XST_SUCCESS)
	}






	if(check_rcvd_CRC(submittedCRC, &header, &data, &lastCRC_rcvd) == XST_SUCCESS)
	{
		//Received CRC is correct
		//check ACK
		if((header[FLAGS_POS] | ACK_MASK) == ACK_DEC && (header[FLAGS_POS] | READY_TO_RECV_MASK) == READY_TO_RECV_DEC)
		{
			//ACK
			//ready to receive
			send_data(); //define method

		}
		else
		{
			/*
			 * CRC is correct
			 * NACK
			 * NOT ready to receive
			 * Restart
			 */
			flags = REQ_TO_SEND_MASK & ACK_MASK;
			UART_Send_Data(ID, &databytes, dataLength); //recursive method call
		}
	}
	else
	{
		//Received CRC is wrong
		//send NACK

	}






	return status;
}

/*
 *Request to send, to establish a connection

 *
 *@ID		Identification number of the package to send
 *@*lastCRC	last CRC value to save the new CRC value for the next package
 *
 *configures a package to send a request to send and saves the first CRC
 */
void request_to_send(uint8_t ID, uint8_t *temp, uint8_t *lastCRC, uint8_t *flags)
{
	if(flags == 0x00)
	{
		*flags = REQ_TO_SEND_MASK;
	}
	uint8_t data[28] = {0};
	uint8_t datasize = 0;
	uint8_t temp_for_CRC[BUFFER_SIZE - 1] = {ID, EMPTY_DATA_LENGTH, flags, data};
	uint8_t newCRC = calc_crc8(temp_for_CRC, BUFFER_SIZE-1, lastCRC);
	uint8_t temp32[BUFFER_SIZE] = {ID, newCRC, datasize, flags, data};

	(*lastCRC) = newCRC;
	temp = temp32;
}

/*
 * Package counter
 *
 * @dataLength	number of bytes of the data to send
 *
 * returns the needed packages to send all the databytes
 */
int package_count(int dataLength)
{
	if (dataLength % PACKAGE_DATA_SIZE > 0)
		return (dataLength / PACKAGE_DATA_SIZE + 1);
	else
		return (dataLength / PACKAGE_DATA_SIZE);
}

/*
 * Checks if the received CRC matches with the calculated CRC of the package
 *
 * @param: 	submittedCRC, CRC value delivered by the receiver
 * @param:	*header, header of the received package
 * @param:	*data, data of the received package
 * @param:	*lastCRC_rcvd, CRC value of the previously received package, 0x00 for the request to send answer
 *
 * @return:	XST_SUCCESS if the received CRC matches with the calculated CRC
 * @return:	XST_FAILURE if it doesn't matches
 */
int check_rcvd_CRC(uint8_t submittedCRC, uint8_t *header, uint8_t *data, uint8_t *lastCRC_rcvd)
{
	if(check_crc(submittedCRC, &header, &data, &lastCRC_rcvd) == XST_SUCCESS) //check passing argument 4
	{
		//CRC value is correct
		return XST_SUCCESS;
	}
	else
		return XST_FAILURE;
}

/*
 *
 */
void get_received_data(uint8_t *header, uint8_t *data, uint8_t *flags, uint8_t *submittedCRC)
{
	extract_header(RecvBuffer, &header, &data);
	flags = header[FLAGS_POS];
	submittedCRC = header[CRC_POS];
}

void send_data(uint8_t ID, uint8_t *databytes, int dataLength, int packageCount, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd, uint8_t *temp)
{
	uint8_t send_array[BUFFER_SIZE];

	uint8_t header[HEADER_SIZE];
	uint8_t data[PACKAGE_DATA_SIZE];
	uint8_t flags;
	uint8_t submittedCRC;

	//fill array temp with the databytes and the header to send
	fill_packages(ID, dataLength, &databytes, &temp, packageCount, lastCRC_send);

	for(int packageCounter = 0; packageCounter < packageCount; packageCounter++)
	{
		//first package
		if(packageCounter == 0)
		{
			for(int i = 0; i < BUFFER_SIZE; i++)
			{
				send_array[i] = temp[i];
				UART_Send(send_array, 1);

				//wait on receive buffer to be filled
				wait_on_answer(&send_array);

				//get received information
				get_received_data(&header, &data, &flags, &submittedCRC);

				//check received CRC
				//check again!!!
				if(check_rcvd_CRC(submittedCRC, &header, &data, &lastCRC_rcvd) == XST_SUCCESS)
				{
					//Received CRC is correct
					//check ACK
					if((header[FLAGS_POS] | ACK_MASK) == ACK_DEC)
					{
						//ACK
						//continue with next package
					}
					else
					{
						/*
						 * CRC is correct
						 * NACK
						 * NOT ready to receive
						 * Restart
						 */
						UART_Send_Data(ID, &databytes, dataLength); //recursive method call
					}
				}
				else
				{
					//Received CRC is wrong
					//send NACK

					//wait on answer to check again
				}
		}

	}

		//middle packages
		else if(packageCounter != 0 && packageCounter != packageCount - 1)
		{

		}

		//last package
		else
		{

		}
	}
}

/*
 *
 */
int wait_on_answer(uint8_t *send_array)
{
	XStatus status = XST_NO_DATA;
	int timer;

	while(status != XST_SUCCESS)
	{
		status = UART_Recv_Buffer();

		if(status != XST_NO_DATA || status != XST_SUCCESS)
			return XST_FAILURE;

		timer++;

		if(timer == MAX_TIMER)
		{
			//Timeout
			//send again array to send
			UART_Send(send_array, 1);

			//reset timer
			timer = 0;
		}
	}
	return XST_SUCCESS;
}


/*
 * Method to fill the packages to send

 *
 * @ID 				Identification number of the package to send
 * @dataLength		length of the data to send, must be given by the user
 * @databytes[]		data to send
 * @temp			temporary array in the main method with the length of BUFFER_SIZE * packageCount,
 * 					which is filled with the header and the databytes
 * @packageCount	numbers of packages needed to send all the databytes
 * @flags			uint8_t number with the flags which are set
 * @last_CRC		CRC value of the last calculation of the last send package
 *
 * fills the variable temp with the databytes and the headers
 */
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *lastCRC_send)
{
	/*Temporary arrays for header and data*/
	uint8_t header[4];
	uint8_t temp28[28];
	uint8_t *flags;

	for (int i = 0; i < packageCount; i++)
	{
		/*first package*/
		if(i = 0){
			for (int j = 0; j < 28; j++)
			{
				/*fill temporary arrays temp and temp28*/
				temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];
				temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
			}

			/*
			 * fill header with the given information
			 * flags for the start package
			 */
			set_Start_Flag(*flags, 1);
			fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i, flags, &lastCRC_send);

			/*fill temporary array temp with the headers*/
			for (int k = 28; k < 32; k++)
			{
				temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
			}
		}

		/*all packages except the first and the last one*/
		else if (i > 0 && i != packageCount - 1)
		{
			/*fill temporary arrays temp and temp28*/
			for (int j = 0; j < 28; j++) {
				temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];
				temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
			}

			/*
			 * fill header with the given information*/
			/* flags for the middle packages
			 */
			flags = 0b00000000;
			fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i, flags, &lastCRC_send);

			/*fill temporary array temp with the headers*/
			for (int k = 28; k < 32; k++)
			{
				temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
			}
		}

		/*last package*/
		else
		{
			int restsize = dataLength - PACKAGE_DATA_SIZE * (packageCount - 1);

			for(int j = 0; j < PACKAGE_DATA_SIZE; j++)
			{
				/*fill temp and temp28*/
				if(j < restsize)
				{
					/*fill with the rest databytes from position 0 to restsize*/
					temp[i * PACKAGE_DATA_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j];
					temp28[j] = *databytes[i * PACKAGE_DATA_SIZE + j];
				}
				else
				{
					/*fill with 0 from position restsize to 28*/
					temp[i * PACKAGE_DATA_SIZE + j] = 0;
					temp28[j] = 0;
				}
			}

			/*fill header with the given information*/
			/*flags for the end package*/
			set_End_Flag(*flags, 1);
			fill_header(header, ID, temp28, restsize, &i, flags, &lastCRC_send);

			/*fill temporary array temp with the headers*/
			for (int k = 28; k < 32; k++)
			{
				temp[i * PACKAGE_DATA_SIZE + k] = header[k - PACKAGE_DATA_SIZE];
			}
		}
	}

}

/*
 *Fill Header with submitted parameters

 *
 *@*header		the header is saved in this parameter
 *@ID 			Identification number of the package to send
 *@*databytes	data to send
 *@dataLength	length of the data to send
 **flags		flags of the package which is going to be send
 **lastCRC_send	last calculated CRC of the last send package
 *
 */
void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *flags, uint8_t *lastCRC_send)
{
	uint8_t temp_array_CRC[] = { ID, dataLength, flags, *databytes }; //funktioniert??

	/*calculate new CRC value*/
	uint8_t newCRC = calc_crc8(temp_array_CRC, dataLength, lastCRC_send);

	/*save new CRC value in old variable*/
	(*lastCRC_send) = newCRC;

	/*fill header*/
	header[ID_POS] = ID;
	header[CRC_POS] = newCRC;
	header[DATA_SIZE_POS] = dataLength;
	header[FLAGS_POS] = flags;
}

int UART_ACK()
{
	return XST_SUCCESS;
}

/*
 * Setter Methods for ACK-Flag
 *
 * @*flags 	flags array
 * @ack 	ACK or NACK
 */
void set_ACK_Flag(uint8_t *flags, uint8_t ack)
{
	if(ack == ACK)
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
 * Setter Methods for Rdy_to_rcv-Flag

 *
 * @*flags 		flags array
 * @rdy_to_rcv 	bit req_to_send
 */
void set_Rdy_to_rcv_Flag(uint8_t *flags, uint8_t rdy_to_rcv)
{
	if(rdy_to_rcv == 1)
		*flags |= READY_TO_RECV_MASK;
	else
		*flags &= ~READY_TO_RECV_MASK;
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
