package gjt;

import java.awt.*;
import java.awt.event.*;

public abstract class Toolbox extends Frame
                              implements WindowListener {
	private GridBagLayout      gbl = new GridBagLayout();
	private GridBagConstraints gbc = new GridBagConstraints();
	private Panel              panel = new Panel();
	private Frame              frame;
	private int                cols;
	private int                ibCnt = 1;

	public Toolbox(Frame frame, String title) {
		this(frame, title, 1);
	}
	public Toolbox(Frame frame, String title, int cols) {
		super(title);
		this.frame = frame;
		this.cols  = cols;

		panel.setLayout(gbl);

		gbc.anchor     = GridBagConstraints.NORTHWEST;
		gbc.gridwidth  = 1;
		gbc.gridheight = 1;
		gbc.fill = GridBagConstraints.HORIZONTAL;

		add("Center", panel);
		addWindowListener(this);
	}
	public Frame getFrame() {
		return frame;
	}
	public ImageButton add(Image image) {
		Assert.notNull(image);
		ImageButton button = new ImageButton(image);

		if(ibCnt % cols == 0) 
			gbc.gridwidth = GridBagConstraints.REMAINDER;
		else
			gbc.gridwidth = 1;

		++ibCnt;

		gbl.setConstraints(button, gbc);
		panel.add(button);
		return button;
	}
	public void show(int x, int y) {
		addNotify();
		pack();
		setLocation(x + getInsets().left, y + getInsets().top);

		if(! isShowing()) super.setVisible(true);
		else              toFront();
	}
	public void setVisible(boolean b) {
		if(b) {
			Point pos = showPosition();
			show(pos.x, pos.y);
		}
	}
	public void windowActivated  (WindowEvent event) { }
	public void windowDeactivated(WindowEvent event) { }
	public void windowClosed     (WindowEvent event) { }
	public void windowIconified  (WindowEvent event) { }
	public void windowDeiconified(WindowEvent event) { }
	public void windowOpened     (WindowEvent event) { }
	public void windowClosing(WindowEvent event) {
		setVisible(false);
		dispose();
	}
	private Point showPosition() {
		Dimension sz = getSize();
		int       x = frame.getLocation().x - sz.width;
		int       y = frame.getLocation().y;
		return new Point(x,y);
	}
}
