/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class NiceToolbar.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import gjt.*;

// Class
/**
 * The <CODE>NiceToolbar</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class NiceToolbar extends Panel
{
  private static int defaultButtonWidth = 20;
  private static int defaultButtonHeight = 20;
  private static int defaultThickness = 2;  

  private NiceToolbarPanel m_buttonPanel;
  private Orientation m_orientation;
  private String m_name;
  private int m_butWidth;
  private int m_butHeight;
  private int m_thickness;
  
  public NiceToolbar(String name, Orientation orientation) {
    this(name, orientation, defaultButtonHeight, defaultButtonHeight, defaultThickness);
  }

  public NiceToolbar(String name, Orientation orientation, 
                     int buttonWidth, int buttonHeight,
                     int thickness) {
    m_name = name;
    m_butHeight = buttonHeight;
    m_butWidth = buttonWidth;
    m_thickness = thickness;
    m_orientation = orientation;

    m_buttonPanel = new NiceToolbarPanel(m_orientation);
    setLayout(new BorderLayout());
    if (m_orientation == Orientation.VERTICAL) {
      add(m_buttonPanel, "West");
    } else {
      add(m_buttonPanel, "North");
    }
  }
  public void add(ImageButton button, String name) {
    m_buttonPanel.add(button, name);
  }
  public void addSpacer(int size) {
    m_buttonPanel.addSpacer(size);
  }
  public ImageButton getButtonByName(String name) {
    return m_buttonPanel.getButtonByName(name);
  }
  public String getName() { return m_name; }
  public int getButtonWidth() { return m_butWidth; }
  public int getButtonHeight() { return m_butHeight; }
  public int getThickness() { return m_thickness; }
  public boolean isVertical() { return (m_orientation == Orientation.VERTICAL); }
  public ImageButtonPanel getButtonPanel() {
    return m_buttonPanel;
  }

}

class NiceToolbarPanel extends ImageButtonPanel {
 
  public NiceToolbarPanel(Orientation orient) {
    super(orient);
  }
  public Insets getInsets() {
    return new Insets(5,5,5,5);
  }
}


