package gjt;

import java.awt.*;

/**
 * CardPanel employs a BorderLayout to lay out North and 
 * Center panels; extensions of CardPanel must implement 
 * Component setViewSelector().  The component returned from 
 * Component setViewSelector() is centered in the North panel, 
 * and should contain UI controls that allow selection of the 
 * component to be displayed in the Center panel.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     IconCardPanel
 * @see     ChoiceCardPanel
 * @see     gjt.test.ChoiceCardPanelTest
 * @see     gjt.test.IconCardPanelTest
 */
public abstract class CardPanel extends Panel {
    private Panel       center;
    private CardLayout  cards;
	private Image       wallpaper;
	private Insets      centerInsets;

    abstract public Component setViewSelector();

    public CardPanel() {
		this(new Insets(0,1,1,1));
	}
	public CardPanel(Insets centerInsets) {
		this(centerInsets, BorderStyle.NONE);
	}
	public CardPanel(Insets      centerInsets, 
	                 BorderStyle borderStyle) {
		this.centerInsets = centerInsets;

        center = new CenterCardPanel(this, borderStyle);
        center.setLayout(cards = new CardLayout());
        setLayout(new BorderLayout(0,0));
        add("Center", center);
	}
    public void addNotify() {
        super.addNotify();
        add("North", setViewSelector());
    }
	public void setCenterWallPaper(Image image) {
		wallpaper = image;
	}
	public Image getWallpaper() {
		return wallpaper;
	}
	public Insets getCenterInsets() {
		return centerInsets;
	}
	protected Panel getCenterPanel() {
		return center;
	}
    protected void addView(String name, Component component) {
        center.add(name, component);
    }
    protected void showView(String name) {
        cards.show(center, name); 
    }
}

class CenterCardPanel extends Panel {
	private CardPanel      cardPanel;
	private DrawnRectangle border;

	public CenterCardPanel(CardPanel cardPanel) {
		this(cardPanel, BorderStyle.NONE);
	}
	public CenterCardPanel(CardPanel   cardPanel, 
	                       BorderStyle borderStyle) {
		this.cardPanel = cardPanel;
		border = RectangleFactory.createRectangle(this, 
		                                          borderStyle);
	}
	/**
	 * @deprecated as of JDK1.1
	 */
	public Insets insets() {
		Insets insets = cardPanel.getCenterInsets();

		if(insets.left   == 0) insets.left   = 1;
		if(insets.bottom == 0) insets.bottom = 1;
		if(insets.right  == 0) insets.right  = 1;
		return insets;
	}
	public Insets getInsets() {
		return insets();
	}
	public void paint(Graphics g) {
		Image wallpaper = cardPanel.getWallpaper();

		if(wallpaper != null) {
			Util.wallPaper(this, g, wallpaper);
		}
		if(border != null) {
			Dimension size            = getSize();
			Insets    insets          = getInsets();
			int       borderThickness = border.getThickness();
			border.setBounds(insets.left-borderThickness, 
			               insets.top-borderThickness,
			               size.width - insets.left -
						   insets.right + (2*borderThickness), 
						   size.height - insets.top -
						   insets.bottom + (2*borderThickness));
			border.paint();
			super.paint(g);  // ensure lightweights get painted
		}
		Color light, dark;
		Dimension size = getSize();
		light = getBackground().brighter().brighter();
		dark  = getBackground().darker().darker();
		g.setColor(light);
		g.drawLine(0,0,0,size.height-1);
		g.setColor(dark);
		g.drawLine(0,size.height-1,size.width-1,size.height-1);
		g.drawLine(size.width-1,size.height-1,size.width-1,0);

	}
}
