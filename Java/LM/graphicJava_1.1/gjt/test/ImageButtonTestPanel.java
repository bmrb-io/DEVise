package gjt.test;

import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

/**
 * 2 ImageButtons, one springy and the other sticky.<p>
 *
 * Both ImageButtons come with an awt.Choice that is used to 
 * enable/disable the ImageButton it's associated with.<p>
 *
 * ImageButtonEvents, along with mouse enter and mouse exit
 * events for the two image buttons are printed out.<p>
 *
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.ImageButton
 * @see     gjt.ImageButtonEvent
 * @see     gjt.SpringyImageButtonController
 * @see     gjt.StickyImageButtonController
 */
public class ImageButtonTestPanel extends Panel
                                  implements ActionListener,
                                  ItemListener {
	private Applet      applet;
    private ImageButton springyButton, stickyButton;
    private Choice      springyChoice, stickyChoice;

    public ImageButtonTestPanel(Applet applet) {
        Image              image;
        Box                springyBox, stickyBox;

		this.applet = applet;

        GridBagLayout      gbl = new GridBagLayout();
        GridBagConstraints gbc = new GridBagConstraints();

        image = applet.getImage(applet.getCodeBase(), 
		                        "gifs/filmstrip.gif");

        springyButton = new ImageButton();
        springyChoice = new Choice     ();
        stickyButton  = new ImageButton(image);
        stickyChoice  = new Choice     ();

		springyChoice.add("Enabled");
		springyChoice.add("Disabled");

		stickyChoice.add("Enabled");
		stickyChoice.add("Disabled");

        image = applet.getImage(applet.getCodeBase(), 
		                        "gifs/filmstrip.gif");

		springyButton.setImage(image);

		stickyButton.addActionListener(this);
		springyButton.addActionListener(this);

		springyChoice.addItemListener(this);
		stickyChoice.addItemListener(this);

		springyButton.setBubbleHelp("Springy Button");
		stickyButton.setBubbleHelp("Sticky Button");

		springyButton.setBubbleInterval(5000);
		stickyButton.setBubbleInterval(500);

		IBTPMouseListener ml = 
			new IBTPMouseListener(applet, springyButton,
		                          stickyButton);

		springyButton.addMouseListener(ml);
		stickyButton.addMouseListener(ml);

		stickyButton.setListener(
			new StickyImageButtonListener());

        setLayout(gbl);

        gbc.anchor    = GridBagConstraints.NORTH;
        springyBox    = new Box(springyButton, "Spring");
        gbc.insets    = new Insets(10,0,0,0);
        gbl.setConstraints(springyBox, gbc); 
		add(springyBox);

        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.insets    = new Insets(45,10,0,0);
        gbl.setConstraints(springyChoice, gbc); 
        add(springyChoice);

        gbc.anchor    = GridBagConstraints.NORTH;
        gbc.gridwidth = 1;
        stickyBox     = new Box(stickyButton, "Stick");
        gbc.insets    = new Insets(10,0,0,0);
        gbc.weighty   = 1.0;
        gbl.setConstraints(stickyBox, gbc); 
		add(stickyBox);

        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.insets    = new Insets(45,10,0,0);
        gbl.setConstraints(stickyChoice, gbc); 
        add(stickyChoice);
    }
	public void itemStateChanged(ItemEvent event) {
		String      s      = null;
		Choice      choice = (Choice)event.getSource();
		int         index  = choice.getSelectedIndex();
		ImageButton button = choice == stickyChoice ? 
		                     stickyButton : springyButton;

		if(button == stickyButton) s = "Sticky Button ";
		else                       s = "Springy Button ";

		if(index == 0) {
			button.setEnabled(true);
			s += "Enabled";
		}
		else {
			button.setEnabled(false);
			s += "Disabled";
		}
		applet.showStatus(s);
	}
	public void actionPerformed(ActionEvent event) {
		String status = new String();
		Object o = event.getSource();

		if(o == stickyButton)       status = "Sticky Fired";
		else if(o == springyButton) status = "Springy Fired";
		applet.showStatus(status);
    }
}
class IBTPMouseListener extends MouseAdapter {
	private Applet      applet;
	private ImageButton springy, sticky;

	public IBTPMouseListener(Applet applet, 
	                         ImageButton springy,
							 ImageButton sticky) {
		this.applet  = applet;
		this.springy = springy;
		this.sticky  = sticky;
	}
	public void mouseEntered(MouseEvent event) {
		String s = getButtonType(event.getSource());
		applet.showStatus(s + " ImageButton Entered");
	}
	public void mouseExited(MouseEvent event) {
		String s = getButtonType(event.getSource());
		applet.showStatus(s + " ImageButton Exited");
	}
	private String getButtonType(Object button) {
		if(button == springy) return "Springy";
		else                  return "Sticky";
	}
}
