package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * A CardPanel whose Component getViewSelector() returns
 * a panel with image buttons to control the selection of the
 * panel to be displayed beneath the view selector panel.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     CardPanel
 * @see     ChoiceCardpanel
 * @see     gjt.test.IconCardPanelTest
 */
public class IconCardPanel extends CardPanel {
	private Panel            viewSelector = new Panel();
    private IconViewSelector iconPanel;

    public IconCardPanel() {
		this(new Insets(0,0,0,0));
	}
    public IconCardPanel(Insets centerInsets) {
		this(centerInsets, BorderStyle.NONE);
    }
    public IconCardPanel(Insets centerInsets,
	                     BorderStyle borderStyle) {
		super(centerInsets, borderStyle);
        iconPanel = new IconViewSelector(this);
		viewSelector.setLayout(new BorderLayout());
		viewSelector.add("Center", iconPanel);
		viewSelector.add("South",  new Separator());
	}
    public Component setViewSelector() {
        return viewSelector;
    }
    public void addImageButton(Image     image, 
                               String    name, 
                               Component component) {
        ImageButton newButton;

        iconPanel.add(
            newButton = new ImageButton(image), name);
        super.addView(name, component);

		newButton.addActionListener(iconPanel);
    }
}

class IconViewSelector extends    ExclusiveImageButtonPanel
                       implements ActionListener {
    private IconCardPanel mvp;

    public IconViewSelector(IconCardPanel panel) {
        super(Orientation.HORIZONTAL);
        setLayout(new FlowLayout());
        mvp = panel;
    }
    public void actionPerformed(ActionEvent event) {
    	ImageButton ib = (ImageButton)event.getSource(); 
        mvp.showView(getButtonName(ib)); 
    }
}
