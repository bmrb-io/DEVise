package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.FilteredImageSource;

import gjt.Util;
import gjt.image.NegativeFilter;

/**
 * Initially displays an unnegative image.  Subsequent mouse
 * clicks in the canvas containing the image toggle between
 * a negative version of the image and an unnegative version.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.image.NegativeFilter
 */
public class NegativeFilterTest extends UnitTest {
    public String title() { 
        return "NegativeFilter Test " + 
               "(Click below to toggle Negative)";
    }
    public Panel centerPanel() { 
        return new NegativeFilterTestPanel(this); 
    }
}

class NegativeFilterTestPanel extends Panel {
    NegativeFilterTestCanvas canvas;

    public NegativeFilterTestPanel(Applet applet) {
        add(canvas = new NegativeFilterTestCanvas(applet));
		canvas.addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent event) {
        		canvas.toggleNegative();
        		canvas.repaint();
    		}
		});
	}
}

class NegativeFilterTestCanvas extends Canvas {
    private Image   im;
    private Image   negative;
    private boolean showingNegative = false;

    public NegativeFilterTestCanvas(Applet applet) {
        im = applet.getImage(applet.getCodeBase(), 
                             "gifs/saint.gif");
        Util.waitForImage(this, im);

        FilteredImageSource source = 
            new FilteredImageSource(im.getSource(), 
                                  new NegativeFilter());

        negative = createImage(source);
        Util.waitForImage(this, negative);
    }
    public Dimension getPreferredSize() {
        return new Dimension(im.getWidth(this), 
                             im.getHeight(this));
    }
    public void paint(Graphics g) {
        if(showingNegative) g.drawImage(negative,0,0,this);
        else                g.drawImage(im,      0,0,this);
    }
    public void toggleNegative() {
        showingNegative = showingNegative ? false : true;
    }
}
