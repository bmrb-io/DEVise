// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class handles the low-level details of client/JSPoP communication
// in CGI mode.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2001/02/23 19:56:42  wenger
// Moved the details of client-side CGI communication into its own class;
// combined separate socket and CGI code that largely duplicated each other.
//

// ========================================================================

// DEViseCommCgi.java

import  java.io.*;
import  java.net.*;

public class DEViseCommCgi
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private boolean _cmdSent = false;
    private boolean _responseReceived = false;

    private DEViseJSValues _values = null;

    private URLConnection _cgiConn = null;
    private DataOutputStream _cgiOutput = null;
    private DataInputStream _cgiInput = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseCommCgi(DEViseJSValues values)
      throws YException
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseCommCgi.DEViseCommCgi()");
	}

	try {
            URL cgiURL = new URL("http", values.connection.hostname,
	      values.connection.cgiURL);
            _cgiConn = cgiURL.openConnection();
	    _cgiConn.setDoOutput(true);
        } catch(MalformedURLException ex) {
	    throw new YException("Badly formed URL: " + ex.getMessage());
        } catch(IOException ex) {
	    throw new YException("Failed I/O: " + ex.getMessage());
        }

	_values = values;
    }

    //-------------------------------------------------------------------
    public void finalize()
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseCommCgi.finalize()");
	}

	if (_cmdSent && !_responseReceived) {
	    System.err.println("CGI command was sent, but response was " +
	      "not recevied.  This means that the CGI script will not be " +
	      "run properly");
	}
    }

    //-------------------------------------------------------------------
    public synchronized void sendCmd(String cmd) throws YException
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseCommCgi.sendCmd(" + cmd + ")");
	}

	_cmdSent = true;

        try {
	    _cgiOutput = new DataOutputStream(_cgiConn.getOutputStream());

	    String[] cmdBuffer = DEViseGlobals.parseString(cmd, true);

	    // nelem is the number of "elements" (arguments); size is
	    // the total size of the arguments plus the length value
	    // for each argument.
	    int nelem, size = 0;

	    nelem = cmdBuffer.length;
            
	    for (int i = 0; i < nelem; i++)  {
		// Note: for some reason, having nulls in the string
		// sent to the CGI script causes problems when the JS
		// is run as an applet (but not when it's run as an
		// application), even though the CGI script seems to
		// receive everything correctly.  RKW 2000-12-21.

		//TEMPTEMP -- appending a null (how it was previously) goofs
		// up CGI mode when running in netscape; not appending
		// anything totally goofs things up.  RKW 2000-12-21.
		cmdBuffer[i] = cmdBuffer[i] + "_";//TEMPTEMP
		// Note: the "2" here is for the length value (short).
                size = size + 2 + cmdBuffer[i].length();
            }

	    _cgiOutput.writeBytes("cmdport=" + _values.connection.cmdport);
	    _cgiOutput.writeBytes("&msgtype=" + DEViseGlobals.API_JAVA_WID);
	    _cgiOutput.writeBytes("&id=" + _values.connection.connectionID);
	    _cgiOutput.writeBytes("&nelem=" + nelem);
	    _cgiOutput.writeBytes("&size=" + size);
	    
            for (int i = 0; i < nelem; i++) {
                _cgiOutput.writeBytes("&len" + i + "=" + cmdBuffer[i].length());
		_cgiOutput.writeBytes("&arg" + i + "=" + cmdBuffer[i]);
            }
	    
	    _cgiOutput.flush();
        } catch(IOException ex) {
	    throw new YException("Failed I/O: " + ex.getMessage());
        }
    }

    //-------------------------------------------------------------------
    public synchronized String receiveCmd() throws YException,
      InterruptedIOException
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseCommCgi.receiveCmd()");
	}

	_responseReceived = true;

        int msgType = 0, numberOfElement = 0, totalSize = 0;
	int cmdId = 0, cgiFlag = 0;
        byte[] dataRead = null;
        int numberRead = 0;
        
      	try {
	     // Note: the input stream MUST NOT be constructed until after
	     // the command is sent (I don't know why).  RKW 2001-02-23.
	     _cgiInput = new DataInputStream(_cgiConn.getInputStream());

	     if (dataRead == null) {
                 dataRead = new byte[10];
                 numberRead = 0;
             }

             int b;
             for (int i = numberRead; i < 10; i++) {
		 b = _cgiInput.read();
                 if (b < 0) {
		     //TEMP -- is this safe?
		     return "cgi no response";
		     //throw new YException(
		     //"Abrupt end of input stream reached",
		     //"cgi_receiveCmd()");
                 }

                 dataRead[numberRead] = (byte)b;
                 numberRead++;
             }

             msgType = DEViseGlobals.toUshort(dataRead);
	     cmdId = DEViseGlobals.toUshort(dataRead, 2);
	     cgiFlag = DEViseGlobals.toUshort(dataRead, 4);
             numberOfElement = DEViseGlobals.toUshort(dataRead, 6);
             totalSize = DEViseGlobals.toUshort(dataRead, 8);

             dataRead = null;

             if (numberOfElement <= 0 || totalSize <= 0) { 
		 throw new YException(
		   "Invalid control information received", "cgi_receiveCmd()");
             }
            
             if (dataRead == null) {
                dataRead = new byte[totalSize];
                numberRead = 0;
             }

             for (int i = numberRead; i < totalSize; i++) {
                b = _cgiInput.read();

                if (b < 0) {
		    throw new YException(
		      "Abrupt end of input stream reached",
		      "cgi_receiveCmd()");
                }

                dataRead[numberRead] = (byte)b;
                numberRead++;
            }
        } catch (InterruptedIOException e) {
	    System.err.println("InterruptedIOException in cgi_receiveCmd: " +
	      e.getMessage());
            throw e;
        } catch (IOException e) {
	    System.err.println("IOException in cgi_receiveCmd: " +
	      e.getMessage());
            throw new YException("Error occurs while reading from " +
	      "input stream in cgi_receiveCmd()", "cgi_receiveCmd()");
        }

        int argsize = 0;
        int pastsize = 0;
        String response = new String("");
        for (int i = 0; i < numberOfElement; i++) {
            if (totalSize < pastsize + 2) {
                throw new YException("Inconsistant data received",
		  "cgi_receiveCmd()");
            }

            argsize = DEViseGlobals.toUshort(dataRead, pastsize);
            pastsize += 2;

            if (totalSize < pastsize + argsize) {
                throw new YException("Inconsistant data received",
		  "cgi_receiveCmd()");
            }

            // use argsize - 1 instead of argsize is to skip the string ending '\0'
            // use one space to seperate different parts of the response command
            response += new String(dataRead, pastsize, argsize - 1) + " ";
            pastsize += argsize;
        }

        // resetData();

        return response;
    }

    //-------------------------------------------------------------------
    public synchronized byte[] receiveData(int size) throws YException
    {
        if (DEBUG >= 1) {
	    System.out.println("DEViseCommCgi.receiveData(" + size + ")");
	}

        byte[] imgData = null;
        byte[] dataRead = null;
        int numberRead = 0;

        // simply give up receiving if input size is invalid
        if (size <= 0) {
            return null;
        }

        try {
            if (dataRead == null) {
                dataRead = new byte[size];
                numberRead = 0;
            }

            int offset = 0;
	    int bytesRemaining = size;
	    while (bytesRemaining > 0) {
		try {
	            int bytesRead = _cgiInput.read(dataRead, offset,
		      bytesRemaining);
		    offset += bytesRead;
		    bytesRemaining -= bytesRead;
		} catch(InterruptedIOException e) {
		    // Note: this may happen if the data transfer is too slow.
		    _values.debug.log("Error reading data: " +
		      e.getMessage());
		    try {
		        Thread.sleep(100); // wait for some more data on socket
		    } catch (InterruptedException ex) {
		    }
		}
	    }

	    imgData = dataRead;

        } catch (IOException e) {
            throw new YException(
	      "Error occurs while reading from input stream",
	      "DEViseCommCgi.receiveData()");
        }

        if (imgData == null) {
            throw new YException("Invalid response received from CGI",
	      "DEViseCommCgi.receiveData()", 1);
        }

        return imgData;
    }
}

// ========================================================================
