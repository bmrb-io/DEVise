/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class PrintObjectOutput.java
****************************************************************************/

//Packages 

package Util;

import java.lang.*;
import java.io.*;

// Class
/**
 * The <CODE>PrintObjectOutput</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class PrintObjectOutput implements ObjectOutput
{
  private PrintWriter m_pw;
  public PrintObjectOutput(PrintWriter pw) {
    m_pw = pw;
  }
  public PrintObjectOutput(OutputStream os) {
    this(new PrintWriter(os));
  }
  public void write(int i) throws IOException {
    m_pw.print(i);
  }
  public void write(byte[] b) throws IOException {
    throw new IOException("Cannot output byte in text form");
  }
  public void write(byte[] b, int off, int len) throws IOException {
    throw new IOException("Cannot output bytes in text form");
  }
  public void writeBoolean(boolean v) throws IOException {
    m_pw.print(v);
  }
  public void writeByte(int v) throws IOException {
    throw new IOException("Cannot output byte in text form");
  }
  public void writeBytes(String s) throws IOException {
    m_pw.print(s);
  }
  public void writeChar(int v) throws IOException {
    m_pw.print(v);
  }
  public void writeChars(String s) throws IOException {
    m_pw.print(s);
  }
  public void writeDouble(double d) throws IOException {
    m_pw.print(d);
  }
  public void writeFloat(float f) throws IOException {
    m_pw.print(f);
  }
  public void writeInt(int i) throws IOException {
    m_pw.print(i);
  }
  public void writeLong(long l) throws IOException {
    m_pw.print(l);
  }
  public void writeShort(int s) throws IOException {
    m_pw.print(s);
  }
  public void writeUTF(String str) throws IOException {
    m_pw.print(str);
  }
  public void writeObject(Object obj) throws IOException, NotSerializableException {
    if (!(obj instanceof Externalizable)) {
      throw new NotSerializableException(obj.getClass().getName());
    }
    ((Externalizable)obj).writeExternal(this);
  }
  public void close() {
    m_pw.close();
  }
  public void flush() {
    m_pw.flush();
  }
}
