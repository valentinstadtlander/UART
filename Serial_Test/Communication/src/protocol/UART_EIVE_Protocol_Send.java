package protocol;

import java.util.ArrayList;

import protocol.crc.CRC;
import protocol.flags.UART_EIVE_Protocol_Flags;
import serialDriver.SerialDriver;

public class UART_EIVE_Protocol_Send 
{
	//DEBUG
	boolean network = false;


	private SerialDriver comm;
	
	public UART_EIVE_Protocol_Send(SerialDriver comm)
	{
		this.comm = comm;
	}
	
	/*
	 * lastCRC_send saves the calculated CRC for the last send package
	 * submittedCRC always saves the new received CRC
	 * lastCRC_rcv saves the last received CRC for the initval for checking the received package
	 */
	byte lastCRC_send = CRC.INIT_CRC, lastCRC_rcvd = CRC.INIT_CRC, submittedCRC = CRC.INIT_CRC;
	
	
	byte snd_flags = UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS;
	byte rcv_flags = UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS;
	

	byte[] temp32 = new byte[Constants.BUFFER_SIZE];
	
	static byte[] header = new byte[Constants.HEADER_SIZE];
	
	byte[] data = new byte[Constants.PACKAGE_DATA_SIZE];

	
/*
 * Main Method of the EIVE UART Protocol to send data
 *
 * @param:	ID		Identification number of the data to send
 * @param:	*databytes	Pointer to the data which is going to be send
 * @param:	dataLength	Length of the data which is going to be send
 *
 * @return:	XST_SUCCES	If the data was send properly
 * @return:	XST_FAILURE	If the data was not send properly
 *
 * This method uses the connection_establishment() -Method to establish a connection between the sender and the receiver.
 * It also uses the send_data() -method to send the transfered data to the receiver after a connection was established.
 * It returns an error if the data could not to be send and a success, if the transmission was possible
 */
public int UART_Send_Data(byte ID, byte databytes[], int dataLength)
{

	int status;

	//connection establishment
	status = connect_(ID);

	System.out.println("connect");
	//return Failure if connection could not be established
	if(status == Constants.XST_FAILURE)
		return Constants.XST_FAILURE;

	//send data
	System.out.println("before send_data");
	status = send_data(ID, databytes, dataLength);
	System.out.println("after send_data, returnValue: " + status);


	//return failure if the data could not be send
	if(status == Constants.XST_FAILURE)
		return Constants.XST_FAILURE;


	return Constants.XST_SUCCESS;

}

/*
 * Method to establish a connection to the receiver
 *
 * @param:	ID				Identification number of the package to send
 * @param:	*databytes		Pointer to the array of data which are going to be send
 * @param:	dataLength		Length of the data which is going to be send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 * @param:	*lastCRC_rcvd	Pointer to the last received CRC value
 *
 * @return:	XST_SUCCES	If the connection was established properly
 * @return:	XST_FAILURE	If the connection was not established properly
 */
int connect_(byte ID)
{

	//int packageCount = package_count(dataLength);
	//uint8_t temp[BUFFER_SIZE * packageCount];
	//byte[] temp32 = new byte[Constants.BUFFER_SIZE];
	
	//byte[] header = new byte[Constants.HEADER_SIZE];
	
	//byte[] data = new byte[Constants.PACKAGE_DATA_SIZE];

	//byte submittedCRC = CRC.INIT_CRC;

	int status;

	int connection = Constants.NACK;
	int conn_counter = 0;

	while(connection != UART_EIVE_Protocol_Flags.ACK && conn_counter < 10)
	{
		System.out.println("before req2send");
		//Request to send, CRC initval = 0x00
		lastCRC_send = CRC.INIT_CRC;
		status = send_request_to_send(ID, temp32);

		System.out.println("nach req2send");

		//check status of sending
		if(status != Constants.XST_SUCCESS)
			return Constants.XST_FAILURE;

		int acknowledge = Constants.NACK; //for CRC
		int succes = 1;

		while(acknowledge != Constants.ACK)
		{
			if(succes == 1)
			{
				System.out.println("Wait on answer a");
				//wait on answer sending again temp32
				System.out.println("Value: " + wait_on_answer(temp32, ID, lastCRC_send));
			}
			else
			{
				System.out.println("Wait on answer b");
				//wait on answer sending again NACK
				System.out.println("Value: " + wait_on_answer(null, ID, lastCRC_send));
			}

			System.out.println("connect: get_received_data");
			//fill header, data, receive flags and submittedCRC with the received values
			get_received_data();

			//check received CRC
			System.out.println("Check crc in connect");
			System.out.println("Next crc check: Initval -> Last_CRC_rcvd: " + ((int)lastCRC_rcvd&0xff));
			if(CRC.check_crc(submittedCRC, Constants.RecvBuffer, lastCRC_rcvd)!= Constants.XST_SUCCESS)
			{
				//CRC values defeer, send failure
				System.out.println("CRC FAILURE in connect_");
				int ret = send_failure(lastCRC_send, ID);
				if(ret > 0)
					lastCRC_send = (byte) ret;
			}
			else
			{
				System.out.println("CRC SUCCESS in connect_");
				acknowledge = Constants.ACK;

				//wird in check_crc übernommen
				lastCRC_rcvd = submittedCRC; //Test
			}
		}

		//set ACK = 1
		System.out.println("Set ack flag in connect");
		snd_flags = UART_EIVE_Protocol_Flags.set_ACK_Flag(snd_flags, Constants.ACK);

		//Received CRC is correct
		//check ACK

		System.out.println("get ack flag: " + ((int) rcv_flags&0xff));
		if( UART_EIVE_Protocol_Flags.get_ACK_flag(rcv_flags) == Constants.SET)
		{
			//check ready to receive
			System.out.println("get ready to rcv flag\n");
				if( UART_EIVE_Protocol_Flags.get_ready_to_recv_flag(rcv_flags) == Constants.SET)
				{
					//send_data();
					connection = Constants.ACK;
					lastCRC_rcvd = submittedCRC; //Test
					System.out.println("R2R Flag set!");
				}
				else
				{
					//NOT ready to receive
					System.out.println("FAILURE r2r flag");
					conn_counter++;
				}
		}
		else
			System.out.println("FAILURE no ack flag set");
		
		//DEBUG
		//break;
	}

	System.out.println("return at connect: conn_counter: "+ conn_counter);
	if(conn_counter == 10)
		return Constants.XST_FAILURE;


	return Constants.XST_SUCCESS;
}


/*
 *Request to send, to establish a connection
 *
 *@param:	ID			Identification number of the package to send
 *@param:	*lastCRC	Pointer, last CRC value to save the new CRC value for the next package
 *
 *Configures a package to send a request to send and saves the first CRC
 */
int send_request_to_send(byte ID, byte temp32[])
{
	System.out.println("asd");

	if(snd_flags == 0x00)
	{
		snd_flags = UART_EIVE_Protocol_Flags.REQ_TO_SEND_MASK;
		System.out.println("flags");
	}

	System.out.println("ID_POS"); //try puts
	temp32[Constants.ID_POS] = ID;

	System.out.println("DATA_SIZE_POS");
	temp32[Constants.DATA_SIZE_POS] = 0;

	System.out.println("FLAG_POS");
	temp32[Constants.FLAGS_POS] = snd_flags;


	System.out.println("Test: " + lastCRC_send);//, temp32[0]);
	System.out.println("Test2: " + temp32[0]);

	System.out.println("Next crc calc: Initval -> Last_CRC_send: " + lastCRC_send);
	int crc = CRC.calc_crc8(temp32, lastCRC_send);

	lastCRC_send = (byte) crc; //Kann auf nach gesetztem ACK-Flag verschoben werden.

	System.out.println("nach crc");

	temp32[Constants.CRC_POS] = lastCRC_send;
	//(*lastCRC_send) = newCRC;

	int status = Constants.XST_FAILURE;
	int tries = 0;

	System.out.println("vor while");
	while(status != Constants.XST_SUCCESS)
	{
		// -> Network now
		//status = UART_Send(temp32, 1);
		System.out.println("SEND DATA!!!!!\n");
		status = Constants.XST_SUCCESS;
		
		/*
		 * Method from driver for serial communication
		 *
		 */
		if(!comm.writeToPort(temp32))
		//if(send(sock, temp32, Constants.BUFFER_SIZE, 0) != Constants.BUFFER_SIZE)
		{
			status = Constants.XST_FAILURE;
			System.out.println("ERROR sendreq2send");
		}

		System.out.println("while");
		if(tries == 50)
		{
			System.out.println("try == 50\n");
			return Constants.XST_FAILURE;
		}

		tries++;
	}

	return Constants.XST_SUCCESS;
}

/*
 * Package counter
 *
 * @param:	dataLength	number of bytes of the data to send
 *
 * returns the number of the needed packages to send all the databytes
 */
private int package_count(int dataLength)
{
	int ret = 0;
	System.out.println("DataLength: " + dataLength + " ------- " + (ret = dataLength/Constants.PACKAGE_DATA_SIZE));

	if (dataLength % Constants.PACKAGE_DATA_SIZE > 0)
		return (dataLength / Constants.PACKAGE_DATA_SIZE + 1);
	else
		return ret; //(dataLength / PACKAGE_DATA_SIZE);
}

/*
 * Method to save the submitted header, data, flags and CRC from the receiver
 *
 * @param:	*header			Pointer to an array of the size of HEADER_SIZE to save the received header
 * @param:	*data			Pointer to an array of the size of PACKAGE_DATA_SIZE to save the received data
 * @param:	*flags			Pointer, to save the received Flags
 * @param: 	*submittedCRC	Pointer, to save the submitted CRC value
 *
 * This method stores in the delivered parameters the received information
 */
private void get_received_data()//, uint8_t *flags, uint8_t *submittedCRC)
{
	System.out.println("getReceivedData");
	ArrayList<byte[]> ret = UART_EIVE_Protocol_Recv.extract_header();
	
	header = ret.get(0);
	data = ret.get(1);
	
	rcv_flags = header[Constants.FLAGS_POS];
	submittedCRC = header[Constants.CRC_POS];
}

/*
 * Method to send the data
 *
 * @param:	ID				Identification number of the package to send
 * @param:	*databytes		Pointer to the array of data which are going to be send
 * @param:	dataLength		Length of the data which is going to be send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 * @param:	*lastCRC_rcvd	Pointer to the last received CRC value
 *
 * @return:	XST_SUCCES	If the data was send properly
 * @return:	XST_FAILURE	If the data was not send properly
 */
private int send_data(byte ID, byte databytes[], int dataLength)
{
	byte send_array[] = new byte[Constants.BUFFER_SIZE];
	int packageCount = package_count(dataLength);
	//byte header[] = new byte[Constants.HEADER_SIZE];
	//byte data[] = new byte[Constants.PACKAGE_DATA_SIZE];
	byte temp[] = new byte[Constants.BUFFER_SIZE * packageCount];
	int crc = 0;
	int status = Constants.XST_SUCCESS;

	System.out.println("fill packages: " + packageCount);
	//fill array temp with the databytes and the header to send
	fill_packages(ID, dataLength, databytes, temp, packageCount);

	int package_counter = 0;
	int tries = 0;

	while(package_counter < packageCount && tries <= 10)
	{
		System.out.println("while package_counter < packageCount: " + packageCount);
		//Get packages
		for(int i = 0; i < Constants.BUFFER_SIZE; i++)
			send_array[i] = temp[package_counter * Constants.BUFFER_SIZE + i];

		System.out.println("setAckflag");
		//Set acknowledge flag
		send_array[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.set_ACK_Flag(send_array[Constants.FLAGS_POS], UART_EIVE_Protocol_Flags.ACK);

		System.out.println("Flags in package: " + Integer.toBinaryString((int)send_array[Constants.FLAGS_POS]&0xff));
		System.out.println("before crc in while");
		System.out.println("Next crc calc: Initval -> Last_CRC_send: " + ((int)lastCRC_send&0xff));
		//Calculate CRC value
		crc = CRC.calc_crc8(send_array, lastCRC_send);

		send_array[Constants.CRC_POS] = (byte) crc;

		System.out.println("after crc in while");
		
		//Send package -> Network now
		//status = UART_Send(send_array, 1);
		//if(send(sock, send_array, Constants.BUFFER_SIZE, 0) != Constants.BUFFER_SIZE)
		/*
		 * Method from driver for serial communication
		 *
		 */
		if(!comm.writeToPort(send_array))
		{
			System.out.println("EEROR");
			status = Constants.XST_FAILURE;
		}
		else
			status = Constants.XST_SUCCESS;

			
		System.out.println("after send in while");
		if(status != Constants.XST_SUCCESS)
		{
			tries++;
			continue;
		}

		//Wait on acknowledge package and check
		byte acknowledge = Constants.NACK;
		int succes = 1;

		while(acknowledge != Constants.ACK)
		{
			System.out.println("wait on ack in send_data");
			//wait on receive buffer to be filled
			if(succes == 1)
			{
				System.out.println("SendArray");
				wait_on_answer(send_array, send_array[Constants.ID_POS], send_array[Constants.CRC_POS]);
			}
			else
			{
				//wait_on_answer with NACK
				wait_on_answer(null, ID, lastCRC_send);
				System.out.println("Send NACK-Package");
			}

			//get received information
			get_received_data();

			System.out.println("Next crc check: Initval -> Last_CRC_rcvd: " + ((int)lastCRC_rcvd&0xff));
			//check received CRC
			if(CRC.check_crc(submittedCRC, Constants.RecvBuffer, lastCRC_rcvd)!= Constants.XST_SUCCESS)
			{
				System.out.println("CRC Failure on answer");
				send_failure(lastCRC_send, ID);
				
				succes = 0;
			}
			else
			{
				acknowledge = Constants.ACK;

				//wird von check_crc übernommen
				lastCRC_rcvd = submittedCRC; //Test
			}
		}

		System.out.println("crc correct and get ack\n");
		//Received CRC is correct
		//check ACK
		if(UART_EIVE_Protocol_Flags.get_ACK_flag(rcv_flags) != UART_EIVE_Protocol_Flags.SET)
		{
			/* CRC is correct, NACK */
			System.out.println("CRC corr, received NACK");
			continue;
		}

		System.out.println("get last crc sent \n");
		lastCRC_send = send_array[Constants.CRC_POS];

		package_counter++;
	}

	if(tries == 10)
		return Constants.XST_FAILURE;

	return Constants.XST_SUCCESS;
}

/*
 * Method to wait on an answer of the receiver
 *
 * @param:	*send_array:	pointer to the array which is going to be send again if the timer expires and the RecvBuffer does not get filled
 *							NULL if the send_array is NACK
 * @param:	ID				Identification number of the package to send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 *
 * @return:	XST_SUCCES		If an answer was received
 * @return:	XST_FAILURE		If no answer was received
 */
int wait_on_answer(byte send_array[], byte ID, byte lastCRC_send)
{
	//byte[] nack_header = new byte[Constants.BUFFER_SIZE];

	if(null == send_array)
	{
		lastCRC_send = fill_header_for_empty_data(ID, UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS, lastCRC_send);
	}

	int status = Constants.XST_NO_DATA;
	int timer = 0;

	while(status != Constants.XST_SUCCESS)
	{
		System.out.println("Wait for answer: while");
		// -> Network now
		//status = UART_Recv_Buffer();
		//if(recv(sock, Constants.RecvBuffer, Constants.BUFFER_SIZE, 0) < 0)
			//System.out.println("ERROR waitOnAnswer1");

		/*
		 * Method from driver for serial communication
		 *
		 */

		while(!comm.isSerialDataAvailable());
		
		Constants.RecvBuffer = comm.getRecvBuffer();
				
		status = Constants.XST_SUCCESS; //test

		System.out.println("Answer received!!: \n");
		if(status != Constants.XST_NO_DATA && status != Constants.XST_SUCCESS)
			return Constants.XST_FAILURE;

		timer++;

		if(timer == Constants.MAX_TIMER)
		{
			System.out.println("Timer is max timer");
			//Timeout
			//send again array to send
			if(null == send_array)
			{
				
				byte temp[] = new byte[Constants.BUFFER_SIZE];
				temp[Constants.ID_POS]= header[Constants.ID_POS];
				temp[Constants.CRC_POS] = header[Constants.CRC_POS];
				temp[Constants.DATA_SIZE_POS] = header[Constants.DATA_SIZE_POS];
				temp[Constants.FLAGS_POS] = header[Constants.FLAGS_POS];
				
				//UART_Send(temp, 1);
				
				// -> Network now
				
					//if(send(sock, temp, Constants.BUFFER_SIZE, 0) != Constants.BUFFER_SIZE)
						//System.out.println("ERROR waitOnAnswer2");
				
				/*
				 * Method from driver for serial communication
				 *
				 */
				if(!comm.writeToPort(temp))
					System.out.println("ERROR waitOnAnswer2");
			}
			else
			{
				// -> Network now
				//UART_EIVE_Protocol_Send.UART_Send(send_array, 1);	
				
				/*
				 * Method from driver for serial communication
				 *
				 */
				
				if(!comm.writeToPort(send_array))
					System.out.println("ERROR waitOnAnswer3");
				
				//if(send(sock, send_array, Constants.BUFFER_SIZE, 0) != Constants.BUFFER_SIZE)
					//System.out.println("ERROR waitOnAnswer3");
					
					
				
			}

			//reset timer
			timer = 0;
		}
	}
	return Constants.XST_SUCCESS;
}


/*
 * Method to fill the packages to send
 *
 * @param:	ID 				Identification number of the package to send
 * @param: 	dataLength		length of the data to send, must be given by the user
 * @param:	*databytes		Pointer to the data to send
 * @param:	temp			Pointer to the temporary array in the main method with the length of BUFFER_SIZE * packageCount,
 * 							which is filled with the header and the databytes
 * @param: 	packageCount	numbers of packages needed to send all the databytes
 *
 * Fills the submitted variable temp with the databytes and the headers
 */
void fill_packages(byte ID, int dataLength, byte[] databytes, byte[] temp, int packageCount)
{

	/*Temporary arrays for header and data*/
	//uint8_t header[4];

	System.out.println("Fill " + packageCount + " packages with: ");
	
	for(byte b: databytes)
		System.out.print((char) b);
	System.out.println();
	
	//byte[] header = new byte[Constants.HEADER_SIZE];
	header[Constants.ID_POS] = ID;
	header[Constants.CRC_POS] = CRC.INIT_CRC;
	header[Constants.DATA_SIZE_POS] = 0;
	header[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS;

	//uint8_t flags = UNSET_ALL_FLAGS;

	for (int i = 0; i < packageCount; i++)
	{

		/*first package*/
		if(i == 0)
		{
			System.out.println("fill header first pkg");
			
			//Fill header[DATA_SIZE_POS]
			if(databytes.length > Constants.PACKAGE_DATA_SIZE - 1)
				header[Constants.DATA_SIZE_POS] = Constants.PACKAGE_DATA_SIZE;
			else
				header[Constants.DATA_SIZE_POS] = (byte) databytes.length;
			
			System.out.println("Set start flag");
			/*
			 * fill header with the given information
			 * flags for the start package
			 */
			header[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.set_Start_Flag(header[Constants.FLAGS_POS], UART_EIVE_Protocol_Flags.SET);

			//Setting end-flag if only one package will be send
			if(packageCount == 1)
			{
				System.out.println("Before end flag");
				header[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.set_End_Flag(header[Constants.FLAGS_POS], UART_EIVE_Protocol_Flags.SET);
				
			}

			System.out.println("Start flag set");

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < Constants.HEADER_SIZE; k++)
			{
				temp[k] = header[k];
			}

			System.out.println("Chars in this package: ");
			for (int j = Constants.HEADER_SIZE; j < Constants.BUFFER_SIZE; j++)
			{
				if(j - Constants.HEADER_SIZE < dataLength)
					/*fill temporary arrays temp*/
					temp[j] = databytes[j - Constants.HEADER_SIZE];
				else
					/*fill up with zero*/
					temp[j] = 0;
				System.out.println((char) temp[j]);				
			}
			//System.out.println("\n");
		}

		/*all packages except the first and the last one*/
		else if (i > 0 && i != packageCount - 1)
		{
			System.out.println("fill header " + (i+1) + ". pkg");

			//Fill header[DATA_SIZE_POS]
			header[Constants.DATA_SIZE_POS] = Constants.PACKAGE_DATA_SIZE;

			/*
			 * fill header with the given information*/
			/* flags for the middle packages
			 */
			//flags = 0b00000000; //anpassen, ACK flag ist gesetzt!!!
			header[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.set_Start_Flag(header[Constants.FLAGS_POS], UART_EIVE_Protocol_Flags.NOT_SET);


			/*fill temporary array temp with the headers*/
			for (int k = 0; k < Constants.HEADER_SIZE; k++)
			{
				temp[i * Constants.BUFFER_SIZE + k] = header[k];
			}

			System.out.println("Chars in this package: ");
			/*fill temporary arrays temp and temp28*/
			for (int j = Constants.HEADER_SIZE; j < Constants.BUFFER_SIZE; j++)
			{
				temp[i * Constants.BUFFER_SIZE + j] = databytes[i * Constants.PACKAGE_DATA_SIZE + j - Constants.HEADER_SIZE];
				System.out.println((char) temp[i*Constants.BUFFER_SIZE + j]);
			}
			//System.out.println();
		}

		/*last package*/
		else
		{
			int restsize = dataLength - Constants.PACKAGE_DATA_SIZE * (packageCount - 1);

			System.out.println("fill header last pkg, restsize: " + restsize);

			//Fill header[DATA_SIZE_POS]
			header[Constants.DATA_SIZE_POS] = (byte) restsize;

			/*fill header with the given information*/
			/*flags for the end package*/
			header[Constants.FLAGS_POS] = UART_EIVE_Protocol_Flags.set_End_Flag(header[Constants.FLAGS_POS], UART_EIVE_Protocol_Flags.SET);

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < Constants.HEADER_SIZE; k++)
			{
				temp[i * Constants.BUFFER_SIZE + k] = header[k];
			}


			System.out.println("chars in this package: ");
			for(int j = Constants.HEADER_SIZE; j < Constants.BUFFER_SIZE; j++)
			{
				/*fill temp and temp28*/
				if(j - Constants.HEADER_SIZE < restsize)
				{
					/*fill with the rest databytes from position 0 to restsize*/
					temp[i * Constants.BUFFER_SIZE + j] = databytes[i * Constants.PACKAGE_DATA_SIZE + j - Constants.HEADER_SIZE];
					System.out.println((char) temp[i*Constants.BUFFER_SIZE + j]);
				}
				else
				{
					/*fill with 0 from position restsize to 28*/
					temp[i * Constants.BUFFER_SIZE + j] = 0;
				}
			}
			//System.out.println("\n");


		}

		System.out.println("Package no. " + i + ": data size " + header[Constants.DATA_SIZE_POS]);
	}

}

/*
 * Fill Header with submitted parameters
 *
 * @param:	*header			Pointer to store the header
 * @param:	ID 				Identification number of the package to send
 * @param: 	flags			Flags of the package which is going to be send
 * @param:	*lastCRC_send	Pointer to the last calculated CRC of the last send package
 *
 * This method fills the header of empty packages which are going to be send
 */
public static byte fill_header_for_empty_data(byte ID, byte flags, byte lastCRC_send)
{
	System.out.println("fill header for empty dataaa");
	byte[] temp_array_CRC = new byte[Constants.BUFFER_SIZE];
	temp_array_CRC[Constants.ID_POS] = ID;
	temp_array_CRC[Constants.FLAGS_POS] = flags;

	System.out.println("Next crc calc: Initval -> Last_CRC_send: " + ((int)lastCRC_send&0xff));
	/*calculate new CRC value*/
	byte newCRC = CRC.calc_crc8(temp_array_CRC, lastCRC_send);

	/*save new CRC value in old variable*/
	//(*lastCRC_send) = newCRC;

	/*fill header*/
	header[Constants.ID_POS] = ID;
	header[Constants.CRC_POS] = newCRC;
	header[Constants.DATA_SIZE_POS] = Constants.EMPTY_DATA_LENGTH;
	header[Constants.FLAGS_POS] = flags;

	return newCRC;
}


private int send_failure(byte lastCRC_send, byte iD) {

	byte failure_flags = UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS;
	
	//byte header[] = new byte[Constants.HEADER_SIZE];
	
	UART_EIVE_Protocol_Flags.set_ACK_Flag(failure_flags, Constants.NACK);
	
	int calc_crc = fill_header_for_empty_data(header[Constants.ID_POS], header[Constants.FLAGS_POS], lastCRC_send);
	
	
	byte temp[] = new byte[Constants.BUFFER_SIZE];
	temp[Constants.ID_POS] = header[Constants.ID_POS];
	temp[Constants.CRC_POS] = header[Constants.CRC_POS];
	temp[Constants.DATA_SIZE_POS] = header[Constants.DATA_SIZE_POS];
	temp[Constants.FLAGS_POS] = header[Constants.FLAGS_POS];
	
	//send method
	/*
	 * Method from driver for serial communication
	 *
	 */
	
	comm.writeToPort(temp);

	this.lastCRC_send = (byte) calc_crc;
	
	return Constants.XST_SUCCESS;
}

}
