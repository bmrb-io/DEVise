package gjt.test;

import java.net.URL;
import java.applet.Applet;
import java.awt.*;
import java.awt.Panel;

import gjt.Util;
import gjt.Orientation;
import gjt.animation.*;

/**
 * An animation playfield containing two "java drinkers", that
 * both bounce off the sides of the playfield.<p>
 *
 * One of the java drinkers moves slow and spins fast, while
 * the other java drinker moves fast and spins slow.  When
 * the two java drinkers collide, they both play a bump
 * sequence - at different speeds.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.test.AnimationTest
 * @see     gjt.animation.Playfield
 * @see     gjt.animation.Sprite
 */
public class TwoDrinkersAnimationTest extends UnitTest {
    public String title() { 
        return 
            "TwoDrinkers Animation - Mouse Down Starts/Stops"; 
    } 
    public Panel centerPanel() { 
        return new TwoDrinkersAnimationTestPanel(this); 
    }
}
class TwoDrinkersAnimationTestPanel extends Panel {
    public TwoDrinkersAnimationTestPanel(Applet applet) {
        setLayout(new BorderLayout());
        add(new TwoDrinkersPlayfield(applet), "Center"); 
    }
	public void update(Graphics g) {
		paint(g);
	}
}
