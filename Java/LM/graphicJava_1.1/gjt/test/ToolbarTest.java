package gjt.test;

import java.net.URL;
import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;
import gjt.*;

/**
 * Toolbar test has a toolbar across the top and an
 * exclusive image button panel along the west.<p>
 *
 * @version 1.0, Nov 8, 1996
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.ExclusiveImageButtonPanel
 * @see     gjt.ImageButton
 * @see     gjt.Toolbar
 */
public class ToolbarTest extends UnitTest {
    public String title() { 
        return "Toolbar/ImageButtonPanel Test"; 
    }
    public Panel centerPanel() { 
        return new ToolbarTestPanel(this);      
    }
}
class ToolbarTestPanel extends Panel implements ActionListener {
	Applet      applet;
    ImageButton newButton,   openButton, diskButton, 
                printButton, cutButton,  copyButton,
                pasteButton;

    public ToolbarTestPanel(Applet applet) {
		this.applet = applet;
        setLayout(new BorderLayout());  
        add(makeToolbar(applet, applet.getCodeBase()), "North");
        add(makePalette(applet, applet.getCodeBase()), "West");
    }
	public void actionPerformed(ActionEvent event) {
		Object target = event.getSource();
		String s = null;

        if(target == newButton)   s = "New Button Activated";
        if(target == openButton)  s = "Open Button Activated";
        if(target == diskButton)  s = "Disk Button Activated";
        if(target == printButton) s = "Print Button Activated";
        if(target == cutButton)   s = "Cut Button Activated";
        if(target == copyButton)  s = "Copy Button Activated";
        if(target == pasteButton) s = "Paste Button Activated";

		applet.showStatus(s);
    }
    private Toolbar makeToolbar(Applet app, URL cb) {
        Toolbar tb  = new Toolbar(10, 0);

        newButton   = tb.add(app.getImage(cb, "gifs/new.gif"));
        openButton  = tb.add(app.getImage(cb, "gifs/open.gif"));
        diskButton  = tb.add(app.getImage(cb, "gifs/disk.gif"));

		newButton.setBubbleHelp("create");
		newButton.setBubbleInterval(0);
		openButton.setBubbleHelp("open");
		diskButton.setBubbleHelp("save");

        tb.addSpacer(newButton.getPreferredSize().width);

        printButton = tb.add(app.getImage(cb, "gifs/print.gif"));

        tb.addSpacer(newButton.getPreferredSize().width);

        cutButton   = tb.add(app.getImage(cb, "gifs/cut.gif"));
        copyButton  = tb.add(app.getImage(cb, "gifs/copy.gif"));
        pasteButton = tb.add(app.getImage(cb, "gifs/paste.gif"));

		cutButton.setBubbleHelp  ("cut it out");
		copyButton.setBubbleHelp ("copy");
		pasteButton.setBubbleHelp("paste");
		printButton.setBubbleHelp("print");

		newButton.addActionListener  (this);
		openButton.addActionListener (this);
		diskButton.addActionListener (this);
		printButton.addActionListener(this);
		cutButton.addActionListener  (this);
		copyButton.addActionListener (this);
		pasteButton.addActionListener(this);

		return tb;
    }
    private Panel makePalette(Applet app, URL cb) {
        ExclusiveImageButtonPanel iconPalette;
		ImageButton               ballotButton;
        Panel                     iconPalettePanel = new Panel();

        iconPalette = new ExclusiveImageButtonPanel(
                              Orientation.VERTICAL,
                              Orientation.CENTER,
                              Orientation.TOP, 10);

		ballotButton =
        iconPalette.add(app.getImage(cb,"gifs/ballot_box.gif"));

        iconPalette.add(app.getImage(cb,"gifs/palette.gif"));
        iconPalette.add(app.getImage(cb,"gifs/light_bulb1.gif"));
        iconPalette.add(app.getImage(cb,"gifs/Dining.gif"));
        iconPalette.add(app.getImage(cb,"gifs/scissors.gif"));
        iconPalette.add(app.getImage(cb,"gifs/tricycle.gif"));

        iconPalettePanel = new Panel();
        iconPalettePanel.setLayout(new BorderLayout());
        iconPalettePanel.add      (iconPalette, "Center");
        iconPalettePanel.add      (new Separator(), "East");
        return iconPalettePanel;
    }
}
