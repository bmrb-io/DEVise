/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MappingDlg.java
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
 * The <CODE>MappingDlg</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class MappingDlg extends Dialog
{
  static String m_constChoices[][] =  {
    { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 
    { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 
    { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" }, 
    { },
    { "0", "1", "2", "3", "4", "5" }, 
    { },
    { },
    { "0", "90", "180", "270" } 
  };
  static {
    m_constChoices[3] = DEViseColor.COLORS;
    m_constChoices[5] = DEViseSymbol.SYMBOLS;
    m_constChoices[6] = DEVisePattern.PATTERNS;
  }
  private DEViseSchema m_schema;
  private DEViseMapping m_mapping;
  private Choice m_mapChoice[];
  private Choice m_mapType[];
  private TextField m_nameText;
  private int m_choice = 0;

  /* Constructor */
  public MappingDlg(DEViseMapping mapping) {
    this(mapping, mapping.getSchema()); 
  }
  public MappingDlg(DEViseSchema schema) {
    this(null, schema);
  }
  private MappingDlg(DEViseMapping mapping, DEViseSchema schema) {
    super(new Frame(), "Create/Edit Mapping");
    setSize(275,350);
    m_mapping = mapping;
    m_schema = schema;

    Panel buttonPanel = createButtonPanel();
    Panel mainPanel = createMainPanel();
    add(mainPanel, "Center");
    add(buttonPanel, "South");
  }
  public DEViseMapping getMapping() { return m_mapping; }
  private Panel createButtonPanel() {
    Panel buttonPanel = new Panel();
    Button bOk = new Button("OK");
    Button bCancel = new Button("Cancel");
    bOk.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        m_choice = 0;
        MappingDlg.this.dispose();
      }
    });
    bCancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        m_choice = 1;
        MappingDlg.this.dispose();
      }
    });
    buttonPanel.add(bOk);
    buttonPanel.add(bCancel);
    return buttonPanel;
  }
  private Panel createMainPanel() {
    Panel mainPanel = new Panel(new GridBagLayout());
    GridBagConstraints constraints = new GridBagConstraints();
    
    String str[] = new String[] { 
                   "X", "Y", "Z", "Color", "Size", "Symbol", "Pattern", 
                   "Orientation" };
    m_mapChoice = new Choice[str.length];
    m_mapType = new Choice[str.length];

    Label name = new Label("Name");
    constraints.gridx = 0;
    constraints.gridy = 0;
    constraints.weightx = 0.15;
    mainPanel.add(name, constraints);
    m_nameText = new TextField();
    m_nameText.setColumns(25);
    constraints.gridx = 1;
    constraints.weightx = 0.75;
    mainPanel.add(m_nameText, constraints);
    
    for (int i = 0; i < str.length; i++) {
      constraints.gridy = 1 + i;
      constraints.weighty = 0.15;
      Label label = new Label(str[i]);
      constraints.gridx = 0;
      constraints.weightx = 0.33;
      mainPanel.add(label, constraints);
      m_mapType[i] = createChoice(i);
      constraints.gridx = 1;
      constraints.weightx = 0.25;
      constraints.anchor = GridBagConstraints.CENTER;
      mainPanel.add(m_mapType[i], constraints);
      m_mapChoice[i] = new Choice();
      constraints.gridx = 2;
      constraints.weightx = 0.42;
      mainPanel.add(m_mapChoice[i], constraints);
    }
    if (m_mapping == null) {
      m_mapType[0].select("Attribute");
      updateAttributeChoice(0);  m_mapChoice[0].select(0);  

      m_mapType[1].select("Attribute");
      updateAttributeChoice(1);  m_mapChoice[1].select(1); 

      m_mapType[2].select("Attribute");
      updateAttributeChoice(2);  m_mapChoice[2].select(0);

      updateConstantChoice(3);  m_mapChoice[3].select(0);
      updateConstantChoice(4);  m_mapChoice[4].select(0);
      updateConstantChoice(5);  m_mapChoice[5].select(0);
      updateConstantChoice(6);  m_mapChoice[6].select(0);
      updateConstantChoice(7);  m_mapChoice[7].select(0);
    } else {
      initFromMapping();
    }
    return mainPanel;
  }
  private void initFromMapping() {
    m_nameText.setText(m_mapping.getName());

    int x = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_X);
    if (x == -1) {  
      x = (int)m_mapping.getDefaultX();
      m_mapChoice[0].select(String.valueOf(x));
    } else {
      updateAttributeChoice(0); 
      m_mapChoice[0].select(x);
    }
    int y = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_Y);
    if (y == -1) {  
      y = (int)m_mapping.getDefaultY();
      m_mapChoice[1].select(String.valueOf(y));
    } else {
      updateAttributeChoice(1); 
      m_mapChoice[1].select(y);
    }
    int z = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_Z);
    if (z == -1) {  
      z = (int)m_mapping.getDefaultZ();
      m_mapChoice[2].select(String.valueOf(z));
    } else {
      updateAttributeChoice(2); 
      m_mapChoice[2].select(z);
    }
    int clr = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_COLOR);
    if (clr == -1) {
      // Colors are not implemented properly....todo when color management
      // is done better in devise
      DEViseColor c = m_mapping.getDefaultColor();
      m_mapChoice[3].select(c.getColorName());
    } else {
      updateAttributeChoice(3);
      m_mapChoice[3].select(clr);
    }
    int size = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_SIZE);
    if (size == -1) {  
      size = (int)m_mapping.getDefaultSize();
      m_mapChoice[2].select(String.valueOf(size));
    } else {
      updateAttributeChoice(4); 
      m_mapChoice[4].select(size);
    }
    int symbol = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_SYMBOL);
    if (symbol == -1) {  
      DEViseSymbol sym = m_mapping.getDefaultSymbol();
      m_mapChoice[5].select(sym.getSymbolName());
    } else {
      updateAttributeChoice(5); 
      m_mapChoice[5].select(symbol);
    }
    int pattern = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_PATTERN);
    if (pattern == -1) {  
      DEVisePattern pat = m_mapping.getDefaultPattern();
      m_mapChoice[6].select(pat.getPatternName());
    } else {
      updateAttributeChoice(6); 
      m_mapChoice[6].select(pattern);
    }
    int orientation = m_mapping.getAttrMapping(DEViseMapping.GDATA_ATTR_ORIENT);
    if (orientation == -1) {  
      DEViseOrient orient = m_mapping.getDefaultOrient();
      m_mapChoice[7].select(String.valueOf((int)orient.getAngle()));
    } else {
      updateAttributeChoice(7); 
      m_mapChoice[7].select(orientation);
    }
  }
  private Choice createChoice(int index) {
    Choice choice = new Choice();
    choice.add("Constant");
    choice.add("Attribute");
    choice.addItemListener(new ChoiceItemListener(index));
    return choice;
  }   
  
  private class ChoiceItemListener  implements ItemListener{
    private int m_index;
    public ChoiceItemListener(int index) {
      m_index = index;
    }
    public void itemStateChanged(ItemEvent e) {
      Object obj = e.getSource();
      if (e.getStateChange() == ItemEvent.SELECTED) {
        String str = ((Choice)obj).getSelectedItem();
        updateChoice(m_index, str);
      } 
    }
  }

  private void updateChoice(int index, String str) {
    m_mapChoice[index].removeAll();
    if (str.equals("Constant")) {
      updateConstantChoice(index);
    } else {
      updateAttributeChoice(index);
    }
  }
  private void updateConstantChoice(int index) {
    String constants[] = m_constChoices[index];
    for (int i = 0; i < constants.length; i++) {
      m_mapChoice[index].add(constants[i]);
    }
  }

  private void updateAttributeChoice(int index) {
    String attrList[] = m_schema.getAttrList();
    for (int i = 0; i < attrList.length; i++) {
      m_mapChoice[index].add(attrList[i]);
    }
  }
  private void createMapping() {
    String name = m_nameText.getText();
    if (name.equals("")) {
      name = "Unknown"; 
    }
    if (m_mapping == null) {
      m_mapping = new DEViseMapping(name, m_schema);
    }
    if (m_mapType[0].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_X, 
                              m_mapChoice[0].getSelectedIndex());
    } else {
      m_mapping.setDefaultX(Double.valueOf(m_mapChoice[0].getSelectedItem()).doubleValue());
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_X, -1);
    }
    System.out.println("0");

    if (m_mapType[1].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_Y, 
                               m_mapChoice[1].getSelectedIndex());
    } else {
      m_mapping.setDefaultY(Double.valueOf(m_mapChoice[1].getSelectedItem()).doubleValue());
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_Y, -1);
    }
    System.out.println("1");
    if (m_mapType[2].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_Z, 
                               m_mapChoice[2].getSelectedIndex());
    } else {
      m_mapping.setDefaultZ(Double.valueOf(m_mapChoice[2].getSelectedItem()).doubleValue());
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_Z, -1);
    }
    System.out.println("2");
    if (m_mapType[3].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_COLOR,
                               m_mapChoice[3].getSelectedIndex());
    } else {
      m_mapping.setDefaultColor(DEViseColor.getColorByName(m_mapChoice[3].getSelectedItem()));
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_COLOR,  -1);
    }
    System.out.println("3");
    if (m_mapType[4].getSelectedItem().equals("Size")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_SIZE,
                               m_mapChoice[4].getSelectedIndex());
    } else {
      m_mapping.setDefaultSize(Double.valueOf(m_mapChoice[4].getSelectedItem()).doubleValue());
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_SIZE, -1);
    }
    System.out.println("4");
    if (m_mapType[5].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_SYMBOL,
                               m_mapChoice[5].getSelectedIndex());
    } else {
      m_mapping.setDefaultSymbol(DEViseMapping.getSymbolByName(m_mapChoice[5].getSelectedItem()));
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_SYMBOL, -1);
    }
    System.out.println("5");
    if (m_mapType[6].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_PATTERN,
                               m_mapChoice[6].getSelectedIndex());
    } else {
      m_mapping.setDefaultPattern(DEVisePattern.getPatternByName(m_mapChoice[6].getSelectedItem()));
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_PATTERN, -1);
    }
    System.out.println("6");
    if (m_mapType[7].getSelectedItem().equals("Attribute")) {
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_ORIENT,
                               m_mapChoice[7].getSelectedIndex());
    } else {
      m_mapping.setDefaultOrient(new DEViseOrient(Double.valueOf(m_mapChoice[7].getSelectedItem()).doubleValue()));
      m_mapping.setAttrMapping(DEViseMapping.GDATA_ATTR_ORIENT, -1);
    }
    System.out.println("7");
    System.out.println("Mapping created : " + m_mapping);
  }

  /* Public methods */
  public int doModal() {
    setModal(true);
    show();
    if (m_choice == 0) {
      createMapping();
    }
    return m_choice;
  }
}
