package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;
import java.net.URL;

import gjt.ImageScroller;
import gjt.Util;

/**
 * Four images are loaded; subsequent mouse clicks cycle 
 * through the images, that are displayed in an ImageScroller.
 *
 * @version 1.0, April 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.Scroller
 * @see     gjt.ImageScroller
 */
public class ImageScrollerTest extends UnitTest {
    public String title() { 
        return "ImageScroller Test"; 
    }
    public Panel centerPanel() { 
        return new ImageScrollerTestPanel(this); 
    }
}

class ImageScrollerTestPanel extends Panel implements
ActionListener {
    private Image[]       images     = new Image[4];         
    private int           imageIndex = 0;
    private ImageScroller scroller;
	private ImageScrollerAdvancePanel advancePanel;
	private Button        advance = new Button("Next Image");

    public ImageScrollerTestPanel(Applet applet) {
        URL cb = applet.getCodeBase();

        images[0]=applet.getImage(cb,"gifs/ashleyAndRoy.gif"); 
        images[1]=applet.getImage(cb,"gifs/ashleyAndSabre.gif");
        images[2]=applet.getImage(cb,"gifs/anjinAndMariko.gif");
        images[3]=applet.getImage(cb,"gifs/ashleyAndAnjin.gif");

		advancePanel = new ImageScrollerAdvancePanel(this);

        setLayout(new BorderLayout());
		add("North", advancePanel);
        add("Center", scroller = new ImageScroller(images[0]));
    }
	public void actionPerformed(ActionEvent event) {
        if(imageIndex == images.length-1) imageIndex = 0;
        else                              imageIndex++;

        Util.setCursor(Frame.WAIT_CURSOR, this);
        scroller.resetImage(images[imageIndex]);
        Util.setCursor(Frame.DEFAULT_CURSOR, this);

		scroller.getViewport().invalidate();
		scroller.validate();
	}
}

class ImageScrollerAdvancePanel extends Panel {
	private Button advanceButton = new Button("Advance Image");
	public ImageScrollerAdvancePanel(ImageScrollerTestPanel p) {
		add(advanceButton);
		advanceButton.addActionListener(p);
	}
}
