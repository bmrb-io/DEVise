package gjt;

import java.awt.*;

/**
 * A separator is a lightweight component that is drawn either 
 * vertically or horizontally depending upon how it is laid out.  * Separators can be drawn either etched-in or etched-out, with 
 * varying thicknesses.  Both thickness and etching are settable 
 * at construction time only.<p>
 *
 * Default thickness is 2 pixels and default etching is 
 * Etching.IN.  Note that thicknesses greater than 4 tend to 
 * loose the etching effect.<p>
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Feb 1 1997
 *
 *		Made Separator a lightweight (it originally extended
 *  	Canvas).
 *
 * @author  David Geary
 * @see     Etching
 * @see     gjt.test.SeparatorTest
 */
public class Separator extends Component {
    static private Etching _defaultEtching   = Etching.IN;
    static private int     _defaultThickness = 2;

    private Etching etching;
    private int     thickness;

    public Separator() {
        this(_defaultThickness, _defaultEtching);
    }
    public Separator(int thickness) {
        this(thickness, _defaultEtching);
    }
    public Separator(Etching etching) {
        this(_defaultThickness, etching);
    }
    public Separator(int thickness, Etching etching) {
        this.etching   = etching;
        this.thickness = thickness;
        setSize(thickness, thickness);
    }
	/**
	 * @deprecated as of JDK1.1
	 */
    public Dimension minimumSize() {
        return getPreferredSize();
    }
    public Dimension getMinimumSize() {
		return minimumSize();
	}
	/**
	 * @deprecated as of JDK1.1
	 */
    public Dimension preferredSize() {
        return new Dimension(thickness, thickness);
	}
    public Dimension getPreferredSize() {
		return preferredSize();
    }
    public void paint(Graphics g) {
        Dimension size     = getSize();
        Color brighter = SystemColor.controlLtHighlight;
        Color darker   = SystemColor.controlShadow;

        if(etching == Etching.IN) {
            if(size.width > size.height)
                paintHorizontal(g, size, darker, brighter);
            else
                paintVertical(g, size, darker, brighter);
        }
        else {
            if(size.width > size.height)
                paintHorizontal(g, size, brighter, darker);
            else
                paintVertical(g, size, brighter, darker);
        }
    }
    public String paramString() {
        Dimension   size = getSize();
        Orientation orient = size.width > size.height ? 
                             Orientation.HORIZONTAL :
                             Orientation.VERTICAL;
        return super.paramString() + "thickness=" + 
               thickness + "," + etching + "," + orient;
    }
    private void paintHorizontal(Graphics g, Dimension size, 
                                 Color top, Color bottom) {
        g.setColor(top);
		int y = (size.height/2) - (thickness/2);
		while(y < (size.height/2)) {
			g.drawLine(0, y, size.width, y);
			++y;
		}
        g.setColor(bottom);
		y = size.height/2;
		while(y < ((size.height/2) + (thickness/2))) {
			g.drawLine(0, y, size.width, y);
			++y;
		}
    }
    private void paintVertical(Graphics g, Dimension size, 
                               Color left, Color right) {
        g.setColor(left);
		int i = (size.width/2) - (thickness/2);
		while(i < (size.width/2)) {
			g.drawLine(i, 0, i, size.height);
			++i;
		}
        g.setColor(right);
		i = size.width/2;
		while(i < ((size.width/2) + (thickness/2))) {
			g.drawLine(i, 0, i, size.height);
			++i;
		}
    }
}
