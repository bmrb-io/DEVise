/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseGridLayout.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>DEViseGridLayout</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEViseGridLayout extends DEViseLayout
{
  /* Private members */
  private int m_rows;
  private int m_cols;
  private int m_viewsPerRow;
  private int m_viewsPerCol;
  
  /* Constructor */
  public DEViseGridLayout(int rows, int cols) {
    setGrid(rows, cols);
  }
  public DEViseGridLayout() {
    this(-1, -1);
  }
  
  /* Getters and Setters */
  public int getRows() { return m_rows; }
  public int getCols() { return m_cols; }

  /* Public methods */
  public void setGrid(int rows, int cols) { 
    m_rows = rows;
    m_cols = cols;
  }
  /* Interface implementation */
  public void mapChildren(DEViseView parent) {
    //System.out.println("mapChildren(" + parent.getName() + ")");
    int numViews = parent.numChildren(); 
    double totalHeight = parent.getHeight();
    double totalWidth = parent.getWidth();
    
    // compute views per row and col
    computeGrid(totalWidth, totalHeight, numViews);

    //System.out.println("m_viewsPerCol = " + m_viewsPerCol +
    //                  " m_viewsPerRow = " + m_viewsPerRow);
    double hRatio = 1.0/m_viewsPerCol;
    double wRatio = 1.0/m_viewsPerRow;
    double height = totalHeight/m_viewsPerCol;
    double width = totalWidth/m_viewsPerRow;
    double relX = 0; 
    double relY = 0;
    for (DListIterator l = parent.childList(); 
         l.hasMoreElements(); ) {
    //  System.out.println(" relX = " + relX + "relY = " + relY  + 
    //                     "width = " + width + " height = " + height);
      DEViseView child = (DEViseView)l.nextElement();
      child.setX(relX); 
      child.setY(relY);
      child.setWidthRatio(wRatio);
      child.setHeightRatio(hRatio);
      System.out.println(child);
      relX += width;
      if (relX >= totalWidth) {
        relX = 0;
        relY += height;
      }
    }
  }
  private boolean computeGrid(double totalWidth, double totalHeight,
                              int numViews){
    if (numViews < 1) {
      return false;
    }
    boolean useGivenRows = false;
    if ((m_rows > 0) && (m_cols > 0)) {
      if (m_rows * m_cols >= numViews) {
        m_viewsPerCol = m_rows;
        m_viewsPerRow = m_cols;
        return true;
      } else {
        useGivenRows = true;
      }
    } 

    if ((m_rows < 0) && (m_cols < 0)) { 
      // we have total control over the layout
      // if window is much taller than wide, stack views vertically
      // in a single column

      if (totalHeight > 1.5 * totalWidth) {
        m_viewsPerRow = 1;
        m_viewsPerCol = numViews; 
        return true;
      }
      // if window is much wider than tall, arrange views horizontally
      // in a single row -- because we typically display timeseries
      // data requiring more horizontal than vertical space, we have
      // a little more tendency to stack views vertically (see above)
      // than horizontally

      if (totalWidth >= 3 * totalHeight) {
        m_viewsPerCol = 1;
        m_viewsPerRow = numViews;
        return true;
      }
      
      // otherwise, arrange views so that there's roughly an equal number of
      // rows and columns; also try to make sure whole window is used
      // efficiently (no unused blocks)

      for(m_viewsPerRow = (int)Math.sqrt(numViews);
          m_viewsPerRow >= 1; m_viewsPerRow--) {
        if ((numViews % m_viewsPerRow) == 0) {
          break;
        }
      }
      m_viewsPerCol = numViews/m_viewsPerRow;
      return true;
    } 
    if (m_rows < 0) {  // auto compute rows
      m_viewsPerRow = (m_cols < numViews) ? m_cols : numViews;
      m_viewsPerCol = numViews / m_viewsPerRow;
      if ((numViews % m_viewsPerRow) != 0) {
        m_viewsPerCol++; 
      }
      return true;
    }
    if ((m_cols < 0) || useGivenRows) {  // auto compute cols
      m_viewsPerCol = (m_rows < numViews) ? m_rows : numViews;
      m_viewsPerRow = numViews / m_viewsPerCol;
      if ((numViews % m_viewsPerCol) != 0 ) {
        m_viewsPerRow++; 
      }
      return true;
    }
    return false;
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
    m_rows = stream.readInt();
    m_cols = stream.readInt();
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    stream.writeChars(m_rows + " " + m_cols);
  }
}
