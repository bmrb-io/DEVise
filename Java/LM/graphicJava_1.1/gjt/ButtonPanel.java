package gjt;

import java.awt.*;

/**
 * Button panel employs a BorderLayout to lay out a Separator in 
 * the north, and a Panel to which Buttons are added in the 
 * center.<p>
 *
 * Buttons may be added to the panel via two methods:
 * <dl>
 * <dd> void   add(Button)
 * <dd> Button add(String)
 * </dl>
 * <p>
 *
 * Button add(String) creates a Button and adds it to the
 * panel, then returns the Button created, as a convenience to
 * clients so that they do not have to go through the pain
 * and agony of creating an ImageButton.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     WorkDialog
 */
public class ButtonPanel extends Panel {
    Panel     buttonPanel = new Panel();
    Separator separator   = new Separator();

	public ButtonPanel() {
		this(Orientation.CENTER);
	}
    public ButtonPanel(Orientation orientation) {
		int buttonPanelOrient = FlowLayout.CENTER;
        setLayout(new BorderLayout(0,5));

		if(orientation == Orientation.CENTER)
			buttonPanelOrient = FlowLayout.CENTER;
		else if(orientation == Orientation.RIGHT)
			buttonPanelOrient = FlowLayout.RIGHT;
		else if(orientation == Orientation.LEFT)
			buttonPanelOrient = FlowLayout.LEFT;

		buttonPanel.setLayout(new FlowLayout(buttonPanelOrient));
        add(separator, "North");
        add(buttonPanel, "Center");
    }
    public void add(Button button) {
        buttonPanel.add(button);
    }
    public Button add(String buttonLabel) {
        Button addMe = new Button(buttonLabel);
        buttonPanel.add(addMe);
        return addMe;
    }
    protected String paramString() {
        return super.paramString() + "buttons=" +
        getComponentCount();
    }
}
