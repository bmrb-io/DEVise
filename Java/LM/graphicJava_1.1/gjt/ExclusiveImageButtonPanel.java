package gjt;
import java.awt.*;
import java.awt.Image;
import java.awt.event.*;

/**
 * An ImageButtonPanel which fits all of its ImageButtons with 
 * a StickyImageButtonLIstener.
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     ImageButton
 * @see     ImageButtonPanel
 * @see     gjt.test.ToolbarTest
 */
public class ExclusiveImageButtonPanel extends ImageButtonPanel
									   implements ActionListener {
	private EventQueue q = new EventQueue();
	private ImageButton insetButton;
	private ImageButtonListener stickyListener = 
					new StickyImageButtonListener();

    public ExclusiveImageButtonPanel(Orientation orient) {
        this(orient, 5);
    }
    public ExclusiveImageButtonPanel(Orientation orient, 
                                     int gap) {
        super(orient, gap);
    }
    public ExclusiveImageButtonPanel(Orientation orient, 
                                     Orientation horient,
                                     Orientation vorient,
                                     int gap) {
        super(orient, horient, vorient, gap);
    }
	public void actionPerformed(ActionEvent event) {
		ImageButton button = (ImageButton)event.getSource();

		if(button.isRaised()) {
			insetButton = null;
		}
		else {
			if(insetButton != null && insetButton != button) {
				MouseEvent mouseEvent = 
					new MouseEvent(insetButton,
								   MouseEvent.MOUSE_PRESSED,
								   System.currentTimeMillis(),
								   0, 0, 0, 1, false); 
										
				q.postEvent(mouseEvent);
			}
			insetButton = button;
		}
	}
    public void add(ImageButton button) {
        super.add(button);
		attachListeners(button);
    }
    public ImageButton add(Image image) {
        ImageButton button = super.add(image);
		attachListeners(button);
        return button;
    }
    public ImageButton add(Image image, String name) {
        ImageButton button = super.add(image, name);
		attachListeners(button);
        return button;
    }
	protected void attachListeners(ImageButton button) {
		button.setListener(stickyListener);
		button.addActionListener(this);
	}
}
