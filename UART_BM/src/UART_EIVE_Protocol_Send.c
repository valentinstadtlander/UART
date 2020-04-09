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


int UART_Send_Data(uint8_t ID, uint8_t *databytes, int dataLength)
{
	//establish connection method
	//while schleife um connection establishment after checkACK and ACK = 0
	//wait_on_answer for NACK

	int packageCount = package_count(dataLength);
	uint8_t temp[BUFFER_SIZE * packageCount];
	uint8_t temp32[BUFFER_SIZE];
	uint8_t header[HEADER_SIZE];
	uint8_t data[PACKAGE_DATA_SIZE];
	int status;

	/*
	 * lastCRC_send saves the calculated CRC for the last send package
	 * submittedCRC always saves the new received CRC
	 * lastCRC_rcv saves the last received CRC for the initval for checking the received package
	 */
	uint8_t lastCRC_send, lastCRC_rcvd = 0x00, submittedCRC;

	uint8_t flags = UNSET_ALL_FLAGS;

	//Request to send, CRC initval = 0x00
	lastCRC_send = 0x00;

	status = send_request_to_send(ID, &temp32, &lastCRC_send, &flags);

	if(status != XST_SUCCESS)
		return XST_FAILURE;

	int acknowledge = NACK;

	while(acknowledge != ACK)
	{
		wait_on_answer(temp32);

		//fill header, data, flags and submittedCRC with the received values
		get_received_data(&header, &data, &flags, &submittedCRC);

		int status;
		//check received CRC
		status = check_crc(submittedCRC, RecvBuffer, lastCRC_rcvd);


		if(check_crc(submittedCRC, RecvBuffer, lastCRC_rcvd)!= XST_SUCCESS)
		{
			//CRC values defeer
			send_NACK(); //define method

		}
		else
		{
			acknowledge = ACK;
		}
	}

		//Received CRC is correct
		//check ACK
		//check ready to receive
		if(get_ACK_flag(&flags) == SET && get_ready_to_recv_flag(*flags) == SET)
		{
			send_data();
		}
		else
		{
			//NACK
			//NOT ready to receive
		}

		send_data();
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
int send_request_to_send(uint8_t ID, uint8_t *temp32, uint8_t *lastCRC_send, uint8_t *flags)
{
	if(flags == 0x00)
	{
		*flags = REQ_TO_SEND_MASK;
	}

	//anpassen
	(*lastCRC_send) = calc_crc8(temp_for_CRC, BUFFER_SIZE-1, *lastCRC_send);

	temp32[ID_POS] = ID;
	temp32[CRC_POS] = *lastCRC_send;
	temp32[DATA_SIZE_POS] = 0;
	temp32[FLAGS_POS] = *flags;

	//(*lastCRC_send) = newCRC;

	int status = XST_FAILURE;
	int try = 0;

	while(status != XST_SUCCESS)
	{
		status = UART_Send(temp32, 1);

		if(try == 10)
			return XST_FAILURE;

		try++;
	}

	return XST_SUCCESS;
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

/*
 *
 */
void get_received_data(uint8_t *header, uint8_t *data, uint8_t *flags, uint8_t *submittedCRC)
{
	extract_header(RecvBuffer, header, data);
	*flags = header[FLAGS_POS];
	*submittedCRC = header[CRC_POS];
}

void send_data(uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd)
{
	uint8_t send_array[BUFFER_SIZE];
	uint8_t packageCount = package_count(dataLength);
	uint8_t header[HEADER_SIZE];
	uint8_t data[PACKAGE_DATA_SIZE];
	uint8_t flags;
	uint8_t submittedCRC;
	uint8_t temp[BUFFER_SIZE * packageCount];
	int status;

	//fill array temp with the databytes and the header to send
	fill_packages(ID, dataLength, &databytes, temp, packageCount);

	int package_counter = 0;

	while(package_counter < packageCount || try = 10)
	{
		//Get packages
		for(int i = 0; i < BUFFER_SIZE; i++)
			send_array[i] = temp[package_counter * BUFFER_SIZE + i];

		//Set acknowledge flag
		set_ACK_Flag(send_array[FLAGS_POS], ACK);

		//Calculate CRC value
		send_array[CRC_POS] = calc_crc8(send_array, *lastCRC_send);

		//Send package
		status = UART_Send(send_array, 1);

		if(status != XST_SUCCESS)
		{
			try++;
			continue;
		}

		//Wait on acknowledge package and check
		uint8_t acknowledge = NACK;
		while(acknowledge != ACK)
		{
			//wait on receive buffer to be filled
			wait_on_answer(&send_array); //NACK berücksichtigen!!!!!!

			//get received information
			get_received_data(&header, &data, &flags, &submittedCRC);

			//check received CRC
			if(check_crc(submittedCRC, RecvBuffer, lastCRC_rcvd)!= XST_SUCCESS)
			{
				send_NACK();
			}
			else
			{
				acknowledge = ACK;
			}
		}

		//Received CRC is correct
		//check ACK
		if(get_ACK_flag(&flags) != SET)
		{
			/* CRC is correct, NACK */
			continue;
		}

		*lastCRC_send = send_array[CRC_POS];
		package_counter++;
	}

	return XST_SUCCESS;
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
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes[], uint8_t *temp, int packageCount)
{
	/*Temporary arrays for header and data*/
	//uint8_t header[4];

	uint8_t header[HEADER_SIZE] = {ID, INIT_CRC, 0, UNSET_ALL_FLAGS};

	uint8_t *flags;

	for (int i = 0; i < packageCount; i++)
	{
		/*first package*/
		if(i = 0)
		{
			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = PACKAGE_DATA_SIZE;

			/*
			 * fill header with the given information
			 * flags for the start package
			 */
			set_Start_Flag(*flags, SET);

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[k] = header[k];
			}

			for (int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				/*fill temporary arrays temp*/
				temp[j] = *databytes[j - HEADER_SIZE];
			}
		}

		/*all packages except the first and the last one*/
		else if (i > 0 && i != packageCount - 1)
		{
			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = PACKAGE_DATA_SIZE;

			/*
			 * fill header with the given information*/
			/* flags for the middle packages
			 */
			//flags = 0b00000000; //anpassen, ACK flag ist gesetzt!!!
			set_Start_Flag(&header[FLAGS_POS], NOT_SET);

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[i * BUFFER_SIZE + k] = header[k];
			}

			/*fill temporary arrays temp and temp28*/
			for (int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				temp[i * BUFFER_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j - HEADER_SIZE];
			}
		}

		/*last package*/
		else
		{
			int restsize = dataLength - PACKAGE_DATA_SIZE * (packageCount - 1);

			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = restsize;

			/*fill header with the given information*/
			/*flags for the end package*/
			set_End_Flag(*flags, SET);

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[i * BUFFER_SIZE + k] = header[k];
			}


			for(int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				/*fill temp and temp28*/
				if(j < restsize)
				{
					/*fill with the rest databytes from position 0 to restsize*/
					temp[i * BUFFER_SIZE + j] = *databytes[i * PACKAGE_DATA_SIZE + j - HEADER_SIZE];
				}
				else
				{
					/*fill with 0 from position restsize to 28*/
					temp[i * BUFFER_SIZE + j] = 0;
				}
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
