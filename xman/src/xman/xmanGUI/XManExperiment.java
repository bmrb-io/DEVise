//----------------------------------------------------------------------
// 
// File Name:    XManExperiment.java
// 
// Description:  Experiment Management Experiment
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import java.util.Date;

public class XManExperiment
{
    private String  experimentName_;
    private Date    lastModifiedDate_;
    private Integer numberOfRecords_;
    private Boolean isSelected_;

    public XManExperiment(String experimentName, 
			  long   lastModifiedDate,
			  int    numberOfRecords,
			  boolean selected)
    {
	experimentName_   = experimentName;
	lastModifiedDate_ = new Date(lastModifiedDate);
	numberOfRecords_  = new Integer(numberOfRecords);
	isSelected_       = new Boolean(selected);
    }

    public String getExperimentName()
    {
	return experimentName_;
    }

    public Date getLastModifiedDate()
    {
	return lastModifiedDate_;
    }

    public Integer getNumberOfRecords()
    {
	return numberOfRecords_;
    }

    public Boolean isSelected()
    {
	return isSelected_;
    }
    
    public void setSelection(Boolean selected)
    {
	isSelected_ = selected;
    }
}







