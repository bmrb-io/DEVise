package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * Creates 10 bordered Components:
 * <dl>
 * <dd> A Canvas (click in canvas to depress/raise the border).
 * <dd> A Label with an etched out border. 
 * <dd> A TextField with an inset 3D border.
 * <dd> A CheckBox with a default border.
 * <dd> A List with a raised 3D border.
 * <dd> A Choice with an etched in border.
 * <dd> A Box with a raised 3D border.
 * <dd> An ImageButton with a thick, red border.
 * <dd> An AWT Button with a cyan border.
 * <dd> A TextArea with a blue default-width border.
 * </dl>
 *
 * @version 1.0, April 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.Border
 * @see     gjt.ThreeDBorder
 * @see     gjt.EtchedBorder
 */
public class BorderTest extends UnitTest {
    public String title() { 
        return "Border Test";             
    }
    public Panel  centerPanel() { 
        return new BorderTestPanel(this); 
    }
}

class BorderTestPanel extends Panel {
    TextField tf = new TextField(
            "Inset TextField: border 5 pixels, gap 5 pixels ");
    ThreeDBorder threeDBorder;
    EtchedBorder etchedLabel;
    Border       border;

    public BorderTestPanel(Applet applet) {
        setLayout(new FlowLayout(FlowLayout.CENTER, 10, 10));

        add(new BorderedCanvas());
        add(etchedLabel = 
                new EtchedBorder(new Label("Etched Label")));
        add(threeDBorder = new ThreeDBorder(tf, 5, 5));
        add(new Border(new Checkbox("Check Me Out")));
        add(makeThreeDBorderedList      ());
        add(makeEtchedBorderedChoice    ());
        add(makeThreeDBorderedCheckboxes());
        add(makeBorderedImageButton     (applet));
        add(makeBorderedAWTButton       ());
        add(makeBorderedTextArea        ());

        threeDBorder.inset();
        etchedLabel.etchedOut();
    }
    private Border makeThreeDBorderedList() {
        List list = new List(10, true);

        list.add("One");
        list.add("Two");
        list.add("Three");
        list.add("Four");
        list.add("Five");
        list.add("Six");
        list.add("Seven");
        list.add("Eight");
        list.add("Nine");
        list.add("Ten");
        list.add("Eleven");
        list.add("Twelve");
        list.add("Thirteen");
        list.add("Fourteen");
        list.add("Fiveteen");
        list.add("Sixteen");
        list.add("Seventeen");
        list.add("Eightteen");
        list.add("Nineteen");
        list.add("Twenty");

        return new ThreeDBorder(list);
    }
    private Border makeEtchedBorderedChoice() {
        Choice choice = new Choice();

        choice.add("Toadies");
        choice.add("SilverChair");
        choice.add("Rug Burns");
        choice.add("Cracker");
        choice.add("Seven Mary Three");
        choice.add("Dishwalla");
        choice.add("Blues Traveler");
        choice.add("BottleRockets");
        choice.add("SpaceHog");
        
        return new EtchedBorder(choice);
    }
    private Border makeBorderedImageButton(Applet applet) {
        Image  snail;
        Border border;

        snail = applet.getImage(applet.getCodeBase(),
                                   "gifs/snail.gif");
        border  = new Border(new ImageButton(snail), 10);
        border.setLineColor(Color.red);

        return border;
    }
    private Border makeBorderedAWTButton() {
        Button button;
        Border cyanBorder, blackBorder;

        button = new Button("Button Inside Two Borders");
        cyanBorder = new Border(button, 7);
        cyanBorder.setLineColor(Color.cyan);

        blackBorder = new Border(cyanBorder);
		blackBorder.setLineColor(Color.black);

        return blackBorder;
    }
    private Border makeThreeDBorderedCheckboxes() {
        Panel         panel = new Panel();
        Box           box   = new Box(panel, "Options");
        CheckboxGroup group = new CheckboxGroup();

        panel.setLayout(new GridLayout(3,0));
        panel.add(new Checkbox("bordered",    group, false));
        panel.add(new Checkbox("transparent", group, false));
        panel.add(new Checkbox("continuous",  group, true));

        return new ThreeDBorder(box, 4);
    }
    private Border makeBorderedTextArea() {
        Border border;

        border = new Border(
                 new TextArea("Blue Bordered TextArea", 5, 30));
        border.setLineColor(Color.blue);

        return border;
    }
}

class BorderedCanvas extends ThreeDBorder {
    public BorderedCanvas() {
        super(new TestCanvas());
		getComponent().addMouseListener(new MouseAdapter() {
    		public void mousePressed(MouseEvent event) {
				TestCanvas testCanvas = 
					(TestCanvas) event.getSource();
		
				testCanvas.toggleTextOn();
        		if(isRaised()) {
					paintInset ();
					testCanvas.drawCenteredString();
				}
        		else {          
					paintRaised();
					testCanvas.repaint();
				}
    		}
		});
	}
}

class TestCanvas extends Canvas {
    private String  centered = new String ("Red Centered Text");
	private boolean textOn   = false;

    public void paint(Graphics g) {
        String      canvas = "Canvas";
        Dimension   size   = getSize();
        FontMetrics fm     = g.getFontMetrics();
        String      click  = "Click Me";

        g.drawString(canvas, (size.width/2) - 
                             (fm.stringWidth(canvas)/2),
                             fm.getHeight() - fm.getDescent());

        g.drawString(click, (size.width/2) - 
                             (fm.stringWidth(click)/2),
                             size.height - fm.getHeight() + 
                             fm.getAscent());
    }
	public void toggleTextOn() {
		textOn = textOn ? false : true;
	}
	public void setTextOn(boolean textOn) {
		this.textOn = textOn;	
	}
	public boolean getTextOn() {
		return textOn;
	}
    public Dimension getPreferredSize() {
		Graphics g = getGraphics();
        FontMetrics fm = g.getFontMetrics();
		g.dispose();
        return new Dimension(fm.stringWidth(centered)+10, 100);
    }
	protected void drawCenteredString() {
        Dimension   size = getSize();
		Graphics    g    = getGraphics();
        FontMetrics fm   = g.getFontMetrics();

        g.setColor(Color.red);
        g.drawString(centered, 
            size.width/2-(fm.stringWidth(centered)/2),
            size.height/2 - (fm.getHeight()/2) + 
            fm.getAscent());

		g.dispose();
	}
}
