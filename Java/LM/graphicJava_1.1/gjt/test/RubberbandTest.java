package gjt.test;

import java.awt.Panel;

/**
 * A simple drawing applet that demonstrates the utility of 
 * the gjt.rubberband package.<p>
 *
 * Note that this unit test also serves as the unit test for
 * gjt.DrawingPanel.<p>
 *
 * @version 1.0, April 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DrawingPanel
 * @see     gjt.rubberband.Rubberband
 * @see     gjt.rubberband.RubberbandLine
 * @see     gjt.rubberband.RubberbandRectangle
 * @see     gjt.rubberband.RubberbandEllipse
 * @see     gjt.rubberband.RubberbandPanel
 */
public class RubberbandTest extends UnitTest {
    public String title() { 
        return "Rubberband Test"; 
    }
    public Panel centerPanel() { 
        return new RubberbandTestPanel(); 
    }
}
