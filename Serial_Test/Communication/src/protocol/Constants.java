package protocol;

public class Constants {

	//XST statics
	public static final int XST_SUCCESS = 0;
	public static final int XST_NO_DATA = 13;
	public static final int XST_FAILURE = -1;
	
	//Header, Data and Package size
	public static final int HEADER_SIZE = 4;
	public static final int PACKAGE_DATA_SIZE = 28;
	public static final int BUFFER_SIZE = 32;
	
	public static final int EMPTY_DATA_LENGTH = 0;
	
	//Header positions
	public static final int ID_POS = 0;
	public static final int CRC_POS = 1;
	public static final int DATA_SIZE_POS = 2;
	public static final int FLAGS_POS = 3;
	
	//Receive Buffer
	public static byte RecvBuffer[] = new byte[BUFFER_SIZE];
	
	//ACK , NACK
	public static final int ACK = 1;
	public static final int NACK = 0;
	public static final int SET = 1;
	
	public static final byte INIT_CRC = 0x00;
	
	public static final int MAX_TIMER = 5000000; //need to be calculcated!!
	
	/*Identification mask for TM/TC */
	public static final byte TC_MASK = 0b00000000;
	public static final byte TM_MASK = (byte) 0b11110000;

	/*Identification numbers for TM/TC */
	public static final byte CAMERA_TC = 0b00000000;
	public static final byte CAMERA_TM = (byte) 0b11110000;

	public static final byte UART_TC = 0b00001010;
	public static final byte UART_TM = (byte) 0b11111010;

	public static final byte BRAM_TC = 0b00000101;
	public static final byte BRAM_TM = (byte) 0b11110101;

	public static final byte CPU_TC = 0b00001111;
	public static final byte CPU_TM = (byte) 0b11111111;

	public static final byte DOWNLINK_TC = 0b00001001;
	public static final byte DOWNLINK_TM = (byte) 0b11111001;

	public static final byte DAC_TC = 0b00000110;
	public static final byte DAC_TM = (byte) 0b11110110;

	
}
