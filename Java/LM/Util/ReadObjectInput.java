/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ReadObjectInput.java
****************************************************************************/

//Packages 

package Util;

import java.lang.*;
import java.io.*;

// Class
/**
 * The <CODE>ReadObjectInput</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ReadObjectInput implements ObjectInput
{
  StreamTokenizer m_st;
  InputStreamReader m_ip;

  public ReadObjectInput(InputStream is) {
    m_ip = new InputStreamReader(is); 
    m_st = new StreamTokenizer(m_ip);
    m_st.wordChars(33,126);
    m_st.commentChar(35);
    m_st.quoteChar(34);
    m_st.quoteChar(39);
    m_st.parseNumbers();
  }
  public boolean readBoolean() throws IOException {
    int token = m_st.nextToken();
    if (m_st.ttype == StreamTokenizer.TT_WORD) {
      if (m_st.sval.equals("true")) {
        return true;
      } else if (m_st.sval.equals("false")) {
        return false;
      } 
    } else {
      throw new IOException("Error in reading boolean value");
    }
    return false;
  }
  public byte readByte() throws IOException {
    throw new IOException("Cannot read byte with ReadObjectInput");
  }
  public char readChar() throws IOException {
    int token = m_st.nextToken();
    if (m_st.ttype == StreamTokenizer.TT_WORD) {
      if (m_st.sval.length() == 1) {
        return (char)m_st.sval.charAt(0);
      }
    } else {
      throw new IOException("Error in reading char value");
    }
    return 0;
  }
  public double readDouble() throws IOException {
    int token = m_st.nextToken();
    if (m_st.ttype == StreamTokenizer.TT_NUMBER) {
      return m_st.nval;
    } else {
      throw new IOException("Error in reading double value");
    }
  }
  public float readFloat() throws IOException {
    return (float)readDouble(); 
  }
  public void readFully(byte[] b) throws IOException {
    throw new IOException("Cannot read bytes with ReadObjectInput");
  }
  public void readFully(byte[] b, int off, int len) throws IOException {
    throw new IOException("Cannot read bytes with ReadObjectInput");
  }
  public int readInt() throws IOException {
    return (int)readDouble();
  }
  public String readLine() throws IOException {
    throw new IOException("Cannot read line yet");
  }
  public long readLong() throws IOException {
    return (long)readDouble();
  }
  public short readShort() throws IOException {
    return (short)readShort();
  }
  public String readUTF() throws IOException {
    int token = m_st.nextToken();
    if (m_st.ttype == StreamTokenizer.TT_WORD) {
      return m_st.sval;
    } else {
      System.out.println("Error in reading String value " + token);
      throw new IOException("Error in reading String value " + token);
    }
  }
  public int readUnsignedByte() throws IOException {
    throw new IOException("Cannot read bytes with ReadObjectInput");
  }
  public int readUnsignedShort() throws IOException {
    throw new IOException("Cannot skip Unsigned short");
  }
  public int skipBytes(int n) throws IOException {
    throw new IOException("Cannot skip bytes yet");
  }
  public int available() throws IOException {
    throw new IOException("Not supported");
  }
  public void close() throws IOException {
    m_ip.close();    
  }
  public int read() throws IOException {
    return readInt();
  }
  public int read(byte[] b) throws IOException {
    readFully(b);
    return -1;
  }
  public int read(byte[] b, int off, int len) throws IOException {
    readFully(b, off, len);
    return -1;
  }
  public Object readObject() throws ClassNotFoundException, IOException {
    throw new IOException("Cannot use this function"); 
  }
  public long skip(long n) throws IOException {
    return skipBytes((int)n);
  }
}

