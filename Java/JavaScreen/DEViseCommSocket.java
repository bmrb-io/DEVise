// DEViseCommSocket.java
// last updated on 04/13/99

import  java.io.*;
import  java.net.*;

public class DEViseCommSocket
{
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;

    private Socket imgSocket = null;
    private DataInputStream imgis = null;
    private DataOutputStream imgos = null;

    // timeout = 0 means no time out
    private int timeout = 0;
    private int checkingSocketTimeout = 100;
    
    // the buffer size for BufferedOutputStream, JDK defaults is 512
    private int bufferSize = 512;

    // The following data are used in receiveCmd() and receiveData() to support timeout
    private boolean isControl = true;
    private int controlFlag = 0, numberOfElement = 0, totalSize = 0;
    private byte[] dataRead = null;
    private int numberRead = 0; 
    

    public DEViseCommSocket(Socket sk, Socket sk1, int to) throws YException
    {
        if (sk == null || sk1 == null) {
            throw new YException("Invalid socket in arguments", "DEViseCommSocket:constructor");
        } else {
            socket = sk;
            imgSocket = sk1;
        }

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            os = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream(), bufferSize));
            is = new DataInputStream(socket.getInputStream());
            socket.setSoTimeout(timeout);

            imgos = new DataOutputStream(imgSocket.getOutputStream());
            imgis = new DataInputStream(imgSocket.getInputStream());
            imgSocket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for sockets", "DEViseCommSocket:constructor");
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open i/o stream for sockets", "DEViseCommSocket:constructor");
        }
    }

    public DEViseCommSocket(Socket sk, Socket sk1) throws YException
    {
        // default time out is 1 second
        this(sk, sk1, 1000);
    }

    public DEViseCommSocket(String hostname, int cmdport, int imgport, int to) throws YException
    {
        if (hostname == null)
            throw new YException("Invalid hostname in arguments", "DEViseCommSocket:constructor");

        if (cmdport < 1024 || cmdport > 65535 || imgport < 1024 || imgport > 65535)
            throw new YException("Invalid port number in arguments", "DEViseCommSocket:constructor");

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            socket = new Socket(hostname, cmdport);
            imgSocket = new Socket(hostname, imgport);
        } catch (UnknownHostException e) {
            closeSocket();
            throw new YException("Unkonwn host {" + hostname + "}", "DEViseCommSocket:constructor");
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open socket connection to host {" + hostname + "}", "DEViseCommSocket:constructor");
        }

        try {
            os = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream(), bufferSize));
            is = new DataInputStream(socket.getInputStream());
            socket.setSoTimeout(timeout);

            imgos = new DataOutputStream(imgSocket.getOutputStream());
            imgis = new DataInputStream(imgSocket.getInputStream());
            imgSocket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for sockets", "DEViseCommSocket:constructor");
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open i/o stream for sockets", "DEViseCommSocket:constructor");
        }
    }

    public DEViseCommSocket(String hostname, int cmdport, int imgport) throws YException
    {
        // default time out is 1 second
        this(hostname, cmdport, imgport, 1000);
    }

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

        try {
            if (imgos != null)
                imgos.close();
        } catch (IOException e) {
        }

        try {
            if (imgis != null)
                imgis.close();
        } catch (IOException e) {
        }

        try {
            if (imgSocket != null)
                imgSocket.close();
        } catch (IOException e) {
        }

        os = null;
        is = null;
        socket = null;
        imgos = null;
        imgis = null;
        imgSocket = null;
    }

    // if at the moment of calling, there are something coming from the input stream,
    // isEmpty will return false, otherwise, it will return true
    public synchronized boolean isEmpty() throws YException
    {
        try {
            if (is == null && imgis == null)
                return true;
                
            if ((is != null && is.available() > 0) || (imgis != null && imgis.available() > 0)) {
                return false;
            } else { 
                return true;
                
                /*
                try {
                    Thread.sleep(checkingSocketTimeout);
                } catch (InterruptedException e1) {
                }
                
                // give it a second chance
                if ((is != null && is.available() > 0) || (imgis != null && imgis.available() > 0)) {
                    return false;
                } else {
                    return true;
                }
                */            
            }
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream", "DEViseCommSocket:isEmpty()");
        }
    }

    public synchronized void clearSocket() throws YException
    {
        resetData();

        try {
            if (socket!= null && is != null) {
                socket.setSoTimeout(0);

                int size = is.available();
                boolean isEnd = false;
                while (size > 0 || !isEnd) {
                    if (size <= 0) {
                        try {
                            Thread.sleep(checkingSocketTimeout);
                        } catch (InterruptedException e1) {
                        }
                        isEnd = true;
                    } else {                    
                        byte[] tmpdat = new byte[size];
                        is.readFully(tmpdat);
                    }
                    
                    size = is.available();
                }

                socket.setSoTimeout(timeout);
            }

            if (imgSocket!= null && imgis != null) {
                imgSocket.setSoTimeout(0);

                int size = imgis.available();
                boolean isEnd = false;
                while (size > 0 || !isEnd) {
                    if (size <= 0) {
                        try {
                            Thread.sleep(checkingSocketTimeout);
                        } catch (InterruptedException e1) {
                        }
                        isEnd = true;
                    } else {                        
                        byte[] tmpdat = new byte[size];
                        imgis.readFully(tmpdat);
                    }
                    
                    size = imgis.available();
                }

                imgSocket.setSoTimeout(timeout);
            }
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while reading from input stream", "DEViseCommSocket:clearSocket()");
        }
    }

    private synchronized void resetData()
    {
        isControl = true;
        controlFlag = 0;
        numberOfElement = 0;
        totalSize = 0;
        dataRead = null;
        numberRead = 0;
    }

    public synchronized void sendCmd(String cmd, short flag) throws YException
    {
        if (os == null) {
            closeSocket();
            throw new YException("Invalid output stream", "DEViseCommSocket:sendCmd()");
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
            throw new YException("Error occurs while writing to output stream", "DEViseCommSocket:sendCmd()");
        }
    }

    public void sendCmd(String cmd) throws YException
    {
        sendCmd(cmd, DEViseGlobals.API_JAVA);
    }

    public synchronized String receiveCmd() throws YException, InterruptedIOException
    {
        if (is == null) {
            closeSocket();
            throw new YException("Invalid input stream", "DEViseCommSocket:receiveCmd()");
        }

        try {
            if (isControl) {
                if (dataRead == null) {
                    dataRead = new byte[6];
                    numberRead = 0;
                }

                int b;
                for (int i = numberRead; i < 6; i++) {
                    b = is.read();
                    if (b < 0) {
                        closeSocket();
                        throw new YException("Abrupt end of input stream reached", "DEViseCommSocket.receiveCmd()");
                    }

                    dataRead[numberRead] = (byte)b;
                    numberRead++;
                }

                controlFlag = DEViseGlobals.toUshort(dataRead);
                numberOfElement = DEViseGlobals.toUshort(dataRead, 2);
                totalSize = DEViseGlobals.toUshort(dataRead, 4);

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

            int b;
            for (int i = numberRead; i < totalSize; i++) {
                b = is.read();
                if (b < 0) {
                    closeSocket();
                    throw new YException("Abrupt end of input stream reached", "DEViseCommSocket.receiveCmd()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }
        } catch (InterruptedIOException e) {
            throw e;
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while reading from input stream", "DEViseCommSocket:receiveCmd()");
        }

        int argsize = 0;
        int pastsize = 0;
        String response = new String("");
        for (int i = 0; i < numberOfElement; i++) {
            if (totalSize < pastsize + 2) {
                closeSocket();
                throw new YException("Inconsistant data received", "DEViseCommSocket:receiveCmd()");
            }

            argsize = DEViseGlobals.toUshort(dataRead, pastsize);
            pastsize += 2;

            if (totalSize < pastsize + argsize) {
                closeSocket();
                throw new YException("Inconsistant data received", "DEViseCommSocket:receiveCmd()");
            }

            // use argsize - 1 instead of argsize is to skip the string ending '\0'
            // use one space to seperate different parts of the response command
            response += new String(dataRead, pastsize, argsize - 1) + " ";
            pastsize += argsize;
        }

        resetData();

        return response;
    }

    public synchronized void sendData(byte[] data) throws YException
    {
        if (imgos == null) {
            closeSocket();
            throw new YException("Invalid output stream", "DEViseCommSocket:sendData()");
        }

        // simply give up if the input data is invalid
        if (data == null || data.length == 0)
            return;

        try {
            imgos.write(data, 0, data.length);
            imgos.flush(); // I am not using BufferedOutputStream for data transfer, so
                           // this method will call its underlying outputstream's flush()
                           // method, which would be OutputStream's flush() method, which
                           // actually does nothing
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while writing to output stream", "DEViseCommSocket:sendData()");
        }
    }

    public synchronized byte[] receiveData(int dataSize) throws YException, InterruptedIOException
    {
        if (imgis == null) {
            closeSocket();
            throw new YException("Invalid input stream", "DEViseCommSocket:receiveData()");
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

            int b;
            for (int i = numberRead; i < dataSize; i++) {
                b = imgis.read();
                if (b < 0) {
                    closeSocket();
                    throw new YException("Abrupt end of input stream reached", "DEViseCommSocket:receiveData()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }

            byte[] data = dataRead;

            resetData();

            return data;
        } catch (InterruptedIOException e) {
            throw e;
        } catch (IOException e) {
            closeSocket();
            throw new YException("Error occurs while reading from input stream", "DEViseCommSocket:receiveData()");
        }
    }
}
