package gjt.rubberband;

import java.awt.*;
import java.awt.event.*;

/** 
 * An abstract base class for rubberbands.<p>
 *
 * Rubberbands do their rubberbanding inside of a Component, 
 * which must be specified at construction time.<p>
 * 
 * Subclasses are responsible for implementing 
 * <em>void drawLast(Graphics g)</em> and 
 * <em>void drawNext(Graphics g)</em>.  
 *
 * drawLast() draws the appropriate geometric shape at the last 
 * rubberband location, while drawNext() draws the appropriate 
 * geometric shape at the next rubberband location.  All of the 
 * underlying support for rubberbanding is taken care of here, 
 * including handling XOR mode setting; extensions of Rubberband
 * need not concern themselves with anything but drawing the 
 * last and next geometric shapes.  Extensions may get information
 * about where to draw their shape from getAnchor(), getStretched(),
 * and getLast(), and getEnd(), all of which return a Point.<p>
 *
 * @version 1.0, Dec 27 1995
 * @version 1.1, Nov 8  1996
 *
 *    Changed names of instance variables with same names as 
 *    methods to work around bug in Microsoft compiler.
 *
 * @author  David Geary
 * @see     RubberbandLine
 * @see     RubberbandRectangle
 * @see     RubberbandEllipse
 * @see     gjt.test.RubberbandTest
 */
abstract public class Rubberband {
    protected Point anchorPt    = new Point(0,0); 
    protected Point stretchedPt = new Point(0,0);
    protected Point lastPt      = new Point(0,0); 
    protected Point endPt       = new Point(0,0);

    private Component component;
    private boolean   firstStretch = true;
	private boolean   active = false;

    abstract public void drawLast(Graphics g);
    abstract public void drawNext(Graphics g);

	public Rubberband() {
	}
	public Rubberband(Component c) {
		setComponent(c);
	}
	public void setActive(boolean b) {
		active = b;
	}
	public void setComponent(Component c) { 
		component = c; 

		component.addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent event) {	
				if(isActive()) {
					anchor(event.getPoint());
				}
			}
			public void mouseClicked(MouseEvent event) {
				if(isActive())
					end(event.getPoint());
			}
			public void mouseReleased(MouseEvent event) {
				if(isActive())
					end(event.getPoint());
			}
		});
		component.addMouseMotionListener(new MouseMotionAdapter() {
			public void mouseDragged(MouseEvent event) {
				if(isActive())
					stretch(event.getPoint());
			}
		});
	}
	public boolean isActive    () { return active;      }
    public Point   getAnchor   () { return anchorPt;    }
    public Point   getStretched() { return stretchedPt; }
    public Point   getLast     () { return lastPt;      }
    public Point   getEnd      () { return endPt;       }

    public void anchor(Point p) {
        firstStretch = true;
        anchorPt.x = p.x;
        anchorPt.y = p.y;

        stretchedPt.x = lastPt.x = anchorPt.x;
        stretchedPt.y = lastPt.y = anchorPt.y;
    }
    public void stretch(Point p) {
        lastPt.x      = stretchedPt.x;
        lastPt.y      = stretchedPt.y;
        stretchedPt.x = p.x;
        stretchedPt.y = p.y;

        Graphics g = component.getGraphics();
        if(g != null) {
            g.setXORMode(component.getBackground());

            if(firstStretch == true) firstStretch = false;
            else                     drawLast(g);

            drawNext(g);
			g.dispose();
        }
    }
    public void end(Point p) {
        lastPt.x = endPt.x = p.x;
        lastPt.y = endPt.y = p.y;

        Graphics g = component.getGraphics();
        if(g != null) {
            g.setXORMode(component.getBackground());
            drawLast(g);
			g.dispose();
        }
    }
    public Rectangle getBounds() {
      return new Rectangle(stretchedPt.x < anchorPt.x ? 
                           stretchedPt.x : anchorPt.x,
                           stretchedPt.y < anchorPt.y ? 
                           stretchedPt.y : anchorPt.y,
                           Math.abs(stretchedPt.x - anchorPt.x),
                           Math.abs(stretchedPt.y - anchorPt.y));
    }

    public Rectangle lastBounds() {
      return new Rectangle(
                  lastPt.x < anchorPt.x ? lastPt.x : anchorPt.x,
                  lastPt.y < anchorPt.y ? lastPt.y : anchorPt.y,
                  Math.abs(lastPt.x - anchorPt.x),
                  Math.abs(lastPt.y - anchorPt.y));
    }
}
