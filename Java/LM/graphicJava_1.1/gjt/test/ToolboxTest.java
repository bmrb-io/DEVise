package gjt.test;

import java.applet.Applet;
import java.net.URL;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * A toolbox test that displays a toolbox with buttons 
 * representing AWT components.
 *
 * @version 1.0, January, 1997
 * @author  David Geary
 * @see     gjt.Toolbox
 */
public class ToolboxTest extends UnitTest {
    public String title() { 
        return "Toolbox Test:  Click Button To Show Toolbox";
    }
    public Panel centerPanel() { 
        return new ToolboxTestPanel(this); 
    }
}

class ToolboxTestPanel extends Panel implements ActionListener {
	private Applet  applet;
	private Toolbox toolbox;
	private ImageButton button;

	public ToolboxTestPanel(Applet applet) {
		this.applet = applet;
        Image image = applet.getImage(applet.getCodeBase(),
		                                 "gifs/ballot_box.gif");
		button = new ImageButton(image);
		button.addActionListener(this);
		add(button); 
	}
	public void addNotify() {
		super.addNotify();
		toolbox = new WidgetToolbox(applet);
	}
	public void actionPerformed(ActionEvent event) {
/* Component.getLocationOnScreen() throws NullPointerException
		ImageButton   ib   = (ImageButton)event.getSource();
		Point     loc  = button.getLocationOnScreen();
		Dimension size = button.getSize();

		toolbox.show(loc.x + size.width, loc.y + size.height);
*/
		ImageButton   ib   = (ImageButton)event.getSource();
		Frame ibFrame = Util.getFrame(ib);
		Insets insets = ibFrame.getInsets();
		Point fLoc    = ibFrame.getLocation();
		Point ibLoc   = ib.getLocation();
		Point tb      = new Point();

		tb.x = fLoc.x + ibLoc.x + insets.left + insets.right;
		tb.y = fLoc.y + ibLoc.y + insets.top + insets.bottom;
		toolbox.show(tb.x, tb.y);
	}
}

class WidgetToolbox extends Toolbox implements ActionListener {
	Applet      applet;
	ImageButton buttonButton, cboxButton,  labelButton,
	            tfieldButton, tareaButton, listButton,
				sbarButton, canvasButton;

	public WidgetToolbox(Applet applet) {
		super(Util.getFrame(applet), "Widgets");
		this.applet = applet;
		URL cb = applet.getCodeBase();

		buttonButton = 
			add(applet.getImage(cb,"gifs/button.gif"));
		cboxButton   = 
			add(applet.getImage(cb,"gifs/checkbox.gif"));
		labelButton  = 
			add(applet.getImage(cb,"gifs/label.gif"));
		tfieldButton = 
			add(applet.getImage(cb,"gifs/textField.gif"));
		tareaButton  = 
			add(applet.getImage(cb,"gifs/textArea.gif"));
		listButton   = 
			add(applet.getImage(cb,"gifs/list.gif"));
		sbarButton   = 
			add(applet.getImage(cb,"gifs/scrollbar.gif"));
		canvasButton = 
			add(applet.getImage(cb,"gifs/canvas.gif"));

		buttonButton.addActionListener(this);
		cboxButton.addActionListener  (this);
		labelButton.addActionListener (this);
		tfieldButton.addActionListener(this);
		tareaButton.addActionListener (this);
		listButton.addActionListener  (this);
		sbarButton.addActionListener  (this);
		canvasButton.addActionListener(this);
	}
	public void actionPerformed(ActionEvent event) {
		Object source = event.getSource();

		if(source instanceof ImageButton) {
			ImageButton ib = (ImageButton)source;
			if(ib == buttonButton)applet.showStatus("Button");
			if(ib == cboxButton)  applet.showStatus("Checkbox");
			if(ib == labelButton) applet.showStatus("Label");
			if(ib == tfieldButton)applet.showStatus("TextField");
			if(ib == tareaButton) applet.showStatus("TextArea");
			if(ib == listButton)  applet.showStatus("List");
			if(ib == sbarButton)  applet.showStatus("Scrollbar");
			if(ib == canvasButton)applet.showStatus("Canvas");
		}
	}
}
