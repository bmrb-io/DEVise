package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * ChoiceCardPanel is an extension of CardPanel which presents
 * an awt.Choice for selecting the panel to be displayed
 * in the center panel.<p>
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Nov 8 1996
 *
 *  Renamed instance variable setViewSelector to viewSelector 
 *  to work around Microsoft compiler bug.
 *
 * @author  David Geary
 * @see     CardPanel
 * @see     IconCardPanel
 * @see     gjt.test.IconCardPanelTest
 * @see     gjt.test.ChoiceCardPanelTest
 */
public class ChoiceCardPanel extends CardPanel {
	private Panel              viewSelector = new Panel();
    private ChoiceViewSelector choicePanel;

	public ChoiceCardPanel() {
		this(new Insets(0,0,0,0));
	}
    public ChoiceCardPanel(Insets centerInsets) {
		this(centerInsets, BorderStyle.NONE);
    }
    public ChoiceCardPanel(Insets      centerInsets,
	                       BorderStyle cardBorderStyle) {
		super(centerInsets, cardBorderStyle);
        choicePanel = new ChoiceViewSelector(this);
		viewSelector.setLayout(new BorderLayout());
		viewSelector.add("Center", choicePanel);
		viewSelector.add("South",  new Separator());
	}
    public Component setViewSelector() {
        return viewSelector;
    }
    public void addChoice(String    name, 
                          Component component) {
        choicePanel.add(name);
        super.addView(name, component);
    }
}

class ChoiceViewSelector extends Panel implements ItemListener {
    private ChoiceCardPanel mvp;
    private Choice          choice;

    public ChoiceViewSelector(ChoiceCardPanel panel) {
        setLayout(new FlowLayout());
        add(choice = new Choice());
        mvp = panel;
		choice.addItemListener(this);
    }
    public void add(String name) {
        choice.add(name);
    }
	public void itemStateChanged(ItemEvent ie) {
        mvp.showView(choice.getSelectedItem());
	}
}
