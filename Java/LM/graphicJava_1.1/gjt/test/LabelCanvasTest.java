package gjt.test;
import java.applet.Applet;
import java.awt.Event;
import java.awt.Panel;
import java.awt.Insets;
import java.awt.Graphics;
import gjt.LabelCanvas;
import gjt.Util;
import java.awt.event.*;

/**
 * Four LabelCanvases, each with different insets.  The leftmost
 * LabelCanvas has def getInsets (2 all around), while the
 * other three were constructed as follows:
 * <pre>
 *      five.setInsets   (new Insets(5,5,5,5));
 *      ten.setInsets    (new Insets(10,10,10,10));
 *      fifteen.setInsets(new Insets(15,15,15,15));
 * </pre><p>
 *
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.LabelCanvas
 */
public class LabelCanvasTest extends UnitTest {
    public String title() { 
		return "LabelCanvases Are Lightweights!"; 
	}
    public Panel centerPanel() { 
        return new LabelCanvasTestPanel(this); 
    }
}

class LabelCanvasTestPanel extends    Panel 
                           implements ItemListener {
    static private Applet applet;
    LabelCanvas def     = new LabelCanvas("Default Insets");
    LabelCanvas five    = new LabelCanvas("Insets = Five");
    LabelCanvas ten     = new LabelCanvas("Insets = Ten");
    LabelCanvas fifteen = new LabelCanvas("Insets = Fifteen");

	static public void broadcastSelection(LabelCanvas canvas) {
		String s = null;

		if(canvas.isSelected())
        	s = "LabelCanvas " + canvas.getLabel() + 
			                     " selected";
        else
        	s = "LabelCanvas " + canvas.getLabel() + 
			                     " deselected";
		applet.showStatus(s);
		System.out.println(s);

		canvas.repaint();
	}
    public LabelCanvasTestPanel(Applet applet) {
        this.applet = applet;

        five.setInsets   (new Insets(5,5,5,5));
        ten.setInsets    (new Insets(10,10,10,10));
        fifteen.setInsets(new Insets(15,15,15,15));

        add(def);
        add(five);
        add(ten);
        add(fifteen);

		def.addItemListener    (this);
		five.addItemListener   (this);
		ten.addItemListener    (this);
		fifteen.addItemListener(this);
	}
	public void itemStateChanged(ItemEvent event) {
		LabelCanvas canvas = (LabelCanvas)event.getSource();
		broadcastSelection(canvas);
	}
}
