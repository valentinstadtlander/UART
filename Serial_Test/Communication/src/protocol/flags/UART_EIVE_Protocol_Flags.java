package protocol.flags;

public class UART_EIVE_Protocol_Flags {

	/*
	 *
	 * Return value for not set bit
	 *
	 *
	 */
	public static final int NOT_SET = 0;

	/*
	 *
	 * Return value for set bit
	 *
	 *
	 */
	public static final int SET = 1;

	/******************** ACK and NACK ********************/

	/*
	 *
	 * Value for positive acknowledging
	 *
	 *
	 */
	public static final int ACK = 1;

	/*
	 *
	 * Value for negative acknowledging
	 *
	 *
	 */
	public static final int NACK = 0;

	/******************** Masks for the flags (binary) ********************/

	/*
	 *
	 * Mask for the acknowledge bit
	 *
	 *
	 */
	public static final byte ACK_MASK = (byte) 0b10000000;

	/*
	 *
	 * Mask for the request to send bit
	 *
	 *
	 */
	public static final byte REQ_TO_SEND_MASK = (byte) 0b01000000;

	/*
	 *
	 * Mask for the ready to receive bit
	 *
	 *
	 */
	public static final byte READY_TO_RECV_MASK = (byte) 0b00100000;

	/*
	 *
	 * Mask for the start bit
	 *
	 *
	 */
	public static final byte START_MASK = (byte) 0b00010000;

	/*
	 *
	 * Mask for the end bit
	 *
	 *
	 */
	public static final byte END_MASK = (byte) 0b00001000;

	/******************** Masks for the flags (decimal) ********************/

	/*
	 *
	 * Mask for the acknowledge bit
	 *
	 *
	 */
	public static final int ACK_DEC = 128;

	/*
	 *
	 * Mask for the request to send bit
	 *
	 *
	 */
	public static final int REQ_TO_SEND_DEC = 64;

	/*
	 *
	 * Mask for the ready to receive bit
	 *
	 *
	 */
	public static final int READY_TO_RECV_DEC = 32;

	/*
	 *
	 * Mask for the start bit
	 *
	 *
	 */
	public static final int START_DEC = 16;

	/*
	 *
	 * Mask for the end bit
	 *
	 *
	 */
	public static final int END_DEC = 8;

	/******************** Set or unset all flags ********************/

	/*
	 *
	 * Set all flags
	 *
	 *
	 */
	public static final byte SET_ALL_FLAGS = (byte) 0b11111111;
	/*
	 *
	 * Reset all flags
	 *
	 *
	 */
	public static final byte UNSET_ALL_FLAGS = (byte) 0b00000000;

	/********************
	 * Setter and getter methods for the protocol flags
	 ********************/

	/*
	 * Setter Method for ACK-Flag
	 *
	 * @*flags flags array
	 * 
	 * @ack ACK or NACK
	 */
	public static byte set_ACK_Flag(byte flags, int ack) {
		if (ack == ACK)
			flags |= ACK_MASK;
		else
			flags &= ~ACK_MASK;

		return flags;
	}

	/*
	 * Setter Method for Req_to_send-Flag
	 *
	 * @*flags flags array
	 * 
	 * @req_to_send bit req_to_send
	 */
	public static byte set_Req_to_send_Flag(byte flags, int req_to_send) {
		if (req_to_send == 1)
			flags |= REQ_TO_SEND_MASK;
		else
			flags &= ~REQ_TO_SEND_MASK;

		return flags;
	}

	/*
	 * Setter Method for Rdy_to_rcv-Flag
	 *
	 * 
	 * @*flags flags array
	 * 
	 * @rdy_to_rcv bit req_to_send
	 */
	public static byte set_Rdy_to_rcv_Flag(byte flags, int rdy_to_rcv) {
		if (rdy_to_rcv == 1)
			flags |= READY_TO_RECV_MASK;
		else
			flags &= ~READY_TO_RECV_MASK;

		return flags;
	}

	/*
	 * Setter Method for Start-Flag
	 *
	 * @*flags flags array
	 * 
	 * @start first package
	 */
	public static byte set_Start_Flag(byte flags, int start) {
		if (start == 1)
			flags |= START_MASK;
		else
			flags &= ~START_MASK;

		return flags;
	}

	/*
	 * Setter Method for End-Flag
	 *
	 * @*flags flags array
	 * 
	 * @end last package
	 */
	public static byte set_End_Flag(byte flags, int end) {
		if (end == 1)
			flags |= END_MASK;
		else
			flags &= ~END_MASK;

		return flags;
	}

	/*
	 * Getter Method for ACK-Flag
	 *
	 * @param: *flags: The pointer of the flag byte from the header
	 *
	 * @return: SET (1) if the bit is set else NOT_SET (0)
	 */
	public static int get_ACK_flag(byte flags) {
		if ((flags & ACK_MASK) != 0)
			return SET;
		else
			return NOT_SET;
	}

	/*
	 * Getter Method for REQUEST_TO_SEND-Flag
	 *
	 * @param: *flags: The pointer of the flag byte from the header
	 *
	 * @return: SET (1) if the bit is set else NOT_SET (0)
	 */
	public static int get_Req_to_send_flag(byte flags) {
		if ((flags & REQ_TO_SEND_MASK) != 0)
			return SET;
		else
			return NOT_SET;
	}

	/*
	 * Getter Method for READY_TO_RECEIVE-Flag
	 *
	 * @param: *flags: The pointer of the flag byte from the header
	 *
	 * @return: SET (1) if the bit is set else NOT_SET (0)
	 */
	public static int get_ready_to_recv_flag(byte flags) {
		if ((flags & READY_TO_RECV_MASK) != 0)
			return SET;
		else
			return NOT_SET;
	}

	/*
	 * Getter Method for START-Flag
	 *
	 * @param: *flags: The pointer of the flag byte from the header
	 *
	 * @return: SET (1) if the bit is set else NOT_SET (0)
	 */
	public static int get_start_flag(byte flags) {
		if ((flags & START_MASK) != 0)
			return SET;
		else
			return NOT_SET;
	}

	/*
	 * Getter Method for END-Flag
	 *
	 * @param: *flags: The pointer of the flag byte from the header
	 *
	 * @return: SET (1) if the bit is set else NOT_SET (0)
	 */
	public static int get_end_flag(byte flags) {
		if ((flags & END_MASK) != 0)
			return SET;
		else
			return NOT_SET;
	}
}
