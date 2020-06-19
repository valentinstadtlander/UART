package protocol;

import java.util.ArrayList;

import main.MainClassReceiverSide;
import main.MainClassTransmitterSide;
import main.tcs.TCs;
import protocol.Constants;
import protocol.crc.CRC;
import protocol.flags.UART_EIVE_Protocol_Flags;
import serialDriver.SerialDriver;

public class UART_EIVE_Protocol_Recv {
	
	
	private SerialDriver comm;
	
	public UART_EIVE_Protocol_Recv(SerialDriver comm)
	{
		this.comm = comm;
	}
	
	
	//CRC
	public byte last_crc_send = Constants.INIT_CRC;
	public byte last_crc_rcv = Constants.INIT_CRC;
	public byte calc_crc = Constants.INIT_CRC;
	
	//flags
	public byte new_flags = 0x00;
	
	//connection ID
	public byte conn_id = 0x00;
	
	//Long buffer for receiving data
	public static byte databuffer[] = new byte[573483];
	
	public static byte header[] = new byte[Constants.HEADER_SIZE];
	public byte data[] = new byte[Constants.PACKAGE_DATA_SIZE];

	public int UART_Recv_Data() {
		
		int status = Constants.XST_SUCCESS;
		
		/*
		 * Method from driver for serial communication
		 *
		 */
		
		if(!comm.isSerialDataAvailable())
			return Constants.XST_NO_DATA;
		
		//if((status = recv_data) //empfangsmethode anpassen, define method
				//System.out.println("Error receiving data!!");
		/*
		 * Method from driver for serial communication
		 *
		 */
		Constants.RecvBuffer = comm.getRecvBuffer();
		status = Constants.XST_SUCCESS;
		
		if(status == Constants.XST_FAILURE)
			return Constants.XST_FAILURE;
		
		status = receive();
		
		System.out.println("Status UART_Recv_Data() " + status);
		
		if(status != Constants.XST_SUCCESS)
			return Constants.XST_FAILURE;
		
		return Constants.XST_SUCCESS;
	}
	
	public int receive() {
		int status = Constants.XST_SUCCESS;
		
		//connection establishment
		status = connection_establishment();
		
		if(status == Constants.XST_FAILURE)
			return Constants.XST_FAILURE;
		
		//receive the TM/TCs
		status = receive_data(new_flags); //define method!!
		
		System.out.println("Status Receive() " + status);
		if(status == Constants.XST_FAILURE)
			return Constants.XST_FAILURE;
		
		return Constants.XST_SUCCESS;
	}
	
	public int connection_establishment() {
		byte header[] = new byte[Constants.HEADER_SIZE];
		
		//byte data[] = new byte[Constants.PACKAGE_DATA_SIZE];
		
		ArrayList<byte[]> retL = extract_header(); //define method!!
		
		header = retL.get(0);
		
		conn_id = header[Constants.ID_POS];
		
		if(CRC.check_crc(header[Constants.CRC_POS], Constants.RecvBuffer, Constants.INIT_CRC) != Constants.XST_SUCCESS) {
			//define method check_crc
			int ret = send_failure(header[Constants.ID_POS]); //define method
			
			if(ret > 0)
				last_crc_send = (byte) ret;
			
			return Constants.XST_FAILURE;
		}
		System.out.println("Writing flags: " + ((int)new_flags&0xff));
		new_flags = UART_EIVE_Protocol_Flags.set_ACK_Flag(new_flags, Constants.SET); //define method
		System.out.println("Writing flags: " + ((int)new_flags&0xff));

		//check request to send
		if(UART_EIVE_Protocol_Flags.get_Req_to_send_flag(header[Constants.FLAGS_POS]) == 0) {
			//define method
			//Send answer without set ACK flag
			send_failure(header[Constants.ID_POS]); //define method
			
			return Constants.XST_FAILURE;
		}
		
		System.out.println("Writing flags: " + ((int)new_flags&0xff));
		new_flags = UART_EIVE_Protocol_Flags.set_Rdy_to_rcv_Flag(new_flags, Constants.SET); //define method
		System.out.println("Writing flags: " + ((int)new_flags&0xff));
		
		int status = 0;
		
		last_crc_rcv = header[Constants.CRC_POS];
		
		/*
		 * Method from driver for serial communication
		 *
		 */
		//System.out.println("Writing flags: " + ((int)new_flags&0xff));
		send_success(header[Constants.ID_POS], new_flags);
		
		System.out.println("----> Last crc send: " + ((int) last_crc_send&0xff));
		
		return status;
	}
	
	public int receive_data(byte last_sent_flags) {
		//byte next_header[] = new byte[Constants.HEADER_SIZE];
		//byte new_data[] = new byte[Constants.PACKAGE_DATA_SIZE];
		
		byte flags_to_send = last_sent_flags;
		
		int datacounter = 0;
		int pkgCounter = 0;
		int end = 0;
		int status = Constants.XST_NO_DATA;
		int timer = 1;
		int success = Constants.SET;
		
		while(end != Constants.SET) {
			//receiving answer
			while(status == Constants.XST_NO_DATA) {
				//timeout for receiving, reset timer for new sending
				if(timer == Constants.MAX_TIMER) {
					//timer = 0; DEBUG
					System.err.println("MAX Timer is reached!");
				}
				if(timer == 0) {
					if(success == Constants.SET) {
						System.out.println("Send success...");
						send_success(conn_id, flags_to_send); //define method
					}
					else {
						System.out.println("Send failure...");
						int ret = send_failure(header[Constants.ID_POS]); //define method
						System.out.println("RET: " + ret);
						if(ret > 0)
							last_crc_send = (byte) ret;
					}
				}
				
				
				//increase timer
				timer++;
				
				//check status of receiving
				//if(recv) //method for receiving
					//return Constants.XST_FAILURE;	
				
				/*
				 * Method from driver for serial communication
				 *
				 */
				while(!comm.isSerialDataAvailable());
				Constants.RecvBuffer = comm.getRecvBuffer();
				status = Constants.XST_SUCCESS;
				//return 0; //DEBUG
			}
			//data received
			
			flags_to_send = 0x00;
			ArrayList<byte[]> ret = extract_header(); //define method
			
			header = ret.get(0);
			data = ret.get(1);
			
			System.out.println("CRC check");
			if(CRC.check_crc(header[Constants.CRC_POS], Constants.RecvBuffer, last_crc_rcv) != Constants.XST_SUCCESS) {
				//failure
				System.out.println("Failure CRC");
				success = 0;
				timer = 0;
				status = Constants.XST_NO_DATA;
				continue;
			}
			
			flags_to_send = UART_EIVE_Protocol_Flags.set_ACK_Flag(flags_to_send, Constants.ACK); //define method
			
			if(UART_EIVE_Protocol_Flags.get_ACK_flag(header[Constants.FLAGS_POS]) != Constants.ACK) {
				//failure
				System.out.println("Failure ACK");
				timer = 0;
				status = Constants.XST_NO_DATA;
				continue;
			}
			
			System.out.println("----> Calc crc: " + ((int) calc_crc&0xff));
			//last_crc_send = calc_crc;
			last_crc_rcv = header[Constants.CRC_POS];
			
			//data buffer
			for(int bytes = 0; bytes < header[Constants.DATA_SIZE_POS]; bytes++) {
				databuffer[pkgCounter * Constants.PACKAGE_DATA_SIZE + bytes] = data[bytes];
			}
			datacounter += header[Constants.DATA_SIZE_POS];
			pkgCounter++;
			
			System.out.println("End Flag set? " + ((int) header[Constants.FLAGS_POS]&0xff));
			if(UART_EIVE_Protocol_Flags.get_end_flag(header[Constants.FLAGS_POS]) == 1) {
				end = 1;
				send_success(header[Constants.ID_POS], flags_to_send); //define method
			}
			timer = 0;
			status = Constants.XST_NO_DATA;
			success = 1;
		}
		//check type
		byte id = (byte) (header[Constants.ID_POS] & Constants.TM_MASK);
		
		//data array with exact length
		byte data[] = new byte[datacounter];
		
		//fill data array
		for(int bytes = 0; bytes < datacounter; bytes++)
			data[bytes] = databuffer[bytes];
		
		int size_of_data = data.length;
		
		switch(id) {
			//received data is tc
			case Constants.TC_MASK: recv_TC(header, data, size_of_data); break;
			//received data is tm
			case Constants.TM_MASK: System.out.println("Status Recv_TM: " + recv_TM(data)); break;
			
			default: //default_operation();
		}
		
		System.out.println("Data received!");
		return Constants.XST_SUCCESS;
	}
	
	public static ArrayList<byte[]> extract_header() {
		
		ArrayList<byte[]> ret = new ArrayList<byte[]>();
		byte[] header = new byte[Constants.HEADER_SIZE];
		byte[] data = new byte[Constants.BUFFER_SIZE - Constants.HEADER_SIZE];
		for(int header_pos = 0; header_pos < Constants.HEADER_SIZE; header_pos++)
			header[header_pos] = Constants.RecvBuffer[header_pos];
		
		for(int data_byte = Constants.HEADER_SIZE; data_byte < Constants.BUFFER_SIZE; data_byte++)
			data[data_byte - Constants.HEADER_SIZE] = Constants.RecvBuffer[data_byte];
		
		ret.add(header);
		ret.add(data);
		
		return ret;
		//return Constants.XST_SUCCESS;
	}
	
	public byte send_failure(byte old_id) {
		byte failure_flags = UART_EIVE_Protocol_Flags.UNSET_ALL_FLAGS;
		
		//byte header[] = new byte[Constants.HEADER_SIZE];
		
		UART_EIVE_Protocol_Flags.set_ACK_Flag(failure_flags, Constants.NACK);
		
		byte calc_crc = fill_header_for_empty_data(header[Constants.ID_POS], header[Constants.FLAGS_POS], last_crc_send);
		
		int status = UART_answer(header);
		
		if(status == Constants.XST_SUCCESS)
		{	
			//last_crc_send = (byte) calc_crc;
			return calc_crc;
		}
		else
			return (byte) status;
		
		//return status;
	}
	
	public int send_success(byte id, byte flags) {
		//byte header[] = new byte[Constants.HEADER_SIZE]; 
		
		byte calc_crc = fill_header_for_empty_data(conn_id, flags, last_crc_send);
		System.out.println("Calc CRC in send_success: " + ((int)calc_crc&0xff));
		int status = UART_answer(header);
		
		if(status == Constants.XST_SUCCESS)
			last_crc_send = calc_crc;
		return status;
	}
	
	public int UART_answer(byte header[]) {
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
		
		return Constants.XST_SUCCESS;
	}
	
	int recv_TC(byte[] header, byte[] databytes, int datalength)
	{
		byte id = header[Constants.ID_POS];
		//FILE *fptr;

		switch(id)
		{
			case Constants.CAMERA_TC: if(new String(databytes).equalsIgnoreCase(TCs.tc3))
										{
											System.out.println("Command: Print first text");

											System.out.println("Done!");
											System.out.println(new String(databytes) + "<End>");
											MainClassReceiverSide.sendTM(3, Constants.CAMERA_TM);
											//System.out.println(new String(databytes).equalsIgnoreCase(ProtocolTest.testData));
										}
										break;
			case Constants.UART_TC: 	if(new String(databytes).equalsIgnoreCase(TCs.tc1))
										{
											System.out.println("Command: Print first text");

											System.out.println("Done!");
											System.out.println(new String(databytes) + "<End>");
											MainClassReceiverSide.sendTM(1, Constants.UART_TM);
										//System.out.println(new String(databytes).equalsIgnoreCase(ProtocolTest.testData));
										}
									break;
			case Constants.CPU_TC: break;
			case Constants.BRAM_TC: if(new String(databytes).equalsIgnoreCase(TCs.tc2))
									{
										System.out.println("Command: Print first text");
										System.out.println("Done!");
										System.out.println(new String(databytes) + "<End>");
										MainClassReceiverSide.sendTM(2, Constants.BRAM_TM);
										
										//System.out.println(new String(databytes).equalsIgnoreCase(ProtocolTest.testData));
									} 
									break;
			case Constants.DOWNLINK_TC: if(new String(databytes).equalsIgnoreCase(TCs.tc4))
										{
											System.out.println("Command: Print first text");

											System.out.println("Done!");
											System.out.println(new String(databytes) + "<End>");
											MainClassReceiverSide.sendTM(4, Constants.DOWNLINK_TM);
											//System.out.println(new String(databytes).equalsIgnoreCase(ProtocolTest.testData));
										} 
										break;
			case Constants.DAC_TC: break;
			default: return Constants.XST_FAILURE;
		}

		return Constants.XST_SUCCESS;
	}
	
	private int recv_TM(byte[] databytes)
	{
		String text = MainClassTransmitterSide.getChosenText();
		
		if(new String(databytes).equalsIgnoreCase(text))
		//Not done yet.
			return Constants.XST_SUCCESS;
		else
			return Constants.XST_FAILURE;
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
		System.out.println("fill header for empty dataaa\n");
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

}
