package gjt.rubberband;

import java.awt.Component;
import java.awt.Graphics;

/** 
 * A Rubberband that does lines.
 *
 * @version 1.0, 12/27/95
 * @author  David Geary
 * @see     Rubberband
 * @see     gjt.test.RubberbandTest
 */
public class RubberbandLine extends Rubberband {
	public RubberbandLine() {
	}
    public RubberbandLine(Component component) {
        super(component);
    }
    public void drawLast(Graphics graphics) {
        graphics.drawLine(anchorPt.x, anchorPt.y, 
		                  lastPt.x,   lastPt.y);
    }
    public void drawNext(Graphics graphics) {
        graphics.drawLine(anchorPt.x,    anchorPt.y, 
                          stretchedPt.x, stretchedPt.y);
    }
}
