// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
// Revision 1.16  1998/08/14 17:48:06  hongyu
// *** empty log message ***
//
// Revision 1.12  1998/06/11 15:07:46  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import  java.io.*;
import  java.net.*;

// Socket used to send and receive Image data between DEViseServer and DEViseClient
public class DEViseImgSocket
{
    private Socket socket = null;
    private DataInputStream is = null;
    private DataOutputStream os = null;
    //private InputStream is = null;
    //private OutputStream os = null;
    private int timeout = 0;

    // The following data are used in receiveImg to support timeout
    private byte[] rspRead = null;
    private int numRead = 0;

    // The meaning of 'id' in YException
    // id = 3, incorrect arguments
    // id = 4, io exception or other socket error, socket is closed, must reconnect

    public DEViseImgSocket(Socket sk, int to) throws YException
    {
        if (sk == null)
            throw new YException("Null socket!", "DEViseImgSocket:constructor", 3);
        else
            socket = sk;

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            os = new DataOutputStream(socket.getOutputStream());
            //os = socket.getOutputStream();
            is = new DataInputStream(socket.getInputStream());
            //is = socket.getInputStream();
            socket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for socket!", "DEViseImgSocket:constructor", 4);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open io stream!", "DEViseImgSocket:constructor", 4);
        }

        //YGlobals.Ydebugpn("Successfully connect to IMG socket!");
    }

    public DEViseImgSocket(Socket sk) throws YException
    {
        this(sk, 0);
    }

    public DEViseImgSocket(String hostname, int port, int to) throws YException
    {
        if (hostname == null || hostname.length() == 0)
            throw new YException("Null hostname!", "DEViseImgSocket:constructor", 3);

        if (port < 1024 || port > 65535)
            throw new YException("Invalid port: " + port + "!", "DEViseImgSocket:constructor", 3);

        if (to < 0)
            timeout = 0;
        else
            timeout = to;

        try {
            socket = new Socket(hostname, port);
        } catch (UnknownHostException e) {
            closeSocket();
            throw new YException("Unkonwn host: " + hostname + "!", "DEViseImgSocket:constructor", 4);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open socket on " + hostname + " at port " + port + "!", "DEViseImgSocket:constructor", 4);
        }

        try {
            os = new DataOutputStream(socket.getOutputStream());
            //os = socket.getOutputStream();
            is = new DataInputStream(socket.getInputStream());
            //is = socket.getInputStream();
            socket.setSoTimeout(timeout);
        } catch (SocketException e) {
            closeSocket();
            throw new YException("Can not set timeout for socket!", "DEViseImgSocket:constructor", 4);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not open io stream!", "DEViseImgSocket:constructor", 4);
        }

        //YGlobals.Ydebugpn("Successfully connect to IMG socket!");
    }

    public DEViseImgSocket(String hostname, int port) throws YException
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
            YGlobals.Ydebugpn("Can not close output stream at DEViseImgSocket:closeSocket");
        }

        try {
            if (is != null)
                is.close();
        }  catch (IOException e)  {
            YGlobals.Ydebugpn("Can not close input stream at DEViseImgSocket:closeSocket");
        }

        try {
            if (socket != null)
                socket.close();
        }  catch (IOException e)  {
            YGlobals.Ydebugpn("Can not close socket at DEViseImgSocket:closeSocket");
        }

        os = null;
        is = null;
        socket = null;

        //YGlobals.Ydebugpn("IMG socket is closed!");
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
            throw new YException("Can not read from input stream!", "DEViseImgSocket:isEmpty", 2);
        }
    }

    public synchronized void clearSocket() throws YException
    {
        clearBuffer();

        if (is == null) {
            closeSocket();
            throw new YException("Input stream is closed!", "DEViseImgSocket:clearSocket", 2);
        }

        try {
            socket.setSoTimeout(0);

            int availableSize = is.available();

            while (availableSize > 0) {
                byte[] tmpData = new byte[availableSize];
                is.read(tmpData, 0, availableSize);

                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                }

                availableSize = is.available();
            }

            socket.setSoTimeout(timeout);
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not read from input stream!", "DEViseImgSocket:clearSocket", 2);
        }
    }

    public synchronized void clearBuffer()
    {
        rspRead = null;
        numRead = 0;
    }

    public synchronized void sendImg(byte[] imageData) throws YException
    {
        if (os == null) {
            closeSocket();
            throw new YException("Output stream is closed!", "DEViseImgSocket:sendImg", 4);
        }

        if (imageData == null || imageData.length == 0)
            throw new YException("Null image data!", "DEViseImgSocket:sendImg", 3);

        try {
            os.write(imageData, 0, imageData.length);
            os.flush(); // I am not using BufferedOutputStream for image transfer, so
                        // this method will call its underlying outputstream's flush()
                        // method, which would be OutputStream's flush() method, which
                        // actually does nothing
            //YGlobals.Ydebugpn("Write out image data " + os.size());
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not write image data to output stream!", "DEViseImgSocket:sendImg", 4);
        }
    }

    public synchronized byte[] receiveImg(int imageSize) throws YException, InterruptedIOException
    {
        if (is == null) {
            closeSocket();
            throw new YException("Input stream is closed!", "DEViseImgSocket:receiveImg", 4);
        }

        if (imageSize <= 0) {
            YGlobals.Ydebugpn("Invalid image size: " + imageSize + " at DEViseImgSocket:receiveImg!");
            clearSocket();
            return null;
        }

        try {
            if (rspRead == null) {
                rspRead = new byte[imageSize];
                numRead = 0;
            }

            int b;
            for (int i = numRead; i < imageSize; i++) {
                b = is.read();
                if (b < 0) {
                    YGlobals.Ydebugpn("End of stream reached at " + numRead + " out of " + imageSize + " at DEViseImgSocket:receiveImg!");
                    clearSocket();
                    return null;
                }

                rspRead[numRead] = (byte)b;
                numRead++;
            }

            byte[] imageData = rspRead;
            clearBuffer();
            return imageData;
        } catch (InterruptedIOException e) {
            //YGlobals.Ydebugpn("IMG socket timeout reached at " + numRead + " of " + imageSize + "!");
            throw e;
        } catch (IOException e) {
            closeSocket();
            throw new YException("Can not receiving image data from input stream!", "DEViseImgSocket:receiveImg", 4);
        }
    }
}
