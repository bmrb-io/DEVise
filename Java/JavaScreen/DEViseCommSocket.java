// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class handles the command socket/image socket pairs that
// connect the devised to the jspop, and the jspop to the js.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.16  2001/02/16 17:45:57  xuk
// Changed variable cgiFlag to flag.
//
// Revision 1.15  2001/01/30 18:42:47  wenger
// Cleaned up formatting.
//
// Revision 1.14  2001/01/30 03:10:25  xuk
// Change for collaboration JS. cgiFlag = -1 to indicate collaboration JS.
//
// Revision 1.13  2001/01/22 17:08:12  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.12  2001/01/08 20:31:51  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.11.4.13  2001/01/05 19:15:45  wenger
// Updated copyright dates.
//
// Revision 1.11.4.12  2001/01/05 17:56:24  wenger
// Minor code cleanups.
//
// Revision 1.11.4.11  2000/12/21 23:28:27  wenger
// Got CGI mode working with applet, minor cleanup still needed; more debug
// output in DEViseCommSocket, jspop, and js.cgi; comments updated for
// new command format that always includes ID and CGI flag.
//
// Revision 1.11.4.10  2000/12/07 17:27:10  xuk
// Sending command ID and cgi flag with every command;
// Retrieving command ID and cgi flag when receiving a command in receiveCmd().
//
// Revision 1.11.4.9  2000/11/21 20:58:37  xuk
// *** empty log message ***
//
// Revision 1.11.4.8  2000/11/21 20:43:31  xuk
// Include cmdId and cgiFlag in every command sent out and received.
//
// Revision 1.11.4.7  2000/11/15 18:44:00  wenger
// Added debug code.
//
// Revision 1.11.4.6  2000/11/10 16:19:43  wenger
// JavaScreen sends message type (API_JAVA_WID) to cgi script.
//
// Revision 1.11.4.5  2000/11/10 15:50:38  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.11.4.4  2000/11/02 23:51:11  xuk
// In sendCmd() send one short to indicate whether it's a cgi communication. default cgi=0.
//
// Revision 1.11.4.3  2000/10/18 20:28:10  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.11.4.2  2000/10/02 19:19:49  xuk
// Changing sendCmd() function for add a ID for each command sent from JS to JSPoP.
//
// Revision 1.11.4.1  2000/09/01 20:00:38  xuk
// Merge command socket and image socket into one socket.
//
// Revision 1.11.2.1  2000/08/02 16:26:53  xuk
// *** empty log message ***
//
// Revision 1.11  2000/07/19 20:11:36  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.10  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.9  2000/04/21 19:45:45  wenger
// Cleaned up DEViseCommSocket class -- removed duplicate code, better
// variable names, more comments.
//
// Revision 1.8  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.7  2000/03/06 08:09:10  hongyu
// fix the 'invalid image data' bug --- hongyu
//
// Revision 1.6  1999/12/07 20:37:44  wenger
// Changed reading of data from socket to a single system call to speed things
// up.
//
// Revision 1.5  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.4  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.3  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCommSocket.java

import  java.io.*;
import  java.net.*;

public class DEViseCommSocket
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private Socket socket = null;
    public DataInputStream is = null;
    public DataOutputStream os = null;

    // timeout = 0 means no time out
    private int timeout = 0;
    private int checkingSocketTimeout = 200;

    // the buffer size for BufferedOutputStream, JDK defaults is 512
    private int bufferSize = 512;

    // The following data are used in receiveCmd() and receiveData() to support timeout
    private boolean isControl = true; // ADD COMMENT about purpose of isControl
    private int msgType = 0, numberOfElement = 0, totalSize = 0;
    private byte[] dataRead = null;
    private int numberRead = 0;

    public int cmdId = 0;
    public int flag = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseCommSocket(Socket s, int to)
      throws YException
    {
        if (s == null) {
            throw new YException("Invalid socket in arguments",
	      "DEViseCommSocket:constructor");
        } else {
            socket = s;
        }

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

	CreateStreams();
    }

    //-------------------------------------------------------------------
    public DEViseCommSocket(Socket s) throws YException
    {
        // default time out is 1 second
        this(s, 1000);
    }

    //-------------------------------------------------------------------
    public DEViseCommSocket(String hostname, int port, int to)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket constructor(" + hostname +
	      ", " + port + ", " + to + ")");
	}

        if (hostname == null)
            throw new YException("Invalid hostname in arguments",
	      "DEViseCommSocket:constructor");

        if (port < 1024 || port > 65535)
            throw new YException("Invalid port number in arguments",
	      "DEViseCommSocket:constructor");

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            socket = new Socket(hostname, port);
        } catch (NoRouteToHostException e) {
	    System.err.println("Exception in DEViseCommSocket constructor: " +
	      e.getMessage());
            closeSocket();
            throw new YException(
	      "Can not find route to host, may caused by an internal firewall", 
	      "DEViseCommSocket:constructor");
        } catch (UnknownHostException e) {
	    System.err.println("Exception in DEViseCommSocket constructor: " +
	      e.getMessage());
            closeSocket();
            throw new YException("Unknown host {" + hostname + "}",
	      "DEViseCommSocket:constructor");
        } catch (IOException e) {
	    System.err.println("Exception in DEViseCommSocket constructor: " +
	      e.getMessage());
            closeSocket();
            throw new YException("Can not open socket connection to host {"
	      + hostname + "}", "DEViseCommSocket:constructor");
        }

	CreateStreams();
    }

    //-------------------------------------------------------------------
    public DEViseCommSocket(String hostname, int port)
      throws YException
    {
        // default time out is 1 second
        this(hostname, port, 1000);
    }

    //-------------------------------------------------------------------
    public synchronized void closeSocket()
    {
        try {
            if (os != null)
                os.close();
        } catch (IOException e) {
        }

        try {
            if (is != null)
                is.close();
        } catch (IOException e) {
        }

        try {
            if (socket != null)
                socket.close();
        } catch (IOException e) {
        }

        os = null;
        is = null;
        socket = null;
    }

    //-------------------------------------------------------------------
    // if at the moment of calling, there is something coming from the input stream,
    // isEmpty will return false, otherwise, it will return true
    public synchronized boolean isEmpty() throws YException
    {
        try {
            if (is != null && is.available() > 0) {
                return false;
            } else {
                return true;
            }
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream",
	      "DEViseCommSocket:isEmpty()");
        }
    }

    //-------------------------------------------------------------------
    // Clear all incoming data off of the sockets.
    public synchronized void clearSocket() throws YException
    {
        clearSocket(-1);
    }

    //-------------------------------------------------------------------
    // Clear all incoming data off of the sockets.
    // imageBytes = -1 if size of image unknown
    public synchronized void clearSocket(int imageBytes) throws YException
    {
        resetData();

        try {
            if (socket != null && is != null) {
                socket.setSoTimeout(0);

                if (imageBytes < 0) {
		    clearStream(is, checkingSocketTimeout);
                } else if (imageBytes > 0) {
		    is.skipBytes(imageBytes);
                } 

                socket.setSoTimeout(timeout);
            }
        } catch (IOException e) {
            closeSocket();
	    System.err.println(e.getMessage());
            throw new YException(
	      "Error occurs while reading from input stream",
	      "DEViseCommSocket:clearSocket()");
        }
    }

    //-------------------------------------------------------------------
    // Note: the format of commands on the socket is as follows:
    // u_short msgType // API_CMD, etc. -- see DEViseGlobals,java,
    //                    graphics.new/ParseAPI.h
    // u_short id // JavaScreen ID; sent only if msgType is API_JAVA_WID
    // u_short useCgi // whether we're using the CGI interface; sent only
    //                   if msgType is API_JAVA_WID
    // u_short argCount
    // u_short totalBytes // size of everything remaining
    // u_short arg1Bytes
    // char [] arg1String // null terminated
    // u_short arg2Bytes
    // char [] arg2String // null terminated
    // ...

    //-------------------------------------------------------------------
    public synchronized void sendCmd(String cmd, short msgType, int ID)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.sendCmd(" + cmd + ", " +
	      msgType + ", " + ID + ")");
        }

        if (os == null) {
            closeSocket();
            throw new YException("Invalid output stream",
	      "DEViseCommSocket:sendCmd()");
        }

        // for invalid cmd, simply discard it
        if (cmd == null || cmd.length() == 0)
            return;

        // for invalid cmd, simply discard it
        String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);
        if (cmdBuffer == null || cmdBuffer.length == 0)
            return;

        int nelem = 0, size = 0;
        int flushedSize = 0;

        try {
            nelem = cmdBuffer.length;
            for (int i = 0; i < nelem; i++)  {
		// null-terminate the string for C/C++ on the other end
		// one byte
                cmdBuffer[i] = cmdBuffer[i] + "\u0000";

                // since DataOutputStream is a Byte oriented Stream, so it will write
                // each unicode character(16bits) as ISO-Latin-1 8bits character, which
                // means only the low 8bits of each unicode character is write out, and
                // the high 8bits has been throwed out.
                size = size + 2 + cmdBuffer[i].length();
            }

	    os.writeShort(msgType);
	    os.writeShort((short)ID);
	    os.writeShort(0); // not cgi

            // if nelem is greater than MAX_VALUE of short, if you use readUnsignedShort
            // on the other size, you can still get correct value
            os.writeShort(nelem);
            os.writeShort(size);

            for (int i = 0; i < nelem; i++) {
                // Since we are using a BufferedOutputStream as the underlying stream of
                // our DataOutputStream, so we must make sure that while the buffer is full,
                // we must flush our stream.
                if ((os.size() - flushedSize) >= (bufferSize - 2)) {
                    flushedSize = os.size();
                    os.flush();
                }

                os.writeShort(cmdBuffer[i].length());
                os.writeBytes(cmdBuffer[i]);
            }

            os.flush();
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while writing to output stream", "DEViseCommSocket:sendCmd()");
        }
    }

    //-------------------------------------------------------------------
    public void sendCmd(String cmd, short msgType) throws YException
    {
        sendCmd(cmd, msgType, DEViseGlobals.DEFAULTID);
    }

    //-------------------------------------------------------------------
    public void sendCmd(String cmd) throws YException
    {
        sendCmd(cmd, DEViseGlobals.API_JAVA, DEViseGlobals.DEFAULTID);
    }

    //-------------------------------------------------------------------
    // Receive a command.  Note that this method may be interrupted by
    // the socket timeout.  If so, it can be repeatedly called until
    // an entire command has been received.
    public synchronized String receiveCmd()
      throws YException, InterruptedIOException
    {
        if (is == null) {
            closeSocket();
            throw new YException("Invalid input stream",
	      "DEViseCommSocket:receiveCmd()");
        }

        try {
            if (isControl) {
                if (dataRead == null) {
		    //TEMP -- 10 is 'magic constant' here!
                    dataRead = new byte[10];
                    numberRead = 0;
                }

		// TEMP -- try read(buf, offset, len) here
		
                int b;
                for (int i = numberRead; i < 10; i++) {
                    b = is.read();
                    if (b < 0) {
                        closeSocket();
                        throw new YException("Abrupt end of input stream reached", "DEViseCommSocket.receiveCmd()");
                    }

                    dataRead[numberRead] = (byte)b;
                    numberRead++;
                }

                msgType = DEViseGlobals.toUshort(dataRead);
                cmdId = DEViseGlobals.toUshort(dataRead, 2);
                flag = DEViseGlobals.toUshort(dataRead, 4);
                numberOfElement = DEViseGlobals.toUshort(dataRead, 6);
                totalSize = DEViseGlobals.toUshort(dataRead, 8);

		// for collabration JS
		if (msgType == DEViseGlobals.API_JAVA_CID)
		    flag = -1; //TEMP use cgiFlag=-1 to indicate collab JS

                dataRead = null;
                isControl = false;

                if (numberOfElement <= 0 || totalSize <= 0) {
                    closeSocket();
                    throw new YException("Invalid control information received", "DEViseCommSocket.receiveCmd()");
                }
            }

            if (dataRead == null) {
                dataRead = new byte[totalSize];
                numberRead = 0;
            }

	    // TEMP -- try read(buf, offset, len) here
            int b;
            for (int i = numberRead; i < totalSize; i++) {
                b = is.read();
                if (b < 0) {
                    closeSocket();
                    throw new YException("Abrupt end of input stream reached",
		      "DEViseCommSocket.receiveCmd()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }
        } catch (InterruptedIOException e) {
            throw e;
        } catch (IOException e) {
            closeSocket();
	    System.err.println(e.getMessage());
            throw new YException("Error occurs while reading from input stream", "DEViseCommSocket:receiveCmd()");
        }

        int argsize = 0;
        int pastsize = 0;
        String response = new String("");
        for (int i = 0; i < numberOfElement; i++) {
            if (totalSize < pastsize + 2) {
                closeSocket();
                throw new YException("Inconsistant data received 1", "DEViseCommSocket:receiveCmd()");
            }

            argsize = DEViseGlobals.toUshort(dataRead, pastsize);
            pastsize += 2;

            if (totalSize < pastsize + argsize) {
                closeSocket();
                throw new YException("Inconsistant data received 2", "DEViseCommSocket:receiveCmd()");
            }

            // use argsize - 1 instead of argsize is to skip the string ending '\0'
            // use one space to seperate different parts of the response command
            response += new String(dataRead, pastsize, argsize - 1) + " ";
            pastsize += argsize;
        }

        resetData();

        return response;
    }

    //-------------------------------------------------------------------
    public synchronized void sendData(byte[] data) throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.sendData(" + data.length +
	      " bytes)");
        }

        if (os == null) {
            closeSocket();
            throw new YException("Invalid output stream", "DEViseCommSocket:sendData()");
        }

        // simply give up if the input data is invalid
        if (data == null || data.length == 0)
            return;

        try {
            os.write(data, 0, data.length);
            os.flush(); // I am not using BufferedOutputStream for data transfer, so
                           // this method will call its underlying outputstream's flush()
                           // method, which would be OutputStream's flush() method, which
                           // actually does nothing
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while writing to output stream",
	      "DEViseCommSocket:sendData()");
        }
    }

    //-------------------------------------------------------------------
    // Number of bytes available on data socket.
    public int dataAvailable()
    {
	int bytes;

        if (is == null) {
	    bytes = 0;
	} else {
	    try {
	        bytes = is.available();
	    } catch (IOException e) {
		System.err.println("Exception " + e.getMessage() +
		  "while getting number of bytes available");
	        bytes = 0;
	    }
	}

	return bytes;
    }

    //-------------------------------------------------------------------
    // Receive data.  This method now does not return until all of the
    // requested data has been read.
    public synchronized byte[] receiveData(int dataSize)
      throws YException
    {
        if (is == null) {
            closeSocket();
            throw new YException("Invalid input stream",
	      "DEViseCommSocket:receiveData()");
        }

        // simply give up receiving if input size is invalid
        if (dataSize <= 0) {
            return null;
        }

        try {
            if (dataRead == null) {
                dataRead = new byte[dataSize];
                numberRead = 0;
            }

	    //TEMP -- make this into a method?
            int offset = 0;
	    int bytesRemaining = dataSize;
	    while (bytesRemaining > 0) {
		try {
	            int bytesRead = is.read(dataRead, offset,
		      bytesRemaining);
		    offset += bytesRead;
		    bytesRemaining -= bytesRead;
		} catch(InterruptedIOException e) {
		    // Note: this may happen if the data transfer is too slow.
		    // Commented out because of static variable fix.
		    // DEViseDebugLog.log("Error reading data: " +
		      // e.getMessage());
		    try {
		        Thread.sleep(100); // wait for some more data on socket
		    } catch (InterruptedException ex) {
		    }
		}
	    }

            byte[] data = dataRead;

            resetData();

            return data;
        } catch (IOException e) {
            closeSocket();
	    System.err.println(e.getMessage());
            throw new YException(
	      "Error occurs while reading from input stream",
	      "DEViseCommSocket:receiveData()");
        }
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Create input and output streams if we already have the sockets.
    private void CreateStreams() throws YException
    {
        try {
            os = new DataOutputStream(new BufferedOutputStream(
	      socket.getOutputStream(), bufferSize));
            is = new DataInputStream(socket.getInputStream());
            socket.setSoTimeout(timeout);
        } catch (NoRouteToHostException e) {
            closeSocket();
            throw new YException(
	      "Can not find route to host, may caused by an internal firewall",
	      "DEViseCommSocket:constructor");
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for sockets",
	      "DEViseCommSocket:constructor");
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open i/o stream for sockets",
	      "DEViseCommSocket:constructor");
        }
    }

    //-------------------------------------------------------------------
    private static void clearStream(DataInputStream istream, int to)
      throws IOException
    {
        int size = istream.available();
        boolean isEnd = false;
        while (size > 0 || !isEnd) {
            if (size <= 0) {
                try {
                    Thread.sleep(to);
                } catch (InterruptedException e1) {
                }
                isEnd = true;
            } else {
		istream.skipBytes(size);
            }

            size = istream.available();
        }
    }

    //-------------------------------------------------------------------
    private synchronized void resetData()
    {
        isControl = true;
        msgType = 0;
        numberOfElement = 0;
        totalSize = 0;
        dataRead = null;
        numberRead = 0;
    }
}

// ========================================================================
