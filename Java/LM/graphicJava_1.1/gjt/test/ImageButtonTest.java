package gjt.test;

import java.applet.Applet;
import java.awt.*;

/**
 * 2 ImageButtons, one springy and the other sticky, both 
 * crabby.<p>
 *
 * Both ImageButtons come with an awt.Button that is used to 
 * enable/disable the ImageButton it's associated with.<p>
 *
 * ImageButtonEvents, along with mouse enter and mouse exit
 * events for the two image buttons are printed out.<p>
 *
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.ImageButton
 * @see     gjt.ImageButtonEvent
 * @see     gjt.SpringyImageButtonController
 * @see     gjt.StickyImageButtonController
 */
public class ImageButtonTest extends UnitTest {
    public String title() { 
        return "ImageButton Test"; 
    }
    public Panel centerPanel() { 
        return new ImageButtonTestPanel(this); 
    }
}
