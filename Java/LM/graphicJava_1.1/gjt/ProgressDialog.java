package gjt;

import java.awt.*;

/**
 * A dialog that uses a bargauge to indicate progress made on a
 * task that presumably takes some time to complete.
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     GJTDialog
 * @see     Bargauge
 * @see     gjt.test.DialogTest
 */
public class ProgressDialog extends GJTDialog {
    static private int     _preferredWidth  = 400;
    static private int     _preferredHeight = 63;

    private Bargauge bargauge;

    public ProgressDialog(Frame frame, DialogClient client,
                           String title, Color color) {
		this(frame, client, title, color, false);
	}
    public ProgressDialog(Frame frame, DialogClient client,
                           String title, Color color,
						   boolean modal) {
        super(frame, title, client, true, modal);

        bargauge = new Bargauge(color);
		setLayout(new BorderLayout());
		add(bargauge, "Center");
    }
    public void setPercentComplete(double percent) {
        bargauge.setFillPercent(percent);
        bargauge.fill();

        if(percent == 100) {
            dispose();
		}
    }
    public void reset() {
        bargauge.setFillPercent(0);
    }
	/**
	 * @deprecated as of JDK1.1
	 */
    public Dimension preferredSize() {
        return new Dimension(_preferredWidth, _preferredHeight);
    }
    public Dimension getPreferredSize() {
		return preferredSize();
	}
}
