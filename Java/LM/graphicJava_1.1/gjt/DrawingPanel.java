package gjt;

import java.awt.*;
import java.awt.event.*;
import gjt.rubberband.*;

/**
 * An extension of gjt.rubberband.RubberbandPanel which serves
 * as a panel used for drawing simple shapes (lines, rectangles,
 * and ellipses).  The shapes may be filled (except for lines,
 * of course), and the color of the shapes may be specified.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.rubberband.RubberbandPanel
 * @see     gjt.rubberband.RubberbandEllipse
 * @see     gjt.rubberband.RubberbandLine
 * @see     gjt.rubberband.RubberbandRectangle
 * @see     gjt.test.RubberbandTest
 * @see     gjt.test.ToolbarTest
 */
public class DrawingPanel extends RubberbandPanel {
	private Rubberband rbLine, rbRect, rbEllipse;
	private Color   color;
	private boolean fill;

	public DrawingPanel() {
		rbLine    = new RubberbandLine     ();
		rbRect    = new RubberbandRectangle();
		rbEllipse = new RubberbandEllipse  ();

		setRubberband(rbLine);
	}
	public void rubberbandEnded(Rubberband rubberband) {
		drawShape(rubberband);
	}
	public void drawLines     () { setRubberband(rbLine);    } 
	public void drawRectangles() { setRubberband(rbRect);    } 
	public void drawEllipses  () { setRubberband(rbEllipse); } 
    
	public void  setColor(Color color) { this.color = color; }
	public Color getColor()            { return color;       }

	public void    setFill(boolean b) { fill = b;    }
	public boolean getFill()          { return fill; }

	protected void drawShape(Rubberband rb) {
		Graphics   g  = getGraphics();

		if(g != null) {
				g.setColor(color);
	
			if     (rb == rbLine)    drawLine     (rb, g);
			else if(rb == rbRect)    drawRectangle(rb, g);
			else if(rb == rbEllipse) drawEllipse  (rb, g);
			g.dispose();
		}
	}
	protected void drawLine(Rubberband rb, Graphics g) {
		Point anchor = rb.getAnchor(), end = rb.getEnd();
		g.drawLine(anchor.x, anchor.y, end.x, end.y);
	}
	protected void drawRectangle(Rubberband rb, Graphics g) {
		Rectangle r = rb.getBounds();

		if(fill) 
			g.fillRect(r.x+1, r.y+1, r.width-1, r.height-1);
		else
			g.drawRect(r.x, r.y, r.width, r.height);
	}
	protected void drawEllipse(Rubberband rb, Graphics g) {
		Rectangle r = rb.getBounds();

		if(fill) 
			g.fillArc(r.x+1, r.y+1, r.width-1, r.height-1, 0, 360);
		else
		g.drawArc(r.x, r.y, r.width, r.height, 0, 360);
	}
}
