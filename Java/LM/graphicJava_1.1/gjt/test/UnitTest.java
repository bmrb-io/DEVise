package gjt.test;

import java.awt.*;
import java.applet.Applet;
import gjt.*;

/**
 * An (abstract) Applet fitted with a BorderLayout that 
 * contains a TitledPanel in the North, and a Panel created by 
 * derived classes in the Center.<p>
 *
 * Since some Applets take awhile to load, UnitTest changes the 
 * cursor to a wait cursor in init(), changing it back to the 
 * default cursor in start().  Derived classes must be sure to 
 * call super.init() if they override init(); likewise for 
 * start().<p>
 *
 * Subclasses must implement: 
 * <dl>
 * <dd>String title()
 * <dd>Panel centerPanel()
 * </dl>
 * Subclasses should populate the Panel returned from 
 * centerPanel() with whatever makes sense for their unit test.
 *
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov 8,  1996
 *    Added EtchedBorder around center panel for asthetic reasons.
 *
 * @author  David Geary
 * @see     TitledPanel
 */
abstract public class UnitTest extends Applet {
    abstract public String title();
    abstract public Panel  centerPanel();

	private Panel center;

    public void init() {
		Util.setCursor(Cursor.WAIT_CURSOR, this);

        Panel titledPanel = new TitledPanel(title());
        setLayout(new BorderLayout());
        add(titledPanel, "North");
        add(new EtchedBorder(center = centerPanel()), "Center");
    }
	public void start() {
		Util.setCursor(Cursor.DEFAULT_CURSOR, this);
	}
}
