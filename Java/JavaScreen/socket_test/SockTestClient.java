import  java.applet.*;
import  java.awt.*;
import java.net.Socket;
import java.net.InetAddress;
import java.io.*;

public class SockTestClient extends Applet
{
	private static boolean _isApplet = false;
	private static TextArea _ta;

	public void init() {
		super.init();
		_isApplet = true;
		this.setBackground(Color.yellow);
		//TEMPTEMP -- make a text frame or whatever the hell it is, and print stuff to that if we're an applet
		//
		_ta = new TextArea("", 20, 80,//TEMPTEMP
			TextArea.SCROLLBARS_VERTICAL_ONLY);
		this.add("Center", _ta);
		DoTest("devise.cs.wisc.edu", 6667);//TEMPTEMP?
	}

	public static void main(String args[]) {
		DoTest("devise.cs.wisc.edu", 6667);//TEMPTEMP?
	}

	public static void DoTest(String hostname, int port) {
		myPrint("Creating socket to <" + hostname +
		  ":" + port + ">");

	    //TEMPTEMP -- more stuff here
	    try {
		    Socket sock = new Socket(hostname, port);

			InetAddress addr = sock.getInetAddress();
			myPrint("addr: " + addr);

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

			myPrint("\nSending command");

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
				myPrint("  " + cmd[i]);
			}

            sockOut.writeShort(nelem);
            sockOut.writeShort(size);

			for (int i = 0; i < nelem; ++i) {
			    sockOut.writeShort(cmd[i].length());
				sockOut.writeBytes(cmd[i]);
			}

			myPrint("\nReading reply");

			// Read command back...
			// TEMP -- this isn't fully correct yet, but it's good
			// enough to see if we get something back...
			short msgType = sockIn.readShort();
			myPrint("msgType: " + msgType);

			int cmdId = sockIn.readInt();
			myPrint("cmdId: " + cmdId);

			short flag = sockIn.readShort();
			myPrint("flag: " + flag);

			nelem = sockIn.readShort();
			myPrint("nelem: " + nelem);

			size = sockIn.readShort();
			myPrint("size: " + size);

			byte buf[] = new byte[size];
			for (int i = 0; i < size; ++i) {
				buf[i] = sockIn.readByte();
			}

			String cmdIn = new String(buf);
			myPrint("cmdIn: " + cmdIn);



			sock.close();
	    } catch (Exception ex) {
	        myPrint("Exception (" + ex.toString() +
			  ") creating socket");
	    }
	}

	private static void myPrint(String msg) {
		System.out.println(msg);
		if (_isApplet) {
			_ta.append(msg + "\n");
		}
	}
}
