package gjt;

import java.awt.*;

/**
 * Layout manager for a Scroller.<p>
 *
 * Lays out 3 Components:  a horizontal scrollbar, a vertical 
 * scrollbar and a viewport (Panel).<p>
 *
 * Valid names/Component pairs that can be added via 
 * addLayoutComponent(String, Component):<p>
 * <dl>
 * <dd> "East"   Scrollbar (vertical)
 * <dd> "West"   Scrollbar (vertical)
 * <dd> "North"  Scrollbar (horizontal)
 * <dd> "South"  Scrollbar (horizontal)
 * <dd> "Scroll" Panel (viewport)
 * </dl>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     Scroller
 */
public class ScrollerLayout implements LayoutManager {
    private Scroller  scroller;
    private Scrollbar hbar, vbar;
    private String    hbarPosition, vbarPosition;
    private Component viewport;
    private int       top, bottom, right, left;

    public ScrollerLayout(Scroller scroller) {
        this.scroller = scroller;
    }

    public void addLayoutComponent(String name, 
                                   Component comp) {
        Assert.notFalse(comp != null);

        if(comp instanceof Scrollbar) {
            Scrollbar sbar = (Scrollbar)comp;

            if(sbar.getOrientation() == Scrollbar.VERTICAL) {
                Assert.notFalse("East".equals(name) == true ||
                                "West".equals(name) == true);
                vbar         = sbar;
                vbarPosition = name;
            }
            else {
                Assert.notFalse("North".equals(name) == true ||
                                "South".equals(name) == true);
                hbar         = sbar;
                hbarPosition = name;
            }
        }
        else {
            Assert.notFalse("Scroll".equals(name) == true);
            viewport = comp;
        }
    }
    public void removeLayoutComponent(Component comp) {
        if(comp == vbar)     vbar     = null;
        if(comp == hbar)     hbar     = null;
        if(comp == viewport) viewport = null;
    }
    public Dimension preferredLayoutSize(Container parent) {
		Assert.isTrue(parent == scroller);
        Dimension ps = new Dimension(0,0);
		Insets    insets = parent.getInsets();

        if(vbar != null && vbar.isVisible()) {
            Dimension d = vbar.getPreferredSize();
            ps.width  = d.width;
			ps.height = d.height;
        }
        if(hbar != null && hbar.isVisible()) {
            Dimension d = hbar.getPreferredSize();
            ps.width  = d.width;
			ps.height = d.height + ps.height;
        }
        if(viewport != null && viewport.isVisible()) {
            Dimension d = viewport.getPreferredSize();
            ps.width  = Math.max(d.width, ps.width);
            ps.height = Math.max(d.height, ps.height);
        }
		ps.width  += insets.left + insets.right;
		ps.height += insets.top + insets.bottom;
        return ps;
    }
    public Dimension minimumLayoutSize(Container parent) {
		Assert.isTrue(parent == scroller);
        Dimension ps = new Dimension(0,0);
		Insets    insets = parent.getInsets();

        if(vbar != null && vbar.isVisible()) {
            Dimension d = vbar.getMinimumSize();
            ps.width  = d.width;
			ps.height = d.height;
        }
        if(hbar != null && hbar.isVisible()) {
            Dimension d = hbar.getMinimumSize();
            ps.width  = d.width;
			ps.height = d.height + ps.height;
        }
        if(viewport != null && viewport.isVisible()) {
            Dimension d = viewport.getMinimumSize();
            ps.width  = Math.max(d.width, ps.width);
            ps.height = Math.max(d.height, ps.height);
        }
		ps.width  += insets.left + insets.right;
		ps.height += insets.top + insets.bottom;
        return ps;
    }
	public void layoutContainer(Container target) {
        Insets insets        = target.getInsets();
        Dimension targetSize = target.getSize();

        top    = insets.top;
        bottom = targetSize.height - insets.bottom;
        left   = insets.left;
        right  = targetSize.width - insets.right;

        scroller.manageScrollbars();

        reshapeHorizontalScrollbar();
        reshapeVerticalScrollbar  ();
        reshapeViewport           ();

        scroller.setScrollbarValues();
    }
    private void reshapeHorizontalScrollbar() {
        if(hbar != null && hbar.isVisible()) {
            if("North".equals(hbarPosition)) {
                Dimension d = hbar.getPreferredSize();
                hbar.setBounds(left, top, right - left, d.height);
                top += d.height;
            }
            else {  // South
                Dimension d = hbar.getPreferredSize();
                hbar.setBounds(left, bottom - d.height,
                            right - left,d.height);
                bottom -= d.height;
            }
        }
    }
    private void reshapeVerticalScrollbar() {
        if(vbar != null && vbar.isVisible()) {
            if("East".equals(vbarPosition)) {
                Dimension d = vbar.getPreferredSize();
                vbar.setBounds(right - d.width, top, 
                             d.width, bottom - top);
                right -= d.width;
            }
            else { // West
                Dimension d = vbar.getPreferredSize();
                vbar.setBounds(left, top, 
                             d.width, bottom - top);
                left += d.width;
            }
        }
    }
    private void reshapeViewport() {
        if(viewport != null && viewport.isVisible()) {
            viewport.setBounds(left, top, 
                             right - left, bottom - top);
        }
    }
}
