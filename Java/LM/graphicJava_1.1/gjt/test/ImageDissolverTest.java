package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.FilteredImageSource;

import gjt.Util;
import gjt.image.ImageDissolver;

/**
 * Initially displays an image.  Subsequent mouse clicks in the 
 * canvas containing the image cause the image to fade in or 
 * fade out, depending upon it's current state.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.image.DissolveFilter
 * @see     gjt.image.ImageDissolver
 */
public class ImageDissolverTest extends UnitTest {
    public String title() { 
        return "ImageDissolver Test " + 
               "(Click Picture to Fade In/Out)";     
    }
    public Panel centerPanel() { 
        return new ImageDissolverTestPanel(this); 
    }
}

class ImageDissolverTestPanel extends    Panel 
                              implements MouseListener {
    ImageDissolverTestCanvas canvas;

    public ImageDissolverTestPanel(Applet applet) {
        add(canvas = new ImageDissolverTestCanvas(applet));
		canvas.addMouseListener(this);
    }
    public void mouseClicked (MouseEvent event) { }
    public void mouseReleased(MouseEvent event) { }
    public void mouseEntered (MouseEvent event) { }
    public void mouseExited  (MouseEvent event) { }

    public void mousePressed(MouseEvent event) {
        canvas.doFade();
    }
}

class ImageDissolverTestCanvas extends Canvas {
    private boolean         isFaded = false;
    private Image           image;
    private ImageDissolver  dissolver;

    public ImageDissolverTestCanvas(Applet applet) {
        image = 
        applet.getImage(applet.getCodeBase(),"gifs/saint.gif");

        Util.waitForImage(this, image);
        dissolver = new ImageDissolver(this, image);
    }
    public void paint(Graphics g) {
        if( ! isFaded)
            g.drawImage(image, 0, 0, this);
    }
    public Dimension getPreferredSize() {
        return new Dimension(image.getWidth(this), 
                             image.getHeight(this));
    }
    public void doFade() {
        if(isFaded) dissolver.fadeIn (0,0);
        else        dissolver.fadeOut(0,0);

        isFaded = isFaded ? false : true;
    }
}
