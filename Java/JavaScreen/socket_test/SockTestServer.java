import java.net.Socket;
import java.net.ServerSocket;
import java.net.InetAddress;
import java.io.*;

public class SockTestServer
{
	public static void main(String args[]) {
		int port = 6667;

		for (int anum = 0; anum < args.length; ++anum) {
			if (args[anum].equals("-usage")) {
				myPrint("Usage: java SockTestClient [-port <port>]");
				System.exit(0);
			} else if (args[anum].equals("-port")) {
				port = Integer.parseInt(args[++anum]);
			}
		}
		DoTest(port);
	}

	public static void DoTest(int port) {
		myPrint("Creating socket to <" + port + ">");

	    try {
			ServerSocket sSock = new ServerSocket(port);
			myPrint("Starting to listen on server socket");
			while (true) {
				Socket sock = sSock.accept();
				myPrint("Got connection on server socket");
				processCmd(sock);
			}
	    } catch (Exception ex) {
	        myPrint("Exception (" + ex.toString() +
			  ") creating socket");
	    }
	}

	private static void processCmd(Socket sock) {
		try {
			DataInputStream sockIn = new DataInputStream(sock.getInputStream());
			//TEMPTEMPDataOutputStream sockOut = new DataOutputStream(sock.getOutputStream());

			short msgType = sockIn.readShort();
			myPrint("  msgType: " + msgType);

			int id = sockIn.readInt();
			myPrint("  id: " + id);

			short useCgi = sockIn.readShort();
			myPrint("  useCgi: " + useCgi);

			//TEMPTEMP -- more stuff here

		} catch (IOException ex) {
			myPrint("IOException (" + ex.toString() + ") processing command");
		}
	}

	private static void myPrint(String msg) {
		System.out.println(msg);
	}
}
