/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class NiceCardPanel.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

// Class
/**
 * The <CODE>NiceCardPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class NiceCardPanel extends CardPanel implements ActionListener
{
  private Panel m_viewSelector;
  private ChoiceToolbar m_selector;

  public NiceCardPanel() {
    this(new Insets(0,0,0,0));
  }
  public NiceCardPanel(Insets insets) {
    this(insets, BorderStyle.NONE);
  }
  public NiceCardPanel(Insets insets, BorderStyle borderstyle) {
    super(insets, borderstyle);
    m_viewSelector = new Panel(new BorderLayout());
    m_selector = new ChoiceToolbar("Selector", Orientation.HORIZONTAL);
    m_viewSelector.add(m_selector, "Center");
    m_viewSelector.add(new Separator(), "South");
  }
  public Component setViewSelector() {
    return m_viewSelector;
  }
  public void addImageButton(Image image, 
                        String name, 
                        Component component) {
    ImageButton newButton = new ImageButton(image);
    m_selector.add(newButton,name);
    super.addView(name, component);
    newButton.addActionListener(this);
  }
  public void actionPerformed(ActionEvent e) {
    ImageButton ib = (ImageButton)e.getSource();
    showView(m_selector.getButtonPanel().getButtonName(ib));
  }
  public String getMode() {
    return m_selector.getMode();
  }
}
