package protocol.crc;

import protocol.Constants;

public class CRC {

	/*
	 * Generator polynom for cyclic redundancy check
	 *
	 *
	 */
	public static final byte CRC_GENERATOR = 0x1D;

	/**
	 *
	 * Initial CRC value for calculating first crc
	 *
	 *
	 */
	public static final byte INIT_CRC = 0x00;

	/*
	 * CRC calculation function for one byte
	 *
	 * start_crc: start value for crc calculation byte: byte for crc calculation
	 *
	 * returns the calculated 8-bit crc value for the given byte
	 */
public static byte calc_crc8_for_one_byte(byte start_crc, byte byteForCRC)
{
	byte crc = (byte) (start_crc ^ byteForCRC);
	for(int i = 0; i < 8; i++)
	{
		if((crc & 0x80) != 0)
		{
			crc = (byte) ((crc<<1) ^ CRC_GENERATOR);
		} else
		{
			crc <<= 1;
		}
	}

	return crc;
}

	/*
	 * CRC calculation function for a message more than one byte
	 *
	 * bytes[]: message with bytes for crc calculation length: length of the byte
	 * array crc_initval: the start value for calculating crc of the data
	 *
	 * returns the calculated 8-bit crc value for the whole message
	 */
public static byte calc_crc8_for_data(byte bytes[], int length, byte crc_initval)
{

	byte crc_val = crc_initval;
	for(int i = 0; i < length; i++)
	{
		crc_val = calc_crc8_for_one_byte(crc_val, bytes[i]);
	}

	System.out.println("calculated crc: " + ((int)crc_val&0xff));
	return crc_val;
}

public static byte calc_crc8(byte send_array[], byte crc_initval)
{
	System.out.println("crc8: initval: " + ((int)crc_initval&0xff));
	byte temp31[] = new byte[Constants.BUFFER_SIZE - 1];

	for(int i = 0; i < Constants.CRC_POS; i++)
	{
		temp31[i] = send_array[i];
	}

	for(int j = Constants.CRC_POS; j < Constants.BUFFER_SIZE - 1; j++)
	{
		temp31[j] = send_array[j + 1];
	}

	System.out.println("calc crc8");
	return calc_crc8_for_data(temp31, Constants.BUFFER_SIZE - 1, crc_initval);
}

	/*
	 *
	 */
public static int check_crc(byte crc_val, byte rcv_buffer[], byte crc_initval)
{
	byte calculated_crc;

	calculated_crc = calc_crc8(rcv_buffer, crc_initval);

	if(calculated_crc != crc_val)
		return Constants.XST_FAILURE;


	//*crc_initval = calculated_crc;

	return Constants.XST_SUCCESS;
}

}
