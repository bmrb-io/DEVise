//----------------------------------------------------------------------
// 
// File Name: XManQueryPanel.java
//
// Description: Dynamically create a query Panel
//
// Author:      Hao Wang
//
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableColumn;
import javax.swing.ListSelectionModel;
import java.util.Vector;
import java.util.Observer;
import java.util.Observable;
import java.awt.Dimension;

import xman.shared.*;
import xman.importers.*;

public class XManAttributeTable
    extends JTable {

    public XManAttributeTable(XManImportBase data) {
	
	XManAttributeModel myModel = new XManAttributeModel(data);
	    
	setModel(myModel);
	setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	setPreferredScrollableViewportSize(new Dimension(300, 100));
	setShowHorizontalLines(true);
	setShowVerticalLines(true);
	sizeColumnsToFit(-1);
    }

    public XManAttribute getSelectedData() {
	int row = getSelectedRow();
	XManAttribute attribute = null;

	if (row != -1) {
	    attribute = ((XManAttributeModel) getModel()).getAttribute(row);
	}
	return attribute;
    }

    class XManAttributeModel extends AbstractTableModel 
	implements Observer {
	private Vector attributes_;
	private String[] cols = {"Attribute Name", 
				 "Default Value",
				 "Editable", 
				 "Displayable"};

	public XManAttributeModel(XManImportBase observed)
	{
	    // Observing the data
	    observed.addObserver(this);
	    initialize(observed);
	}

	public XManAttribute getAttribute(int row) {
	    XManAttribute atr = null;
	    if (row < attributes_.size()) {
		atr = (XManAttribute) attributes_.get(row);
	    }
	    return atr;
	}

        public int getColumnCount() {
            return cols.length;
        }
        
        public int getRowCount() {
	    return attributes_.size();
        }

        public String getColumnName(int col) {
            return cols[col];
        }

        public Object getValueAt(int row, int col) {
	    Object value;
	    switch (col) {
	    case 0 :
		value = ((XManAttribute)attributes_.get(row)).attributeName();
		break;
	    case 1 :
		value = ((XManAttribute)attributes_.get(row)).defaultValue();
		break;
	    case 2:
		value = ((XManAttribute)attributes_.get(row)).isEditable();
		break;
	    case 3:
		value = ((XManAttribute)attributes_.get(row)).isDisplayable();
		break;
	    default:
		value = new String();
		break;
	    }
	    return value;
        }

	public void initialize(XManImportBase data) {
	    attributes_ = data.attributes();
	}

	public void update(Observable observed, Object arg) {
	    System.out.println("New attribute!");
	    // For now, arg is going to be ignored
	    XManImportBase newData = (XManImportBase) observed;

	    if (getRowCount() < newData.attributes().size()) {
		initialize(newData);
		fireTableDataChanged();
		repaint();
	    }
	}
	
        public Class getColumnClass(int c) {
            return getValueAt(0, c).getClass();
        }

        public boolean isCellEditable(int row, int col) {
            //Note that the data/cell address is constant,
            //no matter where the cell appears onscreen.
	    return (col >= 1);
        }

        public void setValueAt(Object value, int row, int col) {
	    XManAttribute atr = (XManAttribute) attributes_.get(row);
	    boolean b;
	    switch(col) {
	    case 1:
		atr.setDefaultValue( (String) value);
		break;
	    case 2:
		b = ((Boolean) value).booleanValue();
		atr.setEditable(b);
		break;
	    case 3:
		b = ((Boolean) value).booleanValue();
		atr.setDisplayable(b);
		break;
	    default:
		break;
	    }
        }	
    }
    
    public Dimension getPreferredSize()
    {
	return new Dimension(300, 150);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(300, 150);
    }
}

