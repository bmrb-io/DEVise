/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class LayoutModeDlg.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>LayoutModeDlg</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class LayoutModeDlg extends CommonDialog
{
  DEViseViewPeer m_view; 
  TextField m_rows, m_cols;

  /* Constructor */
  public LayoutModeDlg(DEViseViewPeer view) {
    super("Change Layout mode for " + view.getName(), 
          new String[] { "Ok", "Cancel"});
    m_view = view;
    setSize(300,150);
    
    Panel mainPanel = new Panel();
    add(mainPanel, "Center");
    
    CheckboxGroup group = new CheckboxGroup();
    Checkbox gridB = new Checkbox("Grid", group, false);
    Checkbox pileB = new Checkbox("Pile", group, false);
    Checkbox customB = new Checkbox("Custom", group, false);
    Label rowL = new Label("Rows");
    m_rows = new TextField("-1");
    Label colL = new Label("Cols");
    m_cols = new TextField("-1");
    DEViseLayout initLayout = view.getDEViseLayout();
    if (initLayout instanceof DEViseGridLayout) {
      DEViseGridLayout gLayout = (DEViseGridLayout)initLayout;
      gridB.setState(true);
      m_rows.setText(Integer.toString(gLayout.getRows()));
      m_cols.setText(Integer.toString(gLayout.getCols()));
    } else if (initLayout instanceof DEVisePileLayout) {
      pileB.setState(true);
    } else if (initLayout instanceof DEViseCustomLayout) {
      customB.setState(true);
    }
    mainPanel.setLayout(new GridBagLayout());
    GridBagConstraints constraints = new GridBagConstraints();
    constraints.weightx = 0.1;
    constraints.weighty = 0.33;
    constraints.gridwidth = 1;
    constraints.gridx = 0;
    constraints.gridy = 0;
    mainPanel.add(gridB, constraints);
    constraints.gridx = GridBagConstraints.RELATIVE;
    mainPanel.add(rowL, constraints);
    mainPanel.add(m_rows, constraints);
    mainPanel.add(colL, constraints);
    mainPanel.add(m_cols, constraints);
    
    constraints.gridx = 0;
    constraints.gridy = 1;
    mainPanel.add(pileB, constraints);
    constraints.gridy = 2;
    mainPanel.add(customB, constraints);

    int choice = doModal();
    if (choice == 0) {
      if (gridB.getState()) {
        m_view.setDEViseLayout(new DEViseGridLayout(
                               Integer.parseInt(m_rows.getText()),
                               Integer.parseInt(m_cols.getText())));
      } else if (pileB.getState()) {
        m_view.setDEViseLayout(new DEVisePileLayout(initLayout));
      } else {
        m_view.setDEViseLayout(new DEViseCustomLayout());
      }
    } 
  } 
}
