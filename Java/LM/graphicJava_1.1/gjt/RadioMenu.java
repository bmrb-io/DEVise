package gjt;

import java.awt.Menu;
import java.awt.MenuItem;
import java.awt.CheckboxMenuItem;

public class RadioMenu extends Menu {
	public RadioMenu(String s) {
		this(s, false);
	}
    public RadioMenu(String s, boolean tearOff) {
        super(s, tearOff);
    }
    public void add(String s) {
        add(new CheckboxMenuItem(s));
    }
    public MenuItem add(MenuItem item) {
        Assert.notFalse(item instanceof CheckboxMenuItem);
        return super.add(item);
    }
	public void selectItem(int index) {
		selectItem(getItem(index));
	}
    public void selectItem(MenuItem item) {
        CheckboxMenuItem nextItem;
        int              numItems = getItemCount();

        for(int i=0; i < numItems; ++i) {
            nextItem = (CheckboxMenuItem)getItem(i);
            if(nextItem != item) nextItem.setState(false);
			else                 nextItem.setState(true);
        }
    }
}
