package gjt.test;

import java.net.URL;
import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

public class DoubleBufferedContainerTest extends UnitTest {
	public String title() {
		return "DoubleBufferedContainerTest:  " +
		       "Drag Lightweights Around!";
	}
	public Panel centerPanel() {
        return new DBCTestPanel(this); 
    }
}
class DBCTestPanel extends Panel {
	private DoubleBufferedContainer container;

	public DBCTestPanel(Applet applet) {
		URL b = applet.getCodeBase();

		Image mandrill   =applet.getImage(b,"gifs/mandrill.jpg");
		Image background =applet.getImage(b,"gifs/paper.gif");
		Image gj         =applet.getImage(b,"gifs/gjMedium.gif");
		Image skelly     =applet.getImage(b,"gifs/skelly.gif");
		Image quarter    =applet.getImage(b,"gifs/quarter.gif");

		container = new DoubleBufferedContainer(background);
		container.setLayout(new FlowLayout());
		container.add(new Lightweight(skelly));
		container.add(new Lightweight(gj));
		container.add(new Lightweight(quarter));
		container.add(new Lightweight(mandrill));

		setLayout(new BorderLayout());
		add("Center", container);
	}
	public void update(Graphics g) {
		paint(g);
	}
}
class Lightweight extends Component {
	private LightweightDragger dragger;
	private Image image;

	public Lightweight(Image image) {
		this.image = image;
		Util.waitForImage(this, image);

		dragger = new LightweightDragger(this);
		addMouseListener      (dragger);
		addMouseMotionListener(dragger);
	}
	public void paint(Graphics g) {
		if(isVisible()) {
			g.drawImage(image, 0, 0, this);
		}
	}
	public Dimension getPreferredSize() {
		return new Dimension(image.getWidth(this),
		                     image.getHeight(this));
	}
}
class LightweightDragger extends    MouseAdapter 
                         implements MouseMotionListener {
	private Lightweight lightweight;
	private Point       press;
	private boolean     dragging = false;

	public LightweightDragger(Lightweight lightweight) {
		this.lightweight = lightweight;
	}
	public void mousePressed(MouseEvent event) {
		press    = new Point(event.getX(),event.getY());
		dragging = true;
	}
	public void mouseReleased(MouseEvent event) {
		DoubleBufferedContainer c;
		c = (DoubleBufferedContainer)lightweight.getParent();
		c.paintComponents(lightweight.getBounds(), true);
		dragging = false;
	}
	public void mouseMoved(MouseEvent event) {
	}
	public void mouseDragged(MouseEvent event) {
		if(dragging) {
			DoubleBufferedContainer c;
			Point loc = lightweight.getLocation();
			Point pt  = new Point();

			pt.x = event.getX() + loc.x - press.x;
			pt.y = event.getY() + loc.y - press.y;
			c = (DoubleBufferedContainer)lightweight.getParent();
			c.moveComponent(lightweight, pt);
		}
	}
}
