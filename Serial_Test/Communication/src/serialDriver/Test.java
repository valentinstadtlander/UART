package serialDriver;

import java.util.Scanner;

public class Test 
{
	public static void main(String[] args) 
	{
		Scanner in = new Scanner(System.in);
		SerialDriver comm = SerialDriver.getInstance(32, "SerialDriverTest");
		
		System.out.println("These ports are available:");
		int i = 1; 
		for(String iter: comm.identifySerialPorts())
		{
			System.out.println(i + ": " + iter);
			i++;
		}
		System.out.println("Mit welchem Port verbinden?");
		String port = in.nextLine();
		System.out.println(port);
		comm.connect(port);
		
		while(comm.isConnected())
		{
			System.out.println("Write to port:");
			String array = in.nextLine()  + '\n';
			for(char c: array.toCharArray())
				System.out.print("'" + c + "', ");
			System.out.println("Length: " + array.toCharArray().length);
			comm.writeToPort(array.getBytes());
			System.out.println("Done!");
			
			while(!comm.isSerialDataAvailable());
			
			System.out.println("Data available, print data!");
			
			for(byte c: comm.getRecvBuffer())
				System.out.print((char) c);
			System.out.println();
			
			if(array.equals("exit\n"))
			{
				comm.disconnect();
				System.out.println("Exit!");
			}
		}
	}
}
