package gjt;

import java.awt.Component;

/**
 * A factory that returns a DrawnRectangle (or extension), given a BorderStyle.
 *
 * This class may not be instantiated.
 *
 * @version 1.0, Apr 11 1996
 * @author  David Geary
 */
public class RectangleFactory {
	static public DrawnRectangle createRectangle(Component comp, 
	                                         BorderStyle style) {
		DrawnRectangle rectangle = null;

		if(style == BorderStyle.RAISED) {
			rectangle = new ThreeDRectangle(comp);
			((ThreeDRectangle)rectangle).raise();
		}
		else if(style == BorderStyle.INSET) {
			rectangle = new ThreeDRectangle(comp);
			((ThreeDRectangle)rectangle).inset();
		}
		else if(style == BorderStyle.SOLID) {
			rectangle = new DrawnRectangle(comp);
		}
		else if(style == BorderStyle.ETCHED) {
			rectangle = new EtchedRectangle(comp);
		}
		return rectangle;
	}
	private RectangleFactory() { }
}
