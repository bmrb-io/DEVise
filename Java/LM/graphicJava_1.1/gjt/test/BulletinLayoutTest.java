package gjt.test;

import java.applet.Applet;
import java.awt.*;
import gjt.*;

/**
 * One of each basic AWT component, plus a gjt.ImageButton
 * are laid out by a BulletinLayout.<p>
 *
 * Note that we override addNotify() for the
 * BulletinLayoutTestPanel because awt.Lists must have a
 * peer created before they can be moved or resized.  Also,
 * awt.Choices must have a peer created before they can be
 * resized.<p>
 *
 * The applet draws a blue rectangle around the preferred
 * size for the BulletinLayoutTestPanel, and also prints
 * the preferred and minimum sizes in the applets status
 * area.<p>
 *
 * @version 1.0, Wed Oct 23, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.BulletinLayout
 */
public class BulletinLayoutTest extends UnitTest {
    public String title() { return "BulletinLayout Test"; }
    public Panel centerPanel() { 
        return new BulletinLayoutTestPanel(this); 
    }
}

class BulletinLayoutTestPanel extends Panel {
	private Applet         applet;
	private Button         button;
	private Choice         choice;
	private TextField      textfield;
	private Checkbox       checkbox;
	private List           list;
	private Label          label;
	private Scrollbar      scrollbar;
	private Border         borderAroundCanvas;
	private ImageButton    imageButton;
	private DrawnRectangle dr;

	public BulletinLayoutTestPanel(Applet applet) {
		this.applet = applet;

		createComponents();
		setLayout(new BulletinLayout());

		add(button);             add(choice);
		add(borderAroundCanvas); add(textfield);
		add(checkbox);           add(scrollbar);
		add(list);               add(label);
		add(imageButton); 

		button.setLocation(10,10);
		choice.setLocation(10,35);
		label.setLocation(230, 120);
		checkbox.setLocation(120, 120);

		borderAroundCanvas.setLocation(10, 75);
		borderAroundCanvas.setSize(100,100);

		textfield.setLocation(120,10);
		textfield.setSize(100,100);

		scrollbar.setLocation(120, 160);
		scrollbar.setSize(200,100);

		imageButton.setLocation(375, 10);
		imageButton.setSize(100,100);

		list.setLocation(230, 10);
		list.setSize(100,100);

	}
	public Insets getInsets() {
		return new Insets(10,10,10,10);
	}
	public void paint(Graphics g) {
		Dimension ps = getPreferredSize();
		Dimension ms = getMinimumSize();

		paintPreferredSizeBorder(ps);
		super.paint(g);
		applet.showStatus("Minimum Size:  " + ms.width + ","
		                  + ms.height + "   " + 
		                  "Preferred Size:  " + ps.width + ","
		                  + ps.height);
	}
	private void paintPreferredSizeBorder(Dimension ps) {
		dr.setSize(ps.width, ps.height);
		dr.paint();
	}
	private void createComponents() {
		button         = new Button("A Button");
		choice         = new Choice();
		textfield      = new TextField("textfield");
		checkbox       = new Checkbox("Checkbox");
		list           = new List();
		label          = new Label("A Label");
		scrollbar      = new Scrollbar(Scrollbar.HORIZONTAL);
		borderAroundCanvas = new EtchedBorder(new Canvas());
		imageButton        = new ImageButton(
						 applet.getImage(applet.getCodeBase(),
						                "gifs/two_cents.gif"));
		dr = new DrawnRectangle(this);
		dr.setLocation(0,0);
		dr.setThickness(2);
		dr.setLineColor(Color.blue);

		choice.add("item one");
		choice.add("item two");
		choice.add("item three");
		choice.add("item four");

		list.add("list item one");
		list.add("list item two");
		list.add("list item three");
		list.add("list item four");
		list.add("list item five");
		list.add("list item six");
		list.add("list item seven");
		list.add("list item eight");
		list.add("list item nine");

		scrollbar.setValues(0,100,0,1000);
	}

}
