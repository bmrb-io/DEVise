package gjt;

import java.awt.Component;

/**
 * A factory that returns a Border, given a BorderStyle.
 *
 * This class may not be instantiated.
 *
 * @version 1.0, Apr 11 1996
 * @author  David Geary
 */
public class BorderFactory {
	static public Border createBorder(Component comp, 
	                                  BorderStyle style) {
		Border border = null;

		if(style == BorderStyle.RAISED) {
			border = new ThreeDBorder(comp);
			((ThreeDBorder)border).raise();
		}
		else if(style == BorderStyle.INSET) {
			border = new ThreeDBorder(comp);
			((ThreeDBorder)border).inset();
		}
		else if(style == BorderStyle.SOLID) {
			border = new Border(comp);
		}
		else if(style == BorderStyle.ETCHED) {
			border = new EtchedBorder(comp);
		}
		return border;
	}
	private BorderFactory() { }
}
