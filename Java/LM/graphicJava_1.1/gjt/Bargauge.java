package gjt;

import java.awt.*;

/**
 * A bargauge which can be filled (wholly or partially) with a 
 * client-specified color.  Fill color is specified at 
 * construction time; both fill color and fill percent may be 
 * set after construction time.<p>
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Feb 1 1997
 *		Made Bargauge a lightweight component - it originally
 *		extended Canvas.
 *
 * @author  David Geary
 * @see     ThreeDRectangle
 * @see     gjt.test.BargaugeTest
 */
public class Bargauge extends Component {
    private double          percentFill = 0;
    private ThreeDRectangle border = new ThreeDRectangle(this);
    private Color           fillColor;

    public Bargauge(Color fillColor) {
        setFillColor(fillColor);
    }
    public void setFillColor(Color fillColor) {
        this.fillColor = fillColor;
    }
    public void setFillPercent(double percentage) {
        Assert.notFalse(percentage >= 0 && percentage <= 100);
        percentFill = percentage;
    }
	/**
	 * @deprecated for JDK1.1
	 */
	public void resize(int w, int h) {
		setSize(w,h);
	}
    public void setSize(int w, int h) {
        setBounds(getLocation().x, getLocation().y, w, h);
    }
	/**
	 * @deprecated for JDK1.1
	 */
	public void reshape(int x, int y, int w, int h) {
		// compiler will issue deprecation warning, but we can't call
		// super.setBounds(), because it calls reshape()!
        super.reshape(x,y,w,h); 
        border.setSize(w,h); 
    }
    public void setBounds(int x, int y, int w, int h) { 
		reshape(x,y,w,h);
	}
	/**
	 * @deprecated for JDK1.1
	 */
	public Dimension minimumSize() {
		return getPreferredSize(); 
	}
    public Dimension getMinimumSize() { 
		return minimumSize();
	}
	/**
	 * @deprecated for JDK1.1
	 */
	public Dimension preferredSize() {
        int w = border.getThickness() * 3;
        return new Dimension(w, w*4);
	}
    public Dimension getPreferredSize() {
		return preferredSize();
    }
    public void paint(Graphics g) {
        border.raise();
        border.paint();
        fill();
    }
    public void fill() {
        Graphics g = getGraphics();

        if((g != null) && (percentFill > 0)) {
            Rectangle b       = border.getInnerBounds();
            int       fillw   = b.width; 
            int       fillh   = b.height;

            if(b.width > b.height) fillw *= percentFill/100;
            else                   fillh *= percentFill/100;

            g.setColor(fillColor);
            border.clearInterior();

            if(b.width > b.height) 
                g.fill3DRect(b.x, b.y, fillw, b.height, true);
            else                   
                g.fill3DRect(b.x, b.y + b.height - fillh, 
                             b.width, fillh, true);
			g.dispose();
        }
    }
    protected String paramString() {
        Dimension size = getSize();
        Orientation orient = size.width > size.height ? 
                             Orientation.HORIZONTAL :
                             Orientation.VERTICAL;
        String    str  = "fill percent=" + percentFill + "," + 
                         "orientation="  + orient      + "," +
                         "color"         + fillColor;
        return str;
    }
}
