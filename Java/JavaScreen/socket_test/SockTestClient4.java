import  java.applet.*;
import  java.awt.*;
import java.net.Socket;
import java.net.InetAddress;
import java.io.*;

public class SockTestClient4 extends Applet
{
	private static boolean _isApplet = false;
	private static TextArea _ta;

	public void init() {
		super.init();
		_isApplet = true;
		this.setBackground(Color.yellow);
		_ta = new TextArea("", 20, 80,
			TextArea.SCROLLBARS_VERTICAL_ONLY);
		this.add("Center", _ta);
		DoTest("devise.cs.wisc.edu", 6667);
	}

	public static void main(String args[]) {
		String host = "localhost";
		int port = 6667;

		for (int anum = 0; anum < args.length; ++anum) {
			if (args[anum].equals("-usage")) {
				myPrint("Usage: java SockTestClient4 [-host <host>] [-port <port>]");
				System.exit(0);
			} else if (args[anum].equals("-host")) {
				host = args[++anum];
			} else if (args[anum].equals("-port")) {
				port = Integer.parseInt(args[++anum]);
			}
		}

		DoTest(host, port);
	}

	public static void DoTest(String hostname, int port) {
		myPrint("Creating socket to <" + hostname +
		  ":" + port + ">");

	    try {
		    Socket sock = new Socket(hostname, port);

			InetAddress addr = sock.getInetAddress();
			myPrint("addr: " + addr);

			ObjectOutputStream sockOut =
			  new ObjectOutputStream(sock.getOutputStream());

			for (short num = 0; num < 10; ++num) {
				sockOut.writeShort(num);
				myPrint("  Wrote short: " + num);
			}

			for (int num = 0; num < 10; ++num) {
				sockOut.writeInt(num);
				myPrint("  Wrote int: " + num);
			}

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
