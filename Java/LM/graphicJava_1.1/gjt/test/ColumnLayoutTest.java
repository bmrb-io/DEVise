package gjt.test;

import java.applet.Applet;
import java.net.URL;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * Lays out 3 image buttons, and provides controls for setting
 * orientations and gaps on the fly.<p>
 *
 * @version 1.0, April 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.ImageButton
 * @see     gjt.Box
 */
public class ColumnLayoutTest extends UnitTest {
    public String title() { 
        return "ColumnLayout Test"; 
    }
    public Panel centerPanel() {
        ColumnButtonPanel buttonPanel;
        Panel panel = new Panel();

        buttonPanel = new ColumnButtonPanel(this);

        panel.setLayout(new BorderLayout());
        panel.add("Center", buttonPanel);
        panel.add("North", new Box(new ColumnPicker(buttonPanel),
                           "Column Layout Settings"));
        return panel;
    }
}
class ColumnButtonPanel extends Panel {
    private ImageButton    one, two, three;
    private Panel          panel  = new Panel();
    private TenPixelBorder border = new TenPixelBorder(panel);

    public ColumnButtonPanel(Applet applet) {
        URL cb = applet.getCodeBase();

        one   = new ImageButton(applet.getImage(cb, 
                                        "gifs/one.gif"));
        two   = new ImageButton(applet.getImage(cb, 
                                        "gifs/two.gif"));
        three = new ImageButton(applet.getImage(cb, 
                                        "gifs/three.gif"));

        panel.setLayout(new ColumnLayout(0));
        panel.add(one);   
        panel.add(two);   
        panel.add(three); 

        setLayout(new BorderLayout());
        add      ("Center", border);
    }
    public void updateOrientations(Orientation horient, 
                                   Orientation vorient,
                                   int gap) {
        panel.setLayout(new ColumnLayout(horient, vorient, gap));
        border.getParent().validate();
    }
}

class ColumnPicker extends Panel implements ItemListener {
    private Label  horientLabel = new Label("Horizontal:");
    private Label  vorientLabel = new Label("Vertical:");
    private Label  gapLabel     = new Label("Gap:");

    private Choice hchoice   = new Choice();
    private Choice vchoice   = new Choice();
    private Choice gapChoice = new Choice();

    private ColumnButtonPanel buttonPanel;

    public ColumnPicker(ColumnButtonPanel buttonPanel) {
        Panel orientations = new Panel();
        Panel gap          = new Panel();

        this.buttonPanel = buttonPanel;
        hchoice.add("left");
        hchoice.add("center");
        hchoice.add("right");
        hchoice.select(1);

        vchoice.add("top");
        vchoice.add("center");
        vchoice.add("bottom");
        vchoice.select(1);

        gapChoice.add("0");
        gapChoice.add("5");
        gapChoice.add("10");
        gapChoice.add("15");
        gapChoice.add("20");

        orientations.add(horientLabel);  
        orientations.add(hchoice);
        orientations.add(vorientLabel);  
        orientations.add(vchoice);

        gap.add(gapLabel);               
        gap.add(gapChoice);

        add(new Box(orientations, "Orientations"));
        add(new Box(gap,          "Gap"));

		hchoice.addItemListener(this);
		vchoice.addItemListener(this);
		gapChoice.addItemListener(this);
    }
	public void itemStateChanged(ItemEvent event) {
        String horient, vorient;
        int    gap;

        horient = hchoice.getSelectedItem();
        vorient = vchoice.getSelectedItem();
        gap     = 
        (new Integer(gapChoice.getSelectedItem())).intValue();

        buttonPanel.updateOrientations(
            Orientation.fromString(horient), 
            Orientation.fromString(vorient), gap);
    }
}
