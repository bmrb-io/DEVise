/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Tracer.java
****************************************************************************/

//Packages 

package Util;

import java.lang.*;

// Class
/**
 * The <CODE>Tracer</CODE> class  for debugging
 *
 * @author  Shilpa Lawande
 */

public class Tracer 
{
  public static void main(String args[]) {
    boolean out = true;
    boolean err = false;
    Tracer t = new Tracer();
    t.setOutTracer(true);
    t.setErrTracer(true);

    t.cout("1 Out");
    t.cerr("1 Err");
    t.setOutTracer(false);
    t.cout("2 Out");
    t.cerr("2 Err");
    t.setErrTracer(false);
    t.cout("3 Out");
    t.cerr("3 Err");
    t.setOutTracer(true);
    t.cout("4 Out");
    t.cerr("4 Err");
    t.setErrTracer(true);
    t.cout("5 Out");
    t.cerr("5 Err");
  }
  static boolean m_globalOutON = true;
  static boolean m_globalErrON = true;
  public static boolean stdout() {
    return m_globalOutON;
  }
  public static boolean stderr() {
    return m_globalErrON;
  }
  public static void setStdout(boolean state) {
    m_globalOutON = true;
  }
  public static void setStderr(boolean state) {
    m_globalErrON = true;
  }
  private boolean m_traceOutON; 
  private boolean m_traceErrON;
  /* Constructor */
  public Tracer() {
    m_traceErrON = true;
    m_traceOutON = true;
  }
  public void setOutTracer(boolean state) {
    m_traceOutON = state;
  }
  public void setErrTracer(boolean state) {
    m_traceErrON = state;
  }
  public boolean outon() {
    return m_traceOutON;
  }
  public boolean erron() {
    return m_traceErrON;
  }
  public void cout(Object obj) {
    if (m_globalOutON && m_traceOutON) {
      System.out.println(obj);
    }
  }
  public void cout(String s) {
    if (m_globalOutON && m_traceOutON) {
      System.out.println(s);
    }
  }
  public void cout(char x[]) {
     if (m_globalOutON && m_traceOutON) {
      System.out.println(x);
    }
  }
  public void cerr(Object obj) {
    if (m_globalErrON && m_traceErrON) {
      System.err.println(obj);
    }
  }
  public void cerr(String s) {
    if (m_globalErrON && m_traceErrON) {
      System.err.println(s);
    }
  }
  public void cerr(char x[]) {
     if (m_globalErrON && m_traceErrON) {
      System.err.println(x);
    }
  }
}
