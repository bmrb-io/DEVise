package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * A simple unit test for exercising ColorChoice.
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.ColorChoice
 */
public class ColorChoiceTest extends UnitTest {
	private ColorChoice   colorChoice = new ColorChoice();
	private ColoredCanvas canvas      = new ColoredCanvas();

    public String title() { 
        return "ColorChoice Test"; 
    }
    public Panel centerPanel() { 
		Panel panel            = new Panel();
		Panel colorChoicePanel = new Panel();
		
		colorChoicePanel.add(colorChoice);
		canvas.setColor(Color.black);

		panel.setLayout(new BorderLayout());
		panel.add("North",  colorChoicePanel);
		panel.add("Center", canvas);

		colorChoice.addItemListener(new ItemListener() {
			public void itemStateChanged(ItemEvent event) {
				canvas.setColor(colorChoice.getColor());
				canvas.repaint();
			}
		});
		return panel;
    }
}
class ColoredCanvas extends Canvas {
	private Color color = SystemColor.control;

	public void setColor(Color color) {
		this.color = color;
	}
	public void paint(Graphics g) {
		Dimension size = getSize();
		g.setColor(color);
		g.fillRect(0,0,size.width-1,size.height-1);
	}
}
