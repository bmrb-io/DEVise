package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

public class SplashTest extends Frame {
	SplashScreen splash;

	static public void main(String[] args) {
		Toolkit tk = Toolkit.getDefaultToolkit();
		new SplashTest(tk.getImage("gifs/saint.gif"));
	}
	public SplashTest(Image image) {
		splash = new BorderedSplash(this, image);

		splash.show();
		splash.toFront();

		try                { Thread.currentThread().sleep(10000); }
		catch(Exception e) { e.printStackTrace(); }
		splash.dispose();
		System.exit(0);

		addMouseListener(new MouseAdapter() {
			public void mousePressed(Event event) {
				splash.dispose();
				System.exit(0);
			}
		});
	}
}
class BorderedSplash extends SplashScreen {
	private Border       blackBorder;
	private ThreeDBorder threeDBorder;

	public BorderedSplash(Frame frame, Image image) {
		super(frame, image);
		threeDBorder = new ThreeDBorder(new ImageCanvas(image));
		blackBorder  = new Border(threeDBorder);
		blackBorder.setLineColor(Color.black);
		add("Center", blackBorder);
	}
}
