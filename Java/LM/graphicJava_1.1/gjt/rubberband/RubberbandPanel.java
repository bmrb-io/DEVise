package gjt.rubberband;

import java.awt.*;
import java.awt.event.*;

/**
 * An extension of Panel which is fitted with a Rubberband.  
 * Handling of mouse events is automatically handled for 
 * rubberbanding.<p>
 *
 * Clients may set or get the Rubberband at any time.<p>
 *
 * @version 1.0, Dec 27 1995
 * @version 1.1, Feb 1997
 *
 *	Made mods for 1.1 event handling.
 *
 * @author  David Geary
 * @see     Rubberband
 * @see     gjt.test.RubberbandTest
 */
abstract public class RubberbandPanel extends Panel {
    private Rubberband rubberband;

	abstract public void rubberbandEnded(Rubberband rb);

    public void setRubberband(Rubberband rb) {
		if(rubberband != null) {
			rubberband.setActive(false);
		}
		rubberband = rb;

		if(rubberband != null) {
			rubberband.setActive(true);
			rubberband.setComponent(this);
		}
    }
	public Rubberband getRubberband() {
		return rubberband;
	}
	public void processMouseEvent(MouseEvent event) {
		super.processMouseEvent(event);  // fire to listeners

		if(rubberband != null && 
		   event.getID() == MouseEvent.MOUSE_RELEASED)
			rubberbandEnded(rubberband);
	}
}
