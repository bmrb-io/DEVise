package gjt.test;

import java.awt.*;

import gjt.*;

/**
 * Activating the button causes the FontDialog to be displayed.  
 * Selecting a font from the FontDialog causes the button to 
 * use the selected font.<p>
 *
 * This unit test overrides FontDialog to reset the labels 
 * displayed in the buttons, and to reset the list of font 
 * sizes displayed.  See FontDialog for a discussion of the 
 * overridden methods.<p>
 *
 *<em>Note:  The FontDialog takes forever to come up in 
 * Netscape.</em>
 *
 * @version 1.0, Apr 25, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.Util
 * @see     gjt.FontDialog
 * @see     gjt.DialogClient
 */
 
public class FontDialogTest extends UnitTest {
    public String title() { return "Font Dialog Test"; }
    public Panel centerPanel() { 
        return new FontDialogTestPanel(); 
    }
}
