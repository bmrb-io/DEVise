package gjt.test;

import java.awt.*;
import java.awt.event.*;
import gjt.ColorChoice;
import gjt.DrawingPanel;
import gjt.Separator;
import gjt.RowLayout;
import gjt.rubberband.*;

public class RubberbandTestPanel extends Panel {
    private DrawingPanel drawingPanel;
    private ChoicePanel  choicePanel;

    public RubberbandTestPanel() {
        drawingPanel = new DrawingPanel();
        choicePanel  = new ChoicePanel(drawingPanel);

        setLayout(new BorderLayout());
        add(choicePanel, "North");
        add(drawingPanel, "Center");
    }
	public Dimension getPreferredSize() {
		return new Dimension(
			choicePanel.getPreferredSize().width, 500);
	}
}
class ChoicePanel extends Panel implements ItemListener {
    private DrawingPanel drawingPanel;
    private ColorChoice  colorChoice  = new ColorChoice();
    private Checkbox     fillCheckbox = new Checkbox();

    public ChoicePanel(DrawingPanel drawingPanel) {
        Panel    choicePanel     = new Panel();
        Choice   geometricChoice = new Choice();

        this.drawingPanel = drawingPanel;
    
        geometricChoice.add("Lines");
        geometricChoice.add("Rectangles");
        geometricChoice.add("Ellipses");

		geometricChoice.addItemListener(this);
		colorChoice.addItemListener(this);
		fillCheckbox.addItemListener(this);

        choicePanel.setLayout(new RowLayout(10));
        choicePanel.add(new Label("Shape:"));
        choicePanel.add(geometricChoice);
        choicePanel.add(new Label("Color:"));
        choicePanel.add(colorChoice);
        choicePanel.add(new Label("Fill:"));
        choicePanel.add(fillCheckbox);

        setLayout(new BorderLayout());
        add("Center", choicePanel);
        add("South",  new Separator());
    }
    public void itemStateChanged(ItemEvent event) {
		Object target = event.getSource();

        if(target instanceof Checkbox) {
            drawingPanel.setFill(fillCheckbox.getState());
        }
        else if(target instanceof Choice) {
			Choice choice = (Choice)target;

			if(choice.getSelectedItem().equals("Lines")) {
                drawingPanel.drawLines();
            }
			else if(
			 choice.getSelectedItem().equals("Rectangles")) {
                drawingPanel.drawRectangles();
			}
			else 
			if(choice.getSelectedItem().equals("Ellipses")) {
                drawingPanel.drawEllipses  ();
			}
            drawingPanel.setColor(colorChoice.getColor());
        }
    }
    public Insets getInsets() { return new Insets(5,0,5,0); }
}
