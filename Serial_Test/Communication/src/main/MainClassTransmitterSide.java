package main;

import java.util.Scanner;

import main.tcs.TCs;
import main.texts.TCTexts;
import protocol.Constants;
import protocol.UART_EIVE_Protocol_Recv;
import protocol.UART_EIVE_Protocol_Send;
import serialDriver.SerialDriver;

public class MainClassTransmitterSide {

	private static Scanner in;
	private static SerialDriver comm;
	private static UART_EIVE_Protocol_Send sender;
	private static UART_EIVE_Protocol_Recv receiver;
	private static int choise;
	
	public static void main(String[] args) {
		
		in = new Scanner(System.in);

		comm = SerialDriver.getInstance(32, "ProtocolTest");
		comm.connect("COM100");
		
		sender = new UART_EIVE_Protocol_Send(comm);
		receiver = new UART_EIVE_Protocol_Recv(comm);
		
		System.out.println("-----------------------Protocol Test-----------------------");
		choise = 0;
		while(choise < 5)
		{
			System.out.println();
			System.out.println("Which command do you want to send?");
			System.out.println(" " + 1 + ": send back text1 (short)");
			System.out.println(" " + 2 + ": send back text2 (medium)");
			System.out.println(" " + 3 + ": send back text3 (long)");
			System.out.println(" " + 4 + ": send back text4 (extra long)");
			System.out.println(">" + 4 + ": exit");
		
			choise = in.nextInt();
			switch(choise)
			{
			case 1: tc_send_text(TCs.tc1, Constants.UART_TC);
					break;
			case 2: tc_send_text(TCs.tc2, Constants.BRAM_TC); 
					break;
			case 3: tc_send_text(TCs.tc3, Constants.CAMERA_TC); 
					break;
			case 4: tc_send_text(TCs.tc4, Constants.DOWNLINK_TC); 
					break;
			default:
			}
		}
		
	}

	/**
	 * 
	 * @param tc
	 */
	private static void tc_send_text(String tc, byte id) {
		
		int status = sender.UART_Send_Data(id, tc.getBytes(), tc.length());
		if (status != Constants.XST_SUCCESS)
			System.out.println("Done! " + status);
		int tries = 0;
		do 
		{
			tries++;
			status = receiver.UART_Recv_Data();
			System.out.println("Status in main: " + status);
			if(status == Constants.XST_FAILURE)
				break;
		} 
		while(status != Constants.XST_SUCCESS);// && tries < 100);	
		
		System.out.println("Test?");
		if(status == Constants.XST_SUCCESS)
			System.out.println("TC done!");
		else
			System.out.println("TC failed!");
	}
	
	public static String getChosenText()
	{
		switch(choise)
		{
		case 1: return TCTexts.tc_text_1;
		case 2: return TCTexts.tc_text_2;
		case 3: return TCTexts.tc_text_3;
		case 4: return TCTexts.tc_text_4;
		default: 
			return "";
		}
	}
	

}
