package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.FilteredImageSource;

import gjt.*;
import gjt.image.BlackAndWhiteFilter;

/**
 * Initially displays an color image.  Subsequent mouse
 * clicks in the canvas containing the image toggle between
 * a black and white version of the image and a color one.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.image.BlackAndWhiteFilter
 */
public class BlackAndWhiteFilterTest extends UnitTest {
    public String title() { 
        return "BlackAndWhiteFilter Test " + 
               "(Click In Image to Toggle Color/B&W)";
    }
    public Panel centerPanel() { 
        return new BlackAndWhiteFilterTestPanel(this); 
    }
}

class BlackAndWhiteFilterTestPanel extends Panel {
    BlackAndWhiteFilterTestCanvas canvas;

    public BlackAndWhiteFilterTestPanel(Applet applet) {
        add(canvas = new BlackAndWhiteFilterTestCanvas(applet));
		canvas.addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent event) {
        		canvas.toggleColor();
        		canvas.repaint();
    		}
		});
    }
}

class BlackAndWhiteFilterTestCanvas extends Canvas {
    private Image   im;
    private Image   blackAndWhite;
    private boolean showingBW = false;

    public BlackAndWhiteFilterTestCanvas(Applet applet) {
        im = applet.getImage(applet.getCodeBase(), 
                             "gifs/saint.gif");
        Util.waitForImage(this, im);

        FilteredImageSource source = 
            new FilteredImageSource(im.getSource(), 
                                  new BlackAndWhiteFilter());

        blackAndWhite = createImage(source);
        Util.waitForImage(this, blackAndWhite);
    }
    public Dimension getPreferredSize() {
        return new Dimension(im.getWidth(this), 
                             im.getHeight(this));
    }
    public void paint(Graphics g) {
        if(showingBW) g.drawImage(blackAndWhite,0,0,this);
        else          g.drawImage(im,      0,0,this);
    }
    public void toggleColor() {
        showingBW = showingBW ? false : true;
    }
}
