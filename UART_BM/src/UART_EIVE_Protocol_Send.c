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

/**Flags number decimal**/
#define ACK_DEC            		128
#define REQ_TO_SEND_DEC    		64
#define READY_TO_RECV_DEC  		32
#define START_DEC          		16
#define END_DEC            		8


int UART_Send_Data(uint8_t ID, uint8_t *databytes[], int dataLength) {

	int packageCount = package_count(dataLength);
	uint8_t temp[BUFFER_SIZE * packageCount];
	uint8_t temp32[BUFFER_SIZE];
	uint8_t header[4];
	uint8_t data[BUFFER_SIZE];
	XStatus status;
	uint8_t lastCRC_send, lastCRC_rcvd = 0x00, submittedCRC;
	uint8_t flags;

	//Request to send, CRC initval = 0x00
	lastCRC = 0x00;
	request_to_send(ID, &temp, &lastCRC);

	//send request to send
	UART_Send(temp, 1);

	//check ready to receive
	//check ACK
	//check CRC
	UART_Recv_Buffer(); //check method and using!!
	extract_header(RecvBuffer, &header, &data);
	flags = header[FLAGS_POS];
	submittedCRC = header[CRC_POS];

	if(check_crc(submittedCRC, header, data, 0x00) == XST_SUCCESS) //crc is okay
	{
		//check ACK Flag
		if(check_ACK_flag(*flags) == 1)
		{

		} else
		{
			UART_Send_Data(ID, &databytes, dataLength); //recursive method call possible??
		}


		if(check_ACK_flag(*flags) == 1 && check_ready_to_recv_flag(*flags) == 1)
		{

		}
	} else
	{
		set_ACK_Flag(*flags, NACK);
		flags &= ACK_MASK;



		//fill temp32 array
		temp32[ID_POS] = ID;
		temp32[CRC_POS] = lastCRC;
		temp32[DATA_SIZE_POS] = dataLength;
		temp32[FLAGS_POS] = flags;

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
void request_to_send(uint8_t ID, uint8_t *temp, uint8_t *lastCRC)
{
	uint8_t flags = REQ_TO_SEND_MASK;
	uint8_t data[28] = {0};
	uint8_t datasize = 0;
	uint8_t temp_for_CRC[BUFFER_SIZE - 1] = {ID, EMPTY_DATA_LENGTH, flags, data};
	uint8_t newCRC = calc_crc8(temp_for_CRC, BUFFER_SIZE-1, lastCRC);
	uint8_t temp32[BUFFER_SIZE] = {ID, newCRC, datasize, flags, data};

	&lastCRC = newCRC;
	temp = temp32;
}

/*
 * Package counter
 *
 * @dataLength	number of bytes of the data to send
 *
 * returns the needed packages to send all the databytes
 */
int package_count(int dataLength) {
	if (dataLength % PACKAGE_DATA_SIZE > 0)
		return (dataLength / PACKAGE_DATA_SIZE + 1);
	else
		return (dataLength / PACKAGE_DATA_SIZE);
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
 * @last_CRC		CRC value of the last calculation
 *
 * fills the variable temp with the databytes and the headers
 */
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount, uint8_t *last_CRC)
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
			fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i, flags, &last_CRC);

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
			fill_header(header, ID, temp28, PACKAGE_DATA_SIZE, &i, flags, &last_CRC);

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
			fill_header(header, ID, temp28, restsize, &i, flags, &last_CRC);

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
 **lastCRC		last calculated CRC for the new CRC calculation
 *
 */
void fill_header(uint8_t *header, uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *flags, uint8_t *lastCRC)
{
	uint8_t temp_array_CRC[] = { ID, dataLength, flags, *databytes }; //funktioniert??

	/*calculate new CRC value*/
	uint8_t newCRC = calc_crc8(temp_array_CRC, dataLength, &lastCRC);

	/*save new CRC value in old variable*/
	(*lastCRC) = newCRC;

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
void set_Req_to_send_Flag(uint8_t *flags, uint8_t rdy_to_rcv)
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
