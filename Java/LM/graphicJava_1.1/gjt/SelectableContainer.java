package gjt;

import java.awt.*;
import java.awt.event.*;

/*
 * SelectableContainer is an itemSelectable, so you can add
 * and remove item listeners to/from it, and you can get the
 * selected objects it contains.  You can also set/get whether 
 * the container itself is selected.
 *
 * getSelectedObjects() is given a default implementation that
 * returns null if the container is not selected, or an
 * array of objects with itself occupying the only slot in the
 * array if the container is selected.  
 *
 * If you want your container to return an array of enclosed
 * selected objects, you can override getSelectedObjects().
 */
public class SelectableContainer extends    Container 
                                 implements ItemSelectable {
	private boolean      isSelected;
	private ItemListener itemListener = null;
	private Object[]     selected = new Object[1];

	public void select() {
		isSelected = true;
	}
	public void deselect() {
		isSelected = false;
	}
	public boolean isSelected() {
		return isSelected;
	}
	public Object[] getSelectedObjects() {
		Object[] objs;

		if(isSelected()) {
			selected[0] = this;
			objs = selected;			
		}
		else {
			objs = null;
		}
		return objs;
	}
	public void processItemEvent(ItemEvent event) {
		itemListener.itemStateChanged(event);
	}
	public void addItemListener(ItemListener l) {
		itemListener = AWTEventMulticaster.add(itemListener, l);
	}
	public void removeItemListener(ItemListener l) {
		itemListener = AWTEventMulticaster.remove(itemListener, l);
	}
}
