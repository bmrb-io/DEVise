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

public class XManAddTable
    extends JTable {

    public XManAddTable(XManImportBase data) {
	
	XManAddModel myModel = new XManAddModel(data);
	    
	setModel(myModel);
	setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	setPreferredScrollableViewportSize(new Dimension(300, 100));
	setShowHorizontalLines(true);
	setShowVerticalLines(true);
	sizeColumnsToFit(-1);

	setMinimumSize(getMinimumSize());
    }

    public XManExperimentInfo getData() {
	return ((XManAddModel) getModel()).getData();
    }

    class XManAddModel extends AbstractTableModel 
	implements Observer {
	private Vector             attributes_;
	private Vector             visualIndex_;
	private XManExperimentInfo info_;
	private String[] cols = {"Attribute Name", 
				 "Value"};

	public XManAddModel(XManImportBase observed)
	{
	    // Observing the data
	    observed.addObserver(this);
	    initialize(observed);
	}

	public XManExperimentInfo getData() {
	    return info_;
	}

        public int getColumnCount() {
            return cols.length;
        }
        
        public int getRowCount() {
	    return visualIndex_.size();
        }

        public String getColumnName(int col) {
            return cols[col];
        }

        public Object getValueAt(int row, int col) {
	    Object value;
	    switch (col) {
	    case 0 :
		value = ((XManAttribute) attributes_.get
			 (((Integer)visualIndex_.get(row)).intValue())).attributeName();
		break;
	    case 1 :
		value = info_.getData((String)getValueAt(row, 0));
		break;
	    default:
		value = new String();
		break;
	    }
	    return value;
        }

	public void initialize(XManImportBase data) {
	    attributes_ = new Vector(data.attributes());
	    visualIndex_= new Vector(data.visualIndex());
	    info_       = new XManExperimentInfo(data);
	}

	public void update(Observable observed, Object arg) {
	    System.out.println("Add Table received notification!");
	    // For now, arg is going to be ignored
	    XManImportBase newData = (XManImportBase) observed;

	    initialize(newData);
	    fireTableDataChanged();
	    repaint();
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
	    boolean b;
	    switch(col) {
	    case 1:
		info_.setData((String)getValueAt(row, 0), (String) value);
		break;
	    default:
		break;
	    }
        }	
    }
    
    public Dimension getPreferredSize()
    {
	return new Dimension(350, 200);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(350, 200);
    }
}

