package gjt.test;

import java.awt.BorderLayout;
import java.awt.Label;
import java.awt.Panel;
import gjt.Separator;

/**
 * A Panel fitted with a BorderLayout that contains a Label 
 * (title) in the North, and a Separator in the South.
 *
 * @version 1.0, Apr 2 1996
 * @version 1.1, Nov 8 1996
 *    Took out Separator for asthetic reasons.
 *
 * @author  David Geary
 */

public class TitledPanel extends Panel {
    public TitledPanel(String title) {
        setLayout(new BorderLayout());
        add(new Label(title, Label.CENTER), "Center");
    }   
}
