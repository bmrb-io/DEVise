package gjt.test;

import java.awt.*;
import java.applet.Applet;
import gjt.*;

/**
 * 9 DrawnRectangles (some of which are EtchedRectangles
 * and ThreeDRectangles) with varying characteristics such
 * as line widths and colors.<p>
 *
 * A mouse down (any mouse button) in any of the rectangles 
 * causes information about the rectangle to be printed to 
 * System.out. (Output will go to Java Console in Netscape).<p>
 *
 * @version 1.0, April 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DrawnRectangle
 * @see     gjt.EtchedRectangle
 * @see     gjt.ThreeDRectangle
 */
public class DrawnRectangleTest extends UnitTest {
    public String title() { 
        return "Drawn Rectangle Test - " +
		       "Click in Rectangles to See Their Properties";  
    }
    public Panel centerPanel() { 
        return new DrawnRectangleTestPanel(this);
    }
}
