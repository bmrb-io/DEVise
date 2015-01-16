//TEMPTEMP -- need to also have this as an applet
import java.net.Socket;
import java.net.InetAddress;
import java.io.*;

class SockTestClient {

	public static void main(String args[]) {
		DoTest("devise.cs.wisc.edu", 6667);//TEMPTEMP?
	}

	public static void DoTest(String hostname, int port) {
		System.out.println("Creating socket to <" + hostname +
		  ":" + port + ">");

	    //TEMPTEMP -- more stuff here
	    try {
		    Socket sock = new Socket(hostname, port);

			InetAddress addr = sock.getInetAddress();
			System.out.println("addr: " + addr);

			DataInputStream sockIn =
			  new DataInputStream(sock.getInputStream());
			DataOutputStream sockOut =
			  new DataOutputStream(sock.getOutputStream());

//DEViseCommSocket.sendCmd(JAVAC_Connect, 5, 0)
//  cmd: JAVAC_Connect
//  cmd: {guest}
//  cmd: {guest}
//  cmd: {17.1}
//  cmd: {client_host}

			System.out.println("\nSending command");

            sockOut.writeShort(5); // msgType
			sockOut.writeInt(0); // ID
            sockOut.writeShort(0); // not CGI

			int nelem = 5;
			String cmd[] = new String[nelem];
			cmd[0] = "JAVAC_Connect" + "\u0000";
			cmd[1] = "{guest}" + "\u0000";
			cmd[2] = "{guest}" + "\u0000";
			cmd[3] = "{17.1}" + "\u0000";
			cmd[4] = "{client_host}" + "\u0000";

			int size = 0;
			for (int i = 0; i < nelem; ++i) {
				size += cmd[i].length() + 2; // +2 copied from real code
				System.out.println("  " + cmd[i]);
			}

            sockOut.writeShort(nelem);
            sockOut.writeShort(size);

			for (int i = 0; i < nelem; ++i) {
			    sockOut.writeShort(cmd[i].length());
				sockOut.writeBytes(cmd[i]);
			}

			System.out.println("\nReading reply");

			// Read command back...
			// TEMP -- this isn't fully correct yet, but it's good
			// enough to see if we get something back...
			short msgType = sockIn.readShort();
			System.out.println("msgType: " + msgType);

			int cmdId = sockIn.readInt();
			System.out.println("cmdId: " + cmdId);

			short flag = sockIn.readShort();
			System.out.println("flag: " + flag);

			nelem = sockIn.readShort();
			System.out.println("nelem: " + nelem);

			size = sockIn.readShort();
			System.out.println("size: " + size);

			byte buf[] = new byte[size];
			for (int i = 0; i < size; ++i) {
				buf[i] = sockIn.readByte();
			}

			String cmdIn = new String(buf);
			System.out.println("cmdIn: " + cmdIn);



			sock.close();
	    } catch (Exception ex) {
	        System.err.println("Exception (" + ex.toString() +
			  ") creating socket");
	    }
	}
}
