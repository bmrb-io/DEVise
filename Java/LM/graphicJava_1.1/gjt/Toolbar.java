package gjt;

import java.awt.*;

/**
 * A toolbar containing image buttons which are laid out to the
 * north of (horizontal) separator.<p>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     ImageButton
 * @see     ImageButtonPanel
 * @see     gjt.test.ToolbarTest
 */
public class Toolbar extends Container {
    static private int _defaultGap       = 0;
    static private int _defaultLeftInset = 0;

    private ToolbarButtonPanel buttonPanel;

    public Toolbar() {
        this(_defaultLeftInset, _defaultGap);
    }
    public Toolbar(int leftInset, int gap) {
        buttonPanel = new ToolbarButtonPanel(leftInset, gap);
        
        setLayout(new BorderLayout());
        add      (buttonPanel, "North");
        add      (new Separator(), "South");
    }
    public ImageButton add(Image image) {
        return buttonPanel.add(image);
    }
    public void add(ImageButton button) {
        buttonPanel.add(button);      
    }
    public void addSpacer(int sizeInPixels) {
        Assert.notFalse(sizeInPixels > 0);
        buttonPanel.addSpacer(sizeInPixels);
    }
}
class ToolbarButtonPanel extends ImageButtonPanel {
    private int leftInset;

    public ToolbarButtonPanel(int leftInset, int gap) {
        super(Orientation.HORIZONTAL,
              Orientation.LEFT,         
              Orientation.CENTER,
              gap);

        this.leftInset = leftInset;
    }
    public Insets getInsets() {
        return new Insets(5,leftInset,5,5);
    }
}
