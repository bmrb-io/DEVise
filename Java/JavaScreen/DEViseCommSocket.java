// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
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

// ========================================================================

// DEViseCommSocket.java

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
    private static final int DEFAULT_RCV_TIMEOUT = 10 * 1000; // millisec

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
            // Turning on this output causes check_connect to always fail.
            // System.out.println("DEViseCommSocket.closeSocket()");
        }

        try {
            if (os != null) {
                os.close();
            }
        } catch (IOException e) {
            System.err.println(
                "IOException in DEViseCommSocket.closeSocket(): " +
                e.getMessage());
        }

        try {
            if (is != null) {
                is.close();
            }
        } catch (IOException e) {
            System.err.println(
                "IOException in DEViseCommSocket.closeSocket(): " +
                e.getMessage());
        }

        try {
            if (socket != null) {
                socket.close();
            }
        } catch (IOException e) {
            System.err.println(
                "IOException in DEViseCommSocket.closeSocket(): " +
                e.getMessage());
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
            System.err.println("Cannot read from input stream in " +
                               "DEViseCommSocket:isEmpty(): " + e.getMessage());
            throw new YException("Cannot read from input stream",
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
                               "DEViseCommSocket:isAvailable(): " + e.getMessage());
            return false;
            //throw new YException("Cannot read from input stream",
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
                System.err.println(
                    "IOException in DEViseCommSocket.clearSocket(): " +
                    e.getMessage());
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

        String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);
        if (cmdBuffer == null || cmdBuffer.length == 0) {
            System.err.println("Unparseable command");
            return;
        }

        //synchronized (writeSync) {
        sendCmd(cmdBuffer, msgType, ID);
        //}
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
    public void sendCmd(String[] cmds, short msgType, int ID)
    throws YException
    {

        if (DEBUG >= 1) {
            //TEMP -- print all args
            System.out.println("DEViseCommSocket.sendCmd(" + cmds[0] + ", " +
                               msgType + ", " + ID + ")");
        }

        synchronized (writeSync) {
            doSendCmd(cmds, msgType, ID);
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
        String[] cmdBuffer = null;

        synchronized (readSync) {
            cmdBuffer = doReceiveCmd();
            // return doReceiveCmd();
        }

        // change from String[] format to String format.
        String response = new String("");

        for (int i=0; i<cmdBuffer.length; i++)
            response = response + cmdBuffer[i] + " ";

        return response;
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
                System.err.println("IOException " + e.getMessage() +
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

    //-------------------------------------------------------------------
    // Return the host name associated with this socket.
    public String getHostName()
    {
        GetHostName get = new GetHostName(socket);
        String hostname = get.getName();

        return hostname;
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
            System.err.println("NoRouteToHostException in " +
                               "DEViseCommSocket.createSocket: " + e.getMessage());
            closeSocket();
            throw new YException(
                "Cannot find route to host, may caused by an internal firewall",
                "DEViseCommSocket.createSocket");
        } catch (UnknownHostException e) {
            System.err.println("UnknownHostException in " +
                               "DEViseCommSocket.createSocket: " + e.getMessage());
            closeSocket();
            throw new YException("Unknown host {" + hostname + "}",
                                 "DEViseCommSocket:constructor");
        } catch (IOException e) {
            System.err.println("IOException in " +
                               "DEViseCommSocket.createSocket: " + e.getMessage());
            closeSocket();
            throw new YException("Cannot open socket connection to host {"
                                 + hostname + "}", "DEViseCommSocket.createSocket");
        } catch (Exception e) {
            System.err.println("Exception in " +
                               "DEViseCommSocket.createSocket: " + e.getMessage());
            closeSocket();
            throw new YException("Cannot create socket to host {"
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
            System.err.println("NoRouteToHostException in " +
                               "DEViseCommSocket.CreateStreams(): " + e.getMessage());
            throw new YException(
                "Cannot find route to host, may caused by an internal firewall",
                "DEViseCommSocket:constructor");
        } catch (SocketException e) {
            closeSocket();
            System.err.println("SocketException in " +
                               "DEViseCommSocket.CreateStreams(): " + e.getMessage());
            throw new YException("Cannot set timeout for sockets",
                                 "DEViseCommSocket:constructor");
        } catch (IOException e) {
            closeSocket();
            System.err.println("IOException in " +
                               "DEViseCommSocket.CreateStreams(): " + e.getMessage());
            throw new YException("Cannot open i/o stream for sockets",
                                 "DEViseCommSocket:constructor");
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
                if ((i != 0) && ! cmd[i].startsWith("{")) // } make things match
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

            //TEMP -- should this be in a little class?
            os.writeShort(msgType);
            os.writeInt(ID);
            os.writeShort(0); // not cgi

            // if nelem is greater than MAX_VALUE of short, if you use readUnsignedShort
            // on the other size, you can still get correct value
            os.writeShort(nelem);
            os.writeShort(size);
            if (DEBUG >= 5) {
                System.err.println("  msgType: " + msgType);
                System.err.println("  cmdId: " + ID);
                System.err.println("  flag: " + flag);
                System.err.println("  nelem: " + nelem);
                System.err.println("  size: " + size);
            }

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
            System.err.println("Error occurs while writing command " + cmd +
                               " to output stream: " + e.getMessage() +
                               " in DEViseCommSocket:sendCmd()");
            //throw new YException("Error occurs while writing to output " +
            //  "stream: " + e.getMessage(), "DEViseCommSocket:sendCmd()");
        }
    }

    //-------------------------------------------------------------------
    // Receive a command in String[] format.
    // Note that this method may be interrupted by
    // the socket timeout.  If so, it can be repeatedly called until
    // an entire command has been received.

    private String[] doReceiveCmd()
    throws YException, InterruptedIOException
    {
        if (DEBUG >= 1) {
            // Turning on this output causes check_connect to always fail.
            // System.out.println("DEViseCommSocket.doReceiveCmd()");
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
                        throw new YException("Abrupt end of input stream reached (1)", "DEViseCommSocket.receiveCmd()");
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
                if (DEBUG >= 5) {
                    System.err.println("  msgType: " + msgType);
                    System.err.println("  cmdId: " + cmdId);
                    System.err.println("  flag: " + flag);
                    System.err.println("  numberOfElement: " +
                                       numberOfElement);
                    System.err.println("  totalSize: " + totalSize);
                }

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
                    throw new YException("Abrupt end of input stream reached (2)",
                                         "DEViseCommSocket.receiveCmd()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }
        } catch (InterruptedIOException e) {
            throw e;
        } catch (IOException e) {
            closeSocket();
            System.err.println(e.getMessage() +
                               " in DEViseCommSocket:receiveCmd()\n");
            // Note: this is a stupid way to signal that something unusual
            // has happened; however, I'm kind of afraid to change it
            // right now.  wenger 2003-04-25.
            return(new String[] {"Connection disabled"});
            //throw new YException("Error occurs while reading from input stream", "DEViseCommSocket.receiveCmd()");
        }

        int argsize = 0;
        int pastsize = 0;
        String[] response = new String[numberOfElement];
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
            response[i] = new String(dataRead, pastsize, argsize - 1);
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
            throw new YException("Invalid output stream", "DEViseCommSocket:doSendData()");
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
                               "stream" + e.getMessage() + " in DEViseCommSocket:doSendData()");
            throw new YException("Error occurs while writing to output " +
                                 "stream" + e.getMessage(), "DEViseCommSocket:doSendData()");
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
            System.err.println("IOException in " +
                               "DEViseCommSocket.doReceiveData(): " +  e.getMessage());
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
            _thread.setName("Socket creator");
            _thread.start();
            if (DEViseGlobals.DEBUG_THREADS >= 1) {
                DEViseUtils.printAllThreads("Starting thread " + _thread);
            }
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
                throw new YException(
                    "Attempt to connect socket timed out; " +
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
            if (DEViseGlobals.DEBUG_THREADS >= 1) {
                DEViseUtils.printAllThreads("Thread " + _thread + " ending");
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

    //-------------------------------------------------------------------
    // This class is used to get the host name, with a short timeout.
    class GetHostName implements Runnable
    {
        private int MAX_TIME = 5 * 1000; // milliseconds
        private Socket _socket;
        private String _hostname = "unknown";
        private Thread _thread;
        private Thread _mainThread;

        public GetHostName(Socket socket)
        {
            if (DEBUG >= 3) {
                System.out.println("DEViseCommSocket.GetHostName.GetHostName()");
            }

            _socket = socket;
            _thread = new Thread(this);
        }

        public String getName()
        {
            if (DEBUG >= 3) {
                System.out.println("DEViseCommSocket.GetHostName.getName()");
            }
            _mainThread = Thread.currentThread();
            _thread.start();

            try {
                Thread.sleep(MAX_TIME);
            } catch (InterruptedException ex) {
                if (DEBUG >= 2) {
                    System.out.println("Sleep was interrupted in " +
                                       "GetHostName.getName()");
                }
            }

            // I'd like to destroy the new thread here, but Thread.destroy()
            // isn't implemented and Thread.stop() is deprecated.  I guess
            // we'll just leave it alone and figure it will eventually
            // end.  wenger 2003-10-28.

            return _hostname;
        }

        public void run()
        {
            if (DEBUG >= 3) {
                System.out.println("DEViseCommSocket.GetHostName.run()");
            }

            InetAddress addr = socket.getInetAddress();
            // In case getHostName fails.
            _hostname = addr.toString();
            _hostname = addr.getHostName();
            _mainThread.interrupt();
        }
    }
}

// ========================================================================
