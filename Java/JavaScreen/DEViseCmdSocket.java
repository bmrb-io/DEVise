import  java.io.*;
import  java.net.*;

// Socket used to send DEVise API commands and receive response between
// DEViseServer and DEViseClient
public class DEViseCmdSocket
{
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
    private int bufferSize = 1024; // The java default is 512 bytes
    private int timeout = 0;

    // The following data are used in receiveRsp to support timeout
    private boolean isFlag = false;
    private int rspFlag = 0, rspNelem = 0, rspSize = 0;
    private byte[] rspRead = null;
    // The number of bytes read so far
    private int numRead = 0;

    // The meaning of 'id' in YException
    // id = 1, incorrect arguments
    // id = 2, io exception or other socket error, socket is closed, must reconnect

    public DEViseCmdSocket(Socket sk, int to) throws YException
    {
        if (sk == null)
            throw new YException("Null socket!", "DEViseCmdSocket:constructor", 1);
        else
            socket = sk;

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            os = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream(), bufferSize));
            is = new DataInputStream(socket.getInputStream());
            socket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for socket!", "DEViseCmdSocket:constructor", 2);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open io stream!", "DEViseCmdSocket:constructor", 2);
        }

        //YGlobals.Ydebugpn("Successfully connect to CMD socket!");
    }

    public DEViseCmdSocket(Socket sk) throws YException
    {
        this(sk, 0);
    }

    public DEViseCmdSocket(String hostname, int port, int to) throws YException
    {
        if (hostname == null)
            throw new YException("Null hostname!", "DEViseCmdSocket:constructor", 1);

        if (port < 1024 || port > 65535)
            throw new YException("Invalid port number: " + port + "!", "DEViseCmdSocket:constructor", 1);

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            socket = new Socket(hostname, port);
        } catch (UnknownHostException e) {
            closeSocket();
            throw new YException("Unkonwn host: " + hostname + "!", "DEViseCmdSocket:constructor", 2);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open socket on " + hostname + " at port " + port + "!", "DEViseCmdSocket:constructor", 2);
        }

        try {
            os = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream(), bufferSize));
            is = new DataInputStream(socket.getInputStream());
            socket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for socket!", "DEViseCmdSocket:constructor", 2);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open io stream!", "DEViseCmdSocket:constructor", 2);
        }

        //YGlobals.Ydebugpn("Successfully connect to CMD socket!");
    }

    public DEViseCmdSocket(String hostname, int port) throws YException
    {
        this(hostname, port, 0);
    }

    public synchronized void closeSocket()
    {
        if (os == null && is == null && socket == null) // socket already closed
            return;

        clearBuffer();

        try  {
            if (os != null)
                os.close();
        }  catch (IOException e)  {
            YGlobals.Ydebugpn("Can not close output stream at DEViseCmdSocket:closeSocket!");
        }

        try {
            if (is != null)
                is.close();
        }  catch (IOException e)  {
            YGlobals.Ydebugpn("Can not close input stream at DEViseCmdSocket:closeSocket!");
        }

        try {
            if (socket != null)
                socket.close();
        }  catch (IOException e)  {
            YGlobals.Ydebugpn("Can not close socket at DEViseCmdSocket:closeSocket!");
        }

        os = null;
        is = null;
        socket = null;

        //YGlobals.Ydebugpn("CMD socket is closed!");
    }

    // if at the moment of calling, there are something coming from the input stream,
    // isEmpty will return false, otherwise, it will return true.
    public synchronized boolean isEmpty() throws YException
    {
        try {
            if (is.available() > 0)
                return false;
            else
                return true;
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream!", "DEViseCmdSocket:isEmpty", 2);
        }
    }

    public synchronized void clearSocket() throws YException
    {
        clearBuffer();

        if (is == null) {
            closeSocket();
            throw new YException("Input stream is closed!", "DEViseCmdSocket:clearSocket", 2);
        }

        try {
            socket.setSoTimeout(0);

            int availableSize = is.available();

            while (availableSize > 0) {
                byte[] tmpData = new byte[availableSize];
                is.readFully(tmpData);

                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                }

                availableSize = is.available();
            }

            socket.setSoTimeout(timeout);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream!", "DEViseCmdSocket:clearSocket", 2);
        }
    }

    private synchronized void clearBuffer()
    {
        isFlag = false;
        rspFlag = 0;
        rspNelem = 0;
        rspSize = 0;
        rspRead = null;
        numRead = 0;
    }

    public synchronized void sendCmd(String cmd, short flag) throws YException
    {
        if (os == null) {
            closeSocket();
            throw new YException("Output stream is closed!", "DEViseCmdSocket:sendCmd", 2);
        }

        if (cmd == null || cmd.length() == 0)
            throw new YException("Null command!", "DEViseCmdSocket:sendCmd", 1);

        String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);
        if (cmdBuffer == null || cmdBuffer.length == 0)
            throw new YException("Invalid command: " + cmd + "!", "DEViseCmdSocket:sendCmd", 1);

        int nelem = 0, size = 0;
        int flushedSize = 0;

        YGlobals.Ydebugpn("Sending: " + cmd);

        try {
            nelem = cmdBuffer.length;
            for (int i = 0; i < nelem; i++)  {
                cmdBuffer[i] = cmdBuffer[i] + "\u0000";
                // since DataOutputStream is a Byte oriented Stream, so it will write
                // each unicode character(16bits) as ISO-Latin-1 8bits character, which
                // means only the low 8bits of each unicode character is write out, and
                // the high 8bits has been throwed out.
                size = size + 2 + cmdBuffer[i].length();
            }

            os.writeShort(flag);
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
            throw new YException("Can not write command " + cmd + " to output stream!", "DEViseCmdSocket:sendCmd", 2);
        }
    }

    public void sendCmd(String cmd) throws YException
    {
        sendCmd(cmd, DEViseGlobals.API_JAVA);
    }

    public synchronized String receiveRsp() throws YException, InterruptedIOException
    {
        if (is == null) {
            closeSocket();
            throw new YException("Input stream is closed!", "DEViseCmdSocket:receiveRsp", 2);
        }

        try {
            if (!isFlag) {
                if (rspRead == null) {
                    rspRead = new byte[6];
                    numRead = 0;
                }

                int b;
                for (int i = numRead; i < 6; i++) {
                    b = is.read();
                    if (b < 0) {
                        YGlobals.Ydebugpn("End of stream reached at " + numRead + " out of 6 at DEViseCmdSocket.receiveRsp!");
                        clearSocket();
                        return null;
                    }

                    rspRead[numRead] = (byte)b;
                    numRead++;
                }

                rspFlag = YGlobals.YtoUshort(rspRead);
                rspNelem = YGlobals.YtoUshort(rspRead, 2);
                rspSize = YGlobals.YtoUshort(rspRead, 4);
                rspRead = null;
                isFlag = true;

                if (rspNelem <= 0 || rspSize <= 0) {
                    YGlobals.Ydebugpn("Invalid rspSize " + rspSize + " or number Of Element " + rspNelem + " at DEViseCmdSocket:receiveRsp!");
                    clearSocket();
                    return null;
                }
            }

            if (rspRead == null) {
                rspRead = new byte[rspSize];
                numRead = 0;
            }

            int b;
            for (int i = numRead; i < rspSize; i++) {
                b = is.read();
                if (b < 0) {
                    YGlobals.Ydebugpn("End of stream reached at " + numRead + " out of " + rspSize + " at DEViseCmdSocket.receiveRsp!");
                    clearSocket();
                    return null;
                }

                rspRead[numRead] = (byte)b;
                numRead++;
            }
        } catch (InterruptedIOException e) {
            //YGlobals.Ydebugpn("CMD socket time out reached at " + numRead + " of " + rspSize + "!");
            throw e;
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream!", "DEViseCmdSocket:receiveRsp", 2);
        }

        int argsize = 0;
        int pastsize = 0;
        String response = new String("");
        for (int i = 0; i < rspNelem; i++) {
            if (rspSize < pastsize + 2) {
                clearBuffer();
                YGlobals.Ydebugpn("Incorrect rspSize " + rspSize + " at DEViseCmdSocket:receiveRsp!");
                return null;
            }

            argsize = YGlobals.YtoUshort(rspRead, pastsize);
            pastsize += 2;

            if (rspSize < pastsize + argsize) {
                clearBuffer();
                YGlobals.Ydebugpn("Incorrect rspSize " + rspSize + " at DEViseCmdSocket:receiveRsp!");
                return null;
            }

            // use argsize - 1 instead of argsize is to skip the string ending '\0'
            // use one space to seperate different parts of the response command
            response += new String(rspRead, pastsize, argsize - 1) + " ";
            pastsize += argsize;
        }

        clearBuffer();

        YGlobals.Ydebugpn("Receiving: " + response);

        return response;
    }
}
