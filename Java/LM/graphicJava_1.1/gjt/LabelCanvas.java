package gjt;

import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

/**
 * A selectable label.  Clients can set the insets around the 
 * label via setInsets(Insets).
 *
 * LabelCanvases fire ItemEvents when they are selected or
 * deselected.  
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Dec 31 1996
 *
 *   Possible method name changes due to deprecated methods 
 *   in 1.1.
 *
 *   New event handling model.
 * 
 * @author  David Geary
 * @see     LabelCanvasListener
 * @see     gjt.test.LabelCanvasTest
 */
public class LabelCanvas extends    Component
                         implements ItemSelectable {
    protected boolean selected = false;
	private ItemListener itemListener = null;
    private String    label;
    private Insets    insets   = new Insets(2,2,2,2);
    private Point     labelLoc = new Point(0,0);

    public LabelCanvas(String label) {
        this.label = label;

		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent event) {
				if(isSelected()) deselect();
				else             select();
			}
		});
    }
    public void paint(Graphics g) {
        if(selected == true) paintSelected  (g);
        else                 paintDeselected(g);         
    }
	public Object[] getSelectedObjects() {
		Object[] objs = null;

		if(isSelected()) {
			objs = new LabelCanvas[1];
			objs[0] = this;
		}
		return objs;
	}
    public void setInsets(Insets insets) { 
        this.insets = insets;   
        repaint();
    }
    public String getLabel() { 
		return label;                 
	}
	public void setLabel(String label) { 
		this.label = label;
	}
    public boolean isSelected() { 
		return selected;              
	}
	public void select() { 
		selected = true;  
		repaint();  
		processItemEvent();
	}
    public void deselect() { 
		selected = false; 
		repaint();  
		processItemEvent();
	}
	/**
	 * @deprecated as of JDK1.1
	 */
	public void resize(int w, int h) {
        setBounds(getLocation().x, getLocation().y, w, h); 
	}
    public void setSize(int w, int h) { 
		resize(w,h);
    }
	/**
	 * @deprecated as of JDK1.1
	 */
	public void reshape(int x, int y, int w, int h) {
        super.reshape(x, y, w, h);
		Graphics g = getGraphics();
		if(g != null) {
        	labelLoc = labelLocation(getGraphics());
			g.dispose();
		}
    }
    public void setBounds(int x, int y, int w, int h) {
		reshape(x,y,w,h);
	}
	/**
	 * @deprecated as of JDK1.1
	 */
	public Dimension minimumSize() {
        return getPreferredSize(); 
	}
    public Dimension getMinimumSize() { 
		return minimumSize();
    }
	/**
	 * @deprecated as of JDK1.1
	 */
    public Dimension preferredSize() {
		return getPreferredSize();
	}
    public Dimension getPreferredSize() {
        FontMetrics fm = getFontMetrics(getFont());
            return new Dimension(
                     insets.left + fm.stringWidth(label) + 
                     insets.right, 
                     insets.top  + fm.getHeight() + 
                     insets.bottom);
    }
    protected void paintSelected(Graphics g) {
        Point labelLoc = labelLocation(g);

        g.setColor(getForeground());
        g.fillRect(0,0,getSize().width-1,getSize().height-1);
        g.setColor(getBackground());
        g.drawString(label, labelLoc.x, labelLoc.y);
    }
	protected void paintDeselected(Graphics g) {
    	g.drawString(label, labelLoc.x, labelLoc.y);
	}
    protected String paramString() {
        return super.paramString() + ",text=" + label + 
		       (isSelected() ? " selected" : " not selected");
    }
    protected Point labelLocation(Graphics g) {
        Dimension   size = getSize();
        FontMetrics fm   = g.getFontMetrics();

        int x = (size.width/2) - (fm.stringWidth(label)/2);
        int y = (size.height/2) + (fm.getAscent()/2) - 
                                   fm.getLeading();
        return new Point(x,y);
    }
	protected void processItemEvent() {
		if(itemListener != null) {
			if(isSelected()) {
				itemListener.itemStateChanged(
					new ItemEvent(this, 
					              ItemEvent.ITEM_STATE_CHANGED,
								  this,
					              ItemEvent.SELECTED));
			}
			else {
				itemListener.itemStateChanged(
					new ItemEvent(this, 
					              ItemEvent.ITEM_STATE_CHANGED,
								  this,
					              ItemEvent.DESELECTED));
			}
		}
	}
	public synchronized void addItemListener(ItemListener l) {
		itemListener = AWTEventMulticaster.add(itemListener, l);
	}
	public synchronized void removeItemListener(ItemListener l) {
		itemListener = AWTEventMulticaster.remove(
							itemListener, l);
	}
}
