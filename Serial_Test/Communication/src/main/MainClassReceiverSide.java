package main;

import main.texts.TCTexts;
import protocol.Constants;
import protocol.UART_EIVE_Protocol_Recv;
import protocol.UART_EIVE_Protocol_Send;
import serialDriver.SerialDriver;

public class MainClassReceiverSide {

	private static SerialDriver comm;
	private static UART_EIVE_Protocol_Send sender;
	private static UART_EIVE_Protocol_Recv receiver;
	private static int status;

	public static void main(String[] args) {
		System.out.println("-----------------------Protocol Test-----------------------");
		
		comm = SerialDriver.getInstance(32, "ProtocolTest");
		comm.connect("COM99");
		
		sender = new UART_EIVE_Protocol_Send(comm);
		receiver = new UART_EIVE_Protocol_Recv(comm);
		
		while(true)
		{
			do 
			{
				status = receiver.UART_Recv_Data();
			} 
			while(status != Constants.XST_SUCCESS || status != Constants.XST_FAILURE);	
		}
	}

	public static void sendTM(int i, byte ID) {
		
		switch(i)
		{
		case 1: sender.UART_Send_Data(ID, TCTexts.tc_text_1.getBytes(), TCTexts.tc_text_1.length());
				break;
		case 2: sender.UART_Send_Data(ID, TCTexts.tc_text_2.getBytes(), TCTexts.tc_text_2.length());
				break;
		case 3: sender.UART_Send_Data(ID, TCTexts.tc_text_3.getBytes(), TCTexts.tc_text_3.length());
				break;
		case 4: sender.UART_Send_Data(ID, TCTexts.tc_text_4.getBytes(), TCTexts.tc_text_4.length());
				break;
		default:
		}
		
	}

}
