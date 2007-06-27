package JavaScreen.UI;
import java.awt.event.*;
import javax.swing.*;

class DEViseToolBarButtonAction extends Object implements ActionListener {
	
	private DEViseToolBar toolbar;
	
	public DEViseToolBarButtonAction(DEViseToolBar tb) {
		toolbar = tb;
	}
	public void actionPerformed(ActionEvent e) {
		DEViseToolBarButton button = (DEViseToolBarButton)e.getSource();
		toolbar.setSelectedButton(button);
/*		JOptionPane.showMessageDialog(null, "The button " + button.getToolTipText() + " was pressed" );*/
	}
}