package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * A simple unit test for exercising CursorChoice.
 *
 * @version 1.0, Feb, 1997
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.CursorChoice
 */
public class CursorChoiceTest extends UnitTest {
	private CursorChoice cursorChoice = new CursorChoice();

    public String title() { 
        return "CursorChoice Test"; 
    }
    public Panel centerPanel() { 
		Panel panel       = new Panel();
		Panel cursorPanel = new Panel();

		cursorPanel.add(cursorChoice);
		panel.setLayout(new BorderLayout());
		panel.add("North", cursorPanel);

		cursorChoice.addItemListener(new ItemListener() {
			public void itemStateChanged(ItemEvent event) {
				setCursor(cursorChoice.getSelectedCursor());
			}
		});
		return panel;
    }
}
