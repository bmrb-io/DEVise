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
import java.util.Vector;
import java.util.Observer;
import java.util.Observable;
import java.awt.Dimension;

import xman.shared.*;
import xman.importers.*;

public class XManQueryPanel
    extends JTable {

    public XManQueryPanel(XManImportBase data) {
	
	XManQueryModel myModel = new XManQueryModel(data);
	    
	setModel(myModel);
	
	setPreferredScrollableViewportSize(new Dimension(300, 100));
	setShowHorizontalLines(true);
	setShowVerticalLines(true);
	sizeColumnsToFit(-1);
    }

    public XManExperimentInfo queryData() {
	return ((XManQueryModel) getModel()).getQueryData();
    }

    class XManQueryModel extends AbstractTableModel 
	implements Observer {
	private Vector attributes_;
	private Vector values_;
	private XManExperimentInfo info_;
	private String[] cols = {"Attribute", "Value"};
	
	public XManQueryModel(XManImportBase observed)
	{
	    // Observing the data
	    observed.addObserver(this);
	    info_ = new XManExperimentInfo(observed);
	    initialize(observed);
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
	    if (col == 0) {
		value = attributes_.get(row);
	    }
	    else {
		value = values_.get(row);
	    }
	    
	    return value;
        }

	public XManExperimentInfo getQueryData() {
	    for (int i=0; i<attributes_.size(); i++) {
		String name = (String) attributes_.get(i);
		info_.setData(name, (String) values_.get(i));
	    }
	    return info_;
	}

	public void initialize(XManImportBase data) {
	    Vector visualIndex = data.visualIndex();
	    Vector attribute   = data.attributes();
	    int size = visualIndex.size();

	    attributes_ = new Vector(size);
	    values_     = new Vector(size);
	    
	    for (int i=0; i < size; i++) {
		int index = ((Integer)visualIndex.get(i)).intValue();
		attributes_.add(i, ((XManAttribute)attribute.get(index)).attributeName());
		values_.add(i, new String());
	    }
 
	}
	public void update(Observable observed, Object arg) {

	    // For now, arg is going to be ignored
	    XManImportBase newData = (XManImportBase) observed;

	    if (getRowCount() < newData.visualIndex().size()) {
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
	    return (col == 1);
        }

        public void setValueAt(Object value, int row, int col) {
	    if (col == 1) {
		values_.setElementAt(value, row);
	    }
        }	
    }
    
    public Dimension getPreferredSize()
    {
	return new Dimension(200, 150);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(200, 150);
    }
}

