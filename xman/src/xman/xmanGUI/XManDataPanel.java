//----------------------------------------------------------------------
// 
// File Name:    XManDataPanel.java
// 
// Description:  Experiment Management Data Panel
//
// Author:       Hao Wang
// 
// D.O.B.        02/28/2000
//
// $Id$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableColumn;
import javax.swing.ListSelectionModel;
import javax.swing.JScrollPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.JOptionPane;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import java.awt.Dimension;
import java.util.Observer;
import java.util.Observable;

import xman.shared.*;
import xman.importers.*;

public class XManDataPanel 
    extends JTable {
    private boolean DEBUG = true;

    public XManDataPanel(XManImportBase data) {
	
	XManTableModel myModel = new XManTableModel(data);
	    
	setModel(myModel);
	//setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	setPreferredScrollableViewportSize(new Dimension(400, 200));
	setShowHorizontalLines(false);
	setShowVerticalLines(false);
	sizeColumnsToFit(-1);
    }

    public void ChangeData(Vector data) {
	((XManTableModel) getModel()).changeData(data);
    }

    public String[] getSelectedData(int index) {
	int row[] = getSelectedRows();
	String dirName[] = null;
	if (row.length > 0) {
	    dirName = new String[row.length];
	    for (int i=0; i < row.length; i++) {
		dirName[i] = ((XManTableModel) getModel()).getValue(i, index);
	    }
	}
	return dirName;
    }

    class XManTableModel extends AbstractTableModel 
        implements Observer {

	private Vector  attributes_;
	private Vector  visualIndex_;
	private Vector  data_;

	public XManTableModel(XManImportBase observed)
	{
	    // Observing the data
	    observed.addObserver(this);

	    attributes_ = new Vector(observed.attributes());
	    visualIndex_= new Vector(observed.visualIndex());
	    data_       = new Vector(observed.data());
	}

        public int getColumnCount() {
            return visualIndex_.size();
        }
        
        public int getRowCount() {
	    return data_.size();
        }

	public String getValue(int row, int col) {
	    return (String) 
		((XManExperimentInfo) data_.get(row)).getData(col);
	}

        public String getColumnName(int col) {
            return ((XManAttribute) attributes_.get( ((Integer)visualIndex_.get(col)).intValue())).attributeName();
        }

        public Object getValueAt(int row, int col) {
	    Object value;
	    value = ((XManExperimentInfo) data_.get(row)).getData(((Integer)visualIndex_.get(col)).intValue());
	    return value;
        }

	public void update(Observable observed, Object arg) {

	    // For now, arg is going to be ignored
	    XManImportBase newData = (XManImportBase) observed;

	    System.out.println(attributes_.size());
	    System.out.println(getColumnCount());

	    if (getColumnCount() < newData.visualIndex().size()) {
		attributes_  = new Vector(newData.attributes());
		visualIndex_ = new Vector(newData.visualIndex());
		System.out.println("Columns inserted!");
		fireTableStructureChanged();
		repaint();
	    }
	    
	    if (getRowCount() < newData.data().size()) {
		data_ = new Vector(newData.data());
		System.out.println("Rows inserted!");
		fireTableRowsInserted(getRowCount(), data_.size());
		repaint();
	    }
	}

	public void changeData(Vector data) {
	    data_ = new Vector(data);
	    fireTableDataChanged();
	}

        /*
         * JTable uses this method to determine the default renderer/
         * editor for each cell.  If we didn't implement this method,
         * then the last column would contain text ("true"/"false"),
         * rather than a check box.
         */
        public Class getColumnClass(int c) {
            return getValueAt(0, c).getClass();
        }

        /*
         * Don't need to implement this method unless your table's
         * editable.
         */
        public boolean isCellEditable(int row, int col) {
            //Note that the data/cell address is constant,
            //no matter where the cell appears onscreen.
	    return ((XManAttribute) attributes_.get(((Integer) visualIndex_.get(col)).intValue())).isEditable().booleanValue();
        }

        /*
         * Don't need to implement this method unless your table's
         * data can change.
         */
        public void setValueAt(Object value, int row, int col) {
            if (DEBUG) {
                System.out.println("Setting value at " + row + "," + col
                                   + " to " + value
                                   + " (an instance of " 
                                   + value.getClass() + ")");
            }

	    ((XManExperimentInfo) data_.get(row)).setData(((Integer)visualIndex_.get(col)).intValue(), (String) value);
	    /*
	    if (col == 3)
		{
		    fireTableCellUpdated(row, col);
		}
	    */
            if (DEBUG) {
                System.out.println("New value of data:");
                printDebugData();
            }
        }

        private void printDebugData() {
            int numRows = getRowCount();
            int numCols = getColumnCount();

            for (int i=0; i < numRows; i++) {
                System.out.print("    row " + i + ":");
                for (int j=0; j < numCols; j++) {
                    //System.out.print("  " + data[i][j]);
                }
                System.out.println();
            }
            System.out.println("--------------------------");
        }
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(350, 400);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(350, 400);
    }
}
