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
// Revision 1.32  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.31  2001/10/30 17:23:51  xuk
// Created DEViseClient object for collaborating clients in jspop.
// In doReceiveCmd(), no difference for collaboration mode clients.
//
// Revision 1.30.2.2  2001/11/07 17:22:36  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.30.2.1  2001/10/28 18:13:18  wenger
// Made msgType and cmdId private in DEViseCommSocket; other minor cleanups.
//
// Revision 1.30  2001/10/25 21:35:42  wenger
// Added heartbeat count to heartbeat command (for debugging); other minor
// cleanup and debug code additions.
//
// Revision 1.29  2001/10/24 17:46:07  wenger
// Fixed bug 720 (one client can block others in the JSPoP).  The fix is that
// the JSPoP now has a separate thread to read from each client.
//
// Revision 1.28  2001/10/22 18:38:24  wenger
// A few more cleanups to the previous fix.
//
// Revision 1.27  2001/10/22 18:06:49  wenger
// Fixed bug 718 (client sockets never deleted in JSPoP).
//
// Revision 1.26  2001/10/12 19:12:41  wenger
// Changed client ID generation to avoid any chance of duplicates;
// updated command format documentation.
//
// Revision 1.25  2001/10/12 01:55:44  xuk
// Using timestamp-based client ID.
// 1. cmdId has been expanded to long int;
// 2. Modified sendCmd(), ID has been expanded to long int;
// 3. Modified receiveCmd(), read 16 bytes of message header, since now cmdId is 8 bytes long.
//
// Revision 1.24  2001/09/10 21:18:52  xuk
// Solve the client disconnection problem.
// Added isAvailable() method to check whether there is something with the socket connection.
// Changed in receiveCmd() method to indicate the socket connection is disabled.
//
// Revision 1.23  2001/05/11 20:36:05  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.22  2001/04/06 19:32:14  wenger
// Various cleanups of collaboration code (working on strange hang
// that Miron has seen); added more debug output; turned heartbeat
// back on (it somehow got turned off by accident).
//
// Revision 1.21  2001/03/20 20:11:39  wenger
// Added more debug output to the JS client and jspop.
//
// Revision 1.20  2001/03/08 21:10:13  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.19  2001/03/03 20:14:02  xuk
// Restore old state if user goes into, then out of, collaboration mode.
//
// Revision 1.18  2001/02/20 20:02:22  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.17  2001/02/16 17:48:45  xuk
// *** empty log message ***
//
// Revision 1.16  2001/02/16 17:45:57  xuk
// Changed variable cgiFlag to flag.
//
// Revision 1.15.2.5  2001/02/05 17:13:19  wenger
// Finally implemented a way to timeout on the Socket constructor that
// works in Netscape (wrote my own version of Thread.join() because
// Netscape's doesn't work right).
//
// Revision 1.15.2.4  2001/02/01 21:54:27  wenger
// More fixes to socket constructor timeout.
//
// Revision 1.15.2.3  2001/02/01 20:52:14  wenger
// Fixed socket constructor timeout to work in applets.
//
// Revision 1.15.2.2  2001/01/31 21:02:39  wenger
// Added timeout to socket creation in the client (currently set to 5
// sec.); improved GUI for socket failure.
//
// Revision 1.15.2.1  2001/01/31 17:44:02  wenger
// Cron job to check jspop now runs every minute on yola; added more
// diagnostic output to checking; temporarily? increased socket receive
// timeouts to see if this helps on yola; added timestamp to
// JAVAC_CheckPop command.
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

//TEMP package edu.wisc.cs.devise.js.comm;
package JavaScreen;

import  java.io.*;
import  java.net.*;
import  java.util.Date;

public class DEViseCommSocket
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //TEMP -- increased this from 1000 to see if it helps on yola.
    private static final int DEFAULT_RCV_TIMEOUT = 10000; // millisec

    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;

    // timeout = 0 means no time out
    private int timeout = 0;
    private int checkingSocketTimeout = 200;

    // the buffer size for BufferedOutputStream, JDK defaults is 512
    private int bufferSize = 512;

    // The following data are used in receiveCmd() and receiveData() to support timeout
    private boolean isControl = true; // ADD COMMENT about purpose of isControl
    private int msgType = 0;
    private int numberOfElement = 0, totalSize = 0;
    private byte[] dataRead = null;
    private int numberRead = 0;

    private int cmdId = 0;

    //TEMP -- document what this means -- -1 seems to mean collaboration
    private int flag = 0;

    // Use these objects for mutex on reading and writing.  I am making
    // the reading and writing methods non-synchronized because it makes
    // sense for one thread to read from the socket and another to write
    // to it at the same time; however we don't want to allow multiple
    // threads to write to the socket, or multiple threads to read from
    // the socket, at the same time.  RKW 2001-10-24.
    private Boolean readSync = new Boolean(false);
    private Boolean writeSync = new Boolean(false);

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseCommSocket(Socket s, int to)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket constructor(" + to + ")");
	}

        if (s == null) {
            throw new YException("Invalid socket in arguments",
	      "DEViseCommSocket:constructor");
        } else {
            socket = s;
        }

        if (to < 0) {
            timeout = 0;
        } else {
            timeout = to;
        }

	CreateStreams();
    }

    //-------------------------------------------------------------------
    public DEViseCommSocket(Socket s) throws YException
    {
        this(s, DEFAULT_RCV_TIMEOUT);
    }

    //-------------------------------------------------------------------
    public DEViseCommSocket(String hostname, int port, int to)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket constructor(" + hostname +
	      ", " + port + ", " + to + ")");
	}

	createSocket(hostname, port, to);
    }

    //-------------------------------------------------------------------
    protected void finalize()
    {
        closeSocket();
    }

    //-------------------------------------------------------------------
    // constTO is constructor timeout in milliseconds
    public DEViseCommSocket(String hostname, int port, int to, int constTO)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket constructor(" + hostname +
	      ", " + port + ", " + to + ", " + constTO + ")");
	}

	SocketCreator tmp = new SocketCreator(hostname, port, to, constTO);
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
	if (DEBUG >= 1) {
	    System.out.println("DEViseCommSocket.closeSocket()");
	}

        try {
            if (os != null) {
                os.close();
	    }
        } catch (IOException e) {
	    System.err.println(e.getMessage());
        }

        try {
            if (is != null) {
                is.close();
	    }
        } catch (IOException e) {
	    System.err.println(e.getMessage());
        }

        try {
            if (socket != null) {
                socket.close();
	    }
        } catch (IOException e) {
	    System.err.println(e.getMessage());
        }

        os = null;
        is = null;
        socket = null;
    }

    //-------------------------------------------------------------------
    //TEMP -- should return short to be consistent with writing and 
    // "real" values in command, but I'm leaving it an int for now.
    // RKW 2001-10-28.
    public int getMsgType()
    {
        return msgType;
    }

    //-------------------------------------------------------------------
    public int getCmdId()
    {
        return cmdId;
    }

    //-------------------------------------------------------------------
    //TEMP -- should return short to be consistent with writing and 
    // "real" values in command, but I'm leaving it an int for now.
    // RKW 2001-10-28.
    public int getFlag()
    {
        return flag;
    }

    //-------------------------------------------------------------------
    // Whether this socket is open.
    public boolean isOpen()
    {
        return ((socket != null) && (is != null) && (os != null));
    }

    //-------------------------------------------------------------------
    // if at the moment of calling, there is something coming from the
    // input stream, isEmpty will return false, otherwise, it will return
    // true.
    public boolean isEmpty() throws YException
    {
        try {
            if (is != null && is.available() > 0) {
                return false;
            } else {
                return true;
            }
        } catch (IOException e) {
            closeSocket();
            System.err.println("Can not read from input stream in " +
	      "DEViseCommSocket:isEmpty()");
            throw new YException("Can not read from input stream",
	      "DEViseCommSocket:isEmpty()");
        }
    }

    //-------------------------------------------------------------------
    // if at the moment of calling, there is something wrong with the socket
    // isAvailable will return false, otherwise, it will return true
    public boolean isAvailable() throws YException
    {
        try {
            int b = is.available();
            os.flush();
	    InputStream i = socket.getInputStream();
	    OutputStream o = socket.getOutputStream();
        } catch (IOException e) {
            closeSocket();
            System.err.println("Socket is not available " +
	      "DEViseCommSocket:isAvailable()");
	    return false;
            //throw new YException("Can not read from input stream",
	    //  "DEViseCommSocket:isEmpty()");
        }

	return true;
    }

    //-------------------------------------------------------------------
    // Clear all incoming data off of the sockets.
    public void clearSocket() throws YException
    {
        clearSocket(-1);
    }

    //-------------------------------------------------------------------
    // Clear all incoming data off of the sockets.
    // imageBytes = -1 if size of image unknown
    public void clearSocket(int imageBytes) throws YException
    {
	synchronized (readSync) {
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
    }

    //-------------------------------------------------------------------
    // Note: the format of commands on the socket is as follows:
    // u_short msgType // API_CMD, etc. -- see DEViseGlobals,java,
    //                    graphics.new/ParseAPI.h
    // int id // JavaScreen ID
    // u_short useCgi // whether we're using the CGI interface
    // u_short argCount
    // u_short totalBytes // size of everything remaining
    // u_short arg1Bytes
    // char [] arg1String // null terminated
    // u_short arg2Bytes
    // char [] arg2String // null terminated
    // ...

    //-------------------------------------------------------------------
    public void sendCmd(String cmd, short msgType, int ID)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.sendCmd(" + cmd + ", " +
	      msgType + ", " + ID + ")");
        }

	synchronized (writeSync) {
	    doSendCmd(cmd, msgType, ID);
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
    public void sendCmd(String[] cmd, short msgType, int ID)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.sendCmd(" + cmd + ", " +
	      msgType + ", " + ID + ")");
        }

	synchronized (writeSync) {
	    doSendCmd(cmd, msgType, ID);
	}
    }

    //-------------------------------------------------------------------
    public void sendCmd(String[] cmd, short msgType) throws YException
    {
        sendCmd(cmd, msgType, DEViseGlobals.DEFAULTID);
    }

    //-------------------------------------------------------------------
    public void sendCmd(String[] cmd) throws YException
    {
        sendCmd(cmd, DEViseGlobals.API_JAVA, DEViseGlobals.DEFAULTID);
    }

    //-------------------------------------------------------------------
    // Receive a command.  Note that this method may be interrupted by
    // the socket timeout.  If so, it can be repeatedly called until
    // an entire command has been received.
    public String receiveCmd()
      throws YException, InterruptedIOException
    {
        synchronized (readSync) {
	    return doReceiveCmd();
	}
    }

    //-------------------------------------------------------------------
    public void sendData(byte[] data) throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.sendData(" + data.length +
	      " bytes)");
        }

	synchronized (writeSync) {
	    doSendData(data);
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
    // The number of bytes that have been written to the socket.
    public int bytesWritten()
    {
        return os.size();
    }

    //-------------------------------------------------------------------
    // Receive data.  This method now does not return until all of the
    // requested data has been read.
    public byte[] receiveData(int dataSize)
      throws YException
    {
        synchronized (readSync) {
	    return doReceiveData(dataSize);
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private void createSocket(String hostname, int port, int to)
      throws YException
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseCommSocket.createSocket(" + hostname +
	      ", " + port + ", " + to + ")");
	}

        if (hostname == null)
            throw new YException("Invalid hostname in arguments",
	      "DEViseCommSocket.createSocket");

        if (port < 1024 || port > 65535)
            throw new YException("Invalid port number in arguments",
	      "DEViseCommSocket.createSocket");

        if (to < 0) {
            timeout = 0;
        } else {
            timeout = to;
	}

        try {
            socket = new Socket(hostname, port);
        } catch (NoRouteToHostException e) {
	    System.err.println("Exception in DEViseCommSocket.createSocket: " +
	      e.getMessage());
            closeSocket();
            throw new YException(
	      "Can not find route to host, may caused by an internal firewall", 
	      "DEViseCommSocket.createSocket");
        } catch (UnknownHostException e) {
	    System.err.println("Exception in DEViseCommSocket.createSocket: " +
	      e.getMessage());
            closeSocket();
            throw new YException("Unknown host {" + hostname + "}",
	      "DEViseCommSocket:constructor");
        } catch (IOException e) {
	    System.err.println("Exception in DEViseCommSocket.createSocket: " +
	      e.getMessage());
            closeSocket();
            throw new YException("Can not open socket connection to host {"
	      + hostname + "}", "DEViseCommSocket.createSocket");
	}

	CreateStreams();
    }

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
    private void doSendCmd(String cmd, short msgType, int ID)
      throws YException
    {
        if (os == null) {
            closeSocket();
            throw new YException("Invalid output stream",
	      "DEViseCommSocket:sendCmd()");
        }

        // for invalid cmd, simply discard it
        if (cmd == null || cmd.length() == 0) {
	    System.err.println("Zero-length command");
            return;
        }

        // for invalid cmd, simply discard it
        String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);
        if (cmdBuffer == null || cmdBuffer.length == 0) {
	    System.err.println("Unparseable command");
            return;
        }


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
	    os.writeInt(ID);
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
	    socket = null;
            System.err.println("Error occurs while writing command " + cmd + " to output stream: " + e.getMessage() + " in DEViseCommSocket:sendCmd()");
            //throw new YException("Error occurs while writing to output " +
	    // "stream: " + e.getMessage(), "DEViseCommSocket:sendCmd()");
        }
    }


    //-------------------------------------------------------------------
    private void doSendCmd(String[] cmd, short msgType, int ID)
      throws YException
    {
        if (os == null) {
            closeSocket();
            throw new YException("Invalid output stream",
	      "DEViseCommSocket:sendCmd()");
        }

        // for invalid cmd, simply discard it
        if (cmd == null || cmd.length == 0) {
	    System.err.println("Zero-length command");
            return;
        }

	int nelem = 0, size = 0;
        int flushedSize = 0;

        try {
            nelem = cmd.length;
            for (int i = 0; i < nelem; i++)  {
		// adding { } to each element if necessary
		if ((i != 0) && ! cmd[i].startsWith("{")) 
		    cmd[i] = "{" + cmd[i] + "}";

		// null-terminate the string for C/C++ on the other end
		// one byte
                cmd[i] = cmd[i] + "\u0000";

                // since DataOutputStream is a Byte oriented Stream, so it will write
                // each unicode character(16bits) as ISO-Latin-1 8bits character, which
                // means only the low 8bits of each unicode character is write out, and
                // the high 8bits has been throwed out.
                size = size + 2 + cmd[i].length();
            }

	    os.writeShort(msgType);
	    os.writeInt(ID);
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

                os.writeShort(cmd[i].length());
                os.writeBytes(cmd[i]);
            }

            os.flush();
        } catch (IOException e) {
            closeSocket();
	    socket = null;
            System.err.println("Error occurs while writing command " + cmd + " to output stream: " + e.getMessage() + " in DEViseCommSocket:sendCmd()");
            //throw new YException("Error occurs while writing to output " +
	    //  "stream: " + e.getMessage(), "DEViseCommSocket:sendCmd()");
        }
    }

    //-------------------------------------------------------------------
    // Receive a command.  Note that this method may be interrupted by
    // the socket timeout.  If so, it can be repeatedly called until
    // an entire command has been received.
    private String doReceiveCmd()
      throws YException, InterruptedIOException
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseCommSocket.receiveCmd()");
	}

        if (is == null) {
            closeSocket();
            throw new YException("Invalid input stream",
	      "DEViseCommSocket:receiveCmd()");
        }

        try {
            if (isControl) {
                if (dataRead == null) {
		    //TEMP -- 12 is 'magic constant' here!
                    dataRead = new byte[12];
                    numberRead = 0;
                }

		// TEMP -- try read(buf, offset, len) here
		
                //TEMP -- why don't we do readInt(), etc, here?  RKW 2001-11-07
                int b;
		//TEMP -- 12 is 'magic constant' here!
                for (int i = numberRead; i < 12; i++) {
                    b = is.read();
                    if (b < 0) {
                        closeSocket();
                        throw new YException("Abrupt end of input stream reached", "DEViseCommSocket.receiveCmd()");
                    }

                    dataRead[numberRead] = (byte)b;
                    numberRead++;
                }

		//TEMP -- "magic constants" here...
                msgType = DEViseGlobals.toUshort(dataRead);
                cmdId = DEViseGlobals.toInt(dataRead, 2);
                flag = DEViseGlobals.toUshort(dataRead, 6);
                numberOfElement = DEViseGlobals.toUshort(dataRead, 8);
                totalSize = DEViseGlobals.toUshort(dataRead, 10);

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
	    System.err.println(e.getMessage() + "DEViseCommSocket:receiveCmd()");
	    return("Connection disabled");
            //throw new YException("Error occurs while reading from input stream", "DEViseCommSocket:receiveCmd()");
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
    private void doSendData(byte[] data) throws YException
    {
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
            System.err.println("Error occurs while writing to output " +
	      "stream" + e.getMessage() + " in DEViseCommSocket:sendData()");
            throw new YException("Error occurs while writing to output " +
	      "stream" + e.getMessage(), "DEViseCommSocket:sendData()");
        }
    }

    //-------------------------------------------------------------------
    // Receive data.  This method now does not return until all of the
    // requested data has been read.
    private byte[] doReceiveData(int dataSize)
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

    //-------------------------------------------------------------------
    class SocketCreator implements Runnable
    {
	private String _host;
	private int _port;
	private int _timeout;
	private Thread _thread;
	private boolean _threadRunning = false;;
	private boolean _afterJoin = false;
	private YException _ex = null;
	private final int DEFAULT_SLEEP = 100;

        //---------------------------------------------------------------
	// This method uses a thread to allow us to put a timeout on the
	// creation of a socket.
	SocketCreator(String hostname, int port, int to, int constTO)
	  throws YException
	{
            if (DEBUG >= 2) {
	        System.out.println("DEViseCommSocket.SocketCreator " +
		  "constructor");
	    }

	    _host = hostname;
	    _port = port;
	    _timeout = to;

	    _thread = new Thread(this); 
	    _thread.start();
	    _threadRunning = true;

	    // Wait until thread finishes or constTO milliseconds have
	    // elapsed, whichever happens first.
	    myJoin(constTO);
	    _afterJoin = true;

	    if (_ex != null) {
	        throw _ex;
	    } else if (socket == null) {
		// Note: this exception is thrown here rather than at the
		// more obvious place in myJoin() in case the standard
		// Thread.join() is fixed and we use that.  RKW 2001-02-05.
	        throw new YException("Attempt to connect socket timed out; " +
		  "may be blocked by firewall");
	    } else {
	        if (DEBUG >= 2) {
		    System.out.println("Socket successfully created");
		}
	    }
	}

        //---------------------------------------------------------------
	public void run()
	{
            if (DEBUG >= 2) {
                System.out.println("DEViseCommSocket.SocketCreator.run()");
	    }

	    try {
	        createSocket(_host, _port, _timeout);
	    } catch (YException ex) {
		_ex = ex;
	    }

	    // Clean up if we somehow create the socket after the join()
	    // has timed out.  There's a little potential for a race
	    // condition here, but if it gets goofed up, finalize()
	    // will take care of it.
	    if (_afterJoin && socket != null) {
	        closeSocket();
	    }

	    _threadRunning = false;

            if (DEBUG >= 2) {
                System.out.println(
		  "  End of DEViseCommSocket.SocketCreator.run()");
	    }
	}

        //---------------------------------------------------------------
	// This method is here solely because Thread.join(int) does not
	// work correctly in Netscape (at least Netscape 4.76).  RKW
	// 2001-02-05.
	private void myJoin(int timeout)
	{
            if (DEBUG >= 2) {
                System.out.println("DEViseCommSocket.SocketCreator.myJoin(" +
		  timeout + ")");
	    }

	    Date date1 = new Date();
	    long endTime = date1.getTime() + timeout;
	    date1 = null;

	    long sleepTime = Math.min((long)timeout, (long)DEFAULT_SLEEP);

	    while (_threadRunning) {
		Date date2 = new Date();
		if (date2.getTime() > endTime) {
		    // Attempt to connect socket timed out.
		    break;
		}
		try {
	            Thread.sleep(sleepTime);
		} catch (InterruptedException ex) {
		    System.err.println("Warning: " + ex.getMessage());
		}
	    }
	}
    }
}

// ========================================================================
