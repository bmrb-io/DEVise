package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * Each Scroller contains a Panel (viewport) and two Scrollbars 
 * (horizontal and vertical).  Works in conjunction with a 
 * ScrollerLayout, that lays out the viewport and two 
 * scrollbars.<p>
 * 
 * Subclasses must override:<p>
 * <dl>
 * <dd> abstract public void      scrollTo(int x, int y)
 * <dd> abstract public Dimension getScrollAreaSize()
 * </dl>
 * 
 * @version 1.0, Apr 1 1996
 *
 * @version 1.1, Dec 1996
 *
 *    Upgraded to new event handling model.
 *
 * @author  David Geary
 * @see     ComponentScroller
 * @see     ImageScroller
 * @see     ScrollerLayout
 * @see     gjt.test.ComponentScrollerTest
 * @see     gjt.test.ImageScrollerTest
*/
public abstract class Scroller extends    Panel
				               implements AdjustmentListener {
    protected Panel     viewport;
    protected Scrollbar hbar, vbar;

    abstract public void      scrollTo(int x, int y);
    abstract public Dimension getScrollAreaSize();

    public Scroller() {
        setLayout(new ScrollerLayout(this));
        add("Scroll", viewport = new Panel());
        add("East", vbar = new Scrollbar(Scrollbar.VERTICAL));
        add("South",hbar = new Scrollbar(Scrollbar.HORIZONTAL));

		hbar.addAdjustmentListener(this);
		vbar.addAdjustmentListener(this);
    }
    public Scrollbar getHorizontalScrollbar() {return hbar;    }
    public Scrollbar getVerticalScrollbar  () {return vbar;    }
    public Panel     getViewport           () {return viewport;}

	public void adjustmentValueChanged(AdjustmentEvent event) {
        switch(event.getAdjustmentType()) {
            case AdjustmentEvent.BLOCK_INCREMENT:  
			scrollLineUp(); 
            break;
            case AdjustmentEvent.BLOCK_DECREMENT:  
			scrollLineDown(); 
            break;
            case AdjustmentEvent.UNIT_INCREMENT:   
			scrollPageUp  (); 
            break;
            case AdjustmentEvent.UNIT_DECREMENT:   
			scrollPageDown(); 
            break;
            case AdjustmentEvent.TRACK:            
			scrollAbsolute(); 
            break;
        }
    }
    public void paint (Graphics g) { scroll(); }
    public void update(Graphics g) { paint(g); }

    public void manageScrollbars() { 
        manageHorizontalScrollbar();
        manageVerticalScrollbar  ();
    }
    protected void manageHorizontalScrollbar() {
        Dimension size           = getSize();
        Dimension scrollAreaSize = getScrollAreaSize();

        if(vbar.isVisible())
            size.width -= vbar.getSize().width;

        if(scrollAreaSize.width > size.width) {
            if( ! hbar.isVisible())
                hbar.setVisible(true);
        }
        else if(hbar.isVisible()) {
            hbar.setVisible(false);
            hbar.setValue(0);
            repaint();
        }
    }
    protected void manageVerticalScrollbar() {
        Dimension size           = getSize();
        Dimension scrollAreaSize = getScrollAreaSize();

        if(hbar.isVisible())
            size.height -= hbar.getSize().height;

        if(scrollAreaSize.height > size.height) {
            if( ! vbar.isVisible())
                vbar.setVisible(true);
        }
        else if(vbar.isVisible()) {
            vbar.setVisible(false);
            vbar.setValue(0);
            repaint();
        }
    }
	public void setScrollPosition(int x, int y) {
		scrollTo(x,y);
		hbar.setValue(x);
		hbar.setValue(y);
		repaint();
	}
    public void setScrollbarValues() {
        if(hbar.isVisible()) setHorizontalScrollbarValues();
        if(vbar.isVisible()) setVerticalScrollbarValues();
    }
    protected void setHorizontalScrollbarValues() {
        Dimension vsize          = viewport.getSize();
        Dimension scrollAreaSize = getScrollAreaSize(); 
        int max = scrollAreaSize.width; 

        hbar.setValues(hbar.getValue(), // value
                       vsize.width,     // amt visible/page
                       0,               // minimum
                       max);            // maximum

        setHorizontalLineAndPageIncrements();
    }
    protected void setVerticalScrollbarValues() {
        Dimension vsize          = viewport.getSize();
        Dimension scrollAreaSize = getScrollAreaSize(); 
        int max = scrollAreaSize.height;

        vbar.setValues(vbar.getValue(), // value
                       vsize.height,    // amt visible/page
                       0,               // minimum
                       max);            // maximum

        setVerticalLineAndPageIncrements();
    }
    protected void scrollLineUp  () { scroll(); }
    protected void scrollLineDown() { scroll(); }
    protected void scrollPageUp  () { scroll(); }
    protected void scrollPageDown() { scroll(); }
    protected void scrollAbsolute() { scroll(); }

    protected void setHorizontalLineAndPageIncrements() {
        Dimension size = getScrollAreaSize();
        hbar.setUnitIncrement(size.width/10);
        hbar.setBlockIncrement(size.width/5);
    }
    protected void setVerticalLineAndPageIncrements() {
        Dimension size = getScrollAreaSize();
        vbar.setUnitIncrement(size.height/10);
        vbar.setBlockIncrement(size.height/5);
    }
    protected void scroll() {
        scrollTo(hbar.getValue(), vbar.getValue());
    }
}
