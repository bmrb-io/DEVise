/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MoreGraphics.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE>MoreGraphics</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class MoreGraphics 
{
  /* Public methods */
  public static  void drawArrow(Graphics g, 
                                int x1, int y1, int x2, int y2, int length, 
                                double delta) {
    drawArrow(g, x1, y1, x2, y2, length, delta, false);
  }
  public static  void drawArrow(Graphics g, 
                                int x1, int y1, int x2, int y2, int length, 
                                double delta, boolean doFill){
    double theta;
    if (x1 != x2) {
      double m =  Math.abs((double)(y1 - y2) /(double)(x1 - x2));
      theta = Math.atan(m);
    } else {
      theta = Math.PI / 2;
    }
    double dx1 = length * Math.cos(theta + delta);
    double dy1 = length * Math.sin(theta + delta);
    double dx2 = length * Math.cos(theta - delta);
    double dy2 = length * Math.sin(theta - delta);
    System.out.println(dx1 + " " + dx2 + " " + dy1 + " " + dy2);
    int signX = (x2 > x1) ? -1 : 1;
    int signY = (y2 > y1) ? -1 : 1;

    if (!doFill) {
      g.drawLine(x2, y2, (int)(x2 + signX * dx1), (int)(y2 + signY * dy1));
      g.drawLine(x2, y2, (int)(x2 + signX * dx2), (int)(y2 + signY * dy2));
    } else {
      int x[] = new int[] {
        x2, (int)(x2 + signX * dx1), (int)(x2 + signX * dx2), x2 
      };
      int y[] = new int[] {
        y2, (int)(y2 + signY * dy1), (int)(y2 + signY * dy2), y2
      };
      g.fillPolygon(x, y, 4);
    }
  }

  public static void drawLine(Graphics g, int x1, int y1, int x2, int y2, 
                              double semithickness) {
    if (semithickness == 0) {
      g.drawLine(x1, y1, x2, y2);
      return;
    } 
    double theta;
    if (x1 != x2) {
      double m = Math.abs((double)(y1 - y1) / (double) (x1 - x2));
      theta = Math.atan(m);
    } else {
      theta = Math.PI / 2;
    }
    double dx = semithickness * Math.sin(theta);
    double dy = semithickness * Math.cos(theta);
    System.out.println("drawLine: " + dx + " " + dy);
    int signX = (x1 < x2) ? 1 : -1;
    int x[] = new int[] {
      (int)(x1 - signX * dx), 
      (int)(x1 + signX * dx), 
      (int)(x2 + signX * dx), 
      (int)(x2 - signX * dx),
      (int)(x1 - signX * dx)
    };
    int y[] = new int[] {
      (int)(y1 + signX * dy), 
      (int)(y1 - signX * dy), 
      (int)(y2 - signX * dy),
      (int)(y2 + signX * dy), 
      (int)(y1 + signX * dy)
    };
    g.fillPolygon(x, y, 5);
  }
}
