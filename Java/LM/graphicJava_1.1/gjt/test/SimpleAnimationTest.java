package gjt.test;

import java.net.URL;
import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;

import gjt.Util;
import gjt.Orientation;
import gjt.animation.*;

/**
 * An animation playfield containing a lone sprite that bounces
 * off the boundaries of the playfield.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.test.AnimationTest
 * @see     gjt.animation.Playfield
 * @see     gjt.animation.Sprite
 */
public class SimpleAnimationTest extends UnitTest {
    public String title() { 
        return "Simple Animation - Mouse Down Starts/Stops"; 
    } 
    public Panel centerPanel() { 
        return new SimpleAnimationTestPanel(this); 
    }
}
class SimpleAnimationTestPanel extends Panel {
    public SimpleAnimationTestPanel(Applet applet) {
        setLayout(new BorderLayout());
        add(new SimplePlayfield(applet), "Center"); 
    }
	public void update(Graphics g) {
		paint(g);
	}
}
class SimplePlayfield extends Playfield {
    private Applet   applet;
    private URL      cb;
    private Sprite   javaDrinker;
    private Sequence spinSequence;

    public SimplePlayfield(Applet applet) {
		this.applet = applet;
		cb = applet.getCodeBase();

        makeSequencesAndSprites();

		setWallpaperImage(
			applet.getImage(cb, "gifs/background.gif"));

		addMouseListener(new MouseAdapter() {
    		public void mousePressed(MouseEvent event) {
        		if(running() == true) stop ();
        		else                  start();
    		}
		});
	}
    public void edgeCollision(Sprite      sprite, 
                              Orientation orientation) {
        if(orientation == Orientation.RIGHT || 
           orientation == Orientation.LEFT)
            sprite.reverseX();
        else
            sprite.reverseY();
    }
    private void makeSequencesAndSprites() {
        String  file;
        Point   startLoc   = new Point(10, 10);
        Image[] spinImages = new Image[19];

        for(int i=0; i < spinImages.length; ++i) {
            file = "gifs/spin";

            if(i < 10) file += "0" + i + ".gif";
            else       file += i + ".gif";

            spinImages[i] = applet.getImage(cb, file);
        }
        spinSequence = new Sequence(this, spinImages);
        javaDrinker  = new Sprite(this, spinSequence, startLoc);

        javaDrinker.setMoveVector(new Point(1,1));
        add(javaDrinker);
    }
}
