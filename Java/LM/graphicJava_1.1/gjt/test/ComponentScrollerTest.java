package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import java.applet.Applet;
import java.net.URL;

import gjt.*;

/**
 * A phony image store, where you can purchase images.<p>
 *
 * @version 1.0, April 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     Border
 * @see     ButtonPanel
 * @see     ImageButton
 * @see     Separator
 * @see     StickyImageButtonController
 * @see     ComponentScroller
 */
public class ComponentScrollerTest extends UnitTest {
    public String title() { 
        return "ComponentScroller Test"; 
    }
    public Panel  centerPanel() { 
        return new CSTestPanel(this);
    }
}

class CSTestPanel extends Panel {
	private ImageStore           imageStore;
	private ImageButtonTestPanel ibPanel;
	private ComponentScroller    scroller;

	public CSTestPanel(Applet applet) {
		imageStore = new ImageStore(applet);
		scroller   = new ComponentScroller();
		ibPanel    = new ImageButtonTestPanel(applet);

		scroller.setComponent(imageStore);

		ibPanel = new ImageButtonTestPanel(applet);

		setLayout(new BorderLayout());
		add("North",  new TogglePanel(this,imageStore,ibPanel));
		add("Center", scroller);
	}
	public ComponentScroller getScroller() {
		return scroller;
	}
}

class TogglePanel extends Panel implements ActionListener {
	private boolean              showingImageStore = true;
	private ImageButtonTestPanel ibPanel;
	private ImageStore           imageStore;
	private CSTestPanel          panel;
	
	public TogglePanel(CSTestPanel          panel,
	                   ImageStore           imageStore,
					   ImageButtonTestPanel ibPanel) {
		Button toggleButton = new Button("Toggle Component");

		this.panel      = panel;
		this.imageStore = imageStore;
		this.ibPanel    = ibPanel;
		add(toggleButton);
		toggleButton.addActionListener(this);
	}
	public void actionPerformed(ActionEvent event) {
		if(showingImageStore) {
			panel.getScroller().setComponent(ibPanel);
			showingImageStore = false;
		}
		else {
			panel.getScroller().setComponent(imageStore);
			showingImageStore = true;
		}
	}
}
