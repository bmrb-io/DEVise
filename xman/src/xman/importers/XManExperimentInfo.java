//----------------------------------------------------------------------
//
// File Name: XManExperimentInfo.java
//
// Purpose:   Object used to describe experiment information
//
// D.O.B.:    3/10/2000
//
//----------------------------------------------------------------------
package xman.importers;

import java.util.Vector;

public class XManExperimentInfo {

    private Boolean        isSelected_;

    private XManImportBase importer_;
    private Vector         experiment_;

    public XManExperimentInfo(XManImportBase importer){
	importer_   = importer;
	isSelected_ = new Boolean(false);

	experiment_ = new Vector(importer_.numberOfAttributes());
	for (int i=0; i < importer_.numberOfAttributes(); i++) {
	    experiment_.add(i, new String(importer_.defaultValue(i)));   
	}
    }

    public String getData(String attribute) {
	return (String) experiment_.get(importer_.indexOf(attribute));
    }

    public String getData(int index) {
	return (String) experiment_.get(index);
    }

    public void setData(int index, String data) {
	try {
	    experiment_.set(index, data);
	}
	catch (java.lang.ArrayIndexOutOfBoundsException exc) {
	    experiment_.add(index, data);
	}
    }
    public void setData(String attribute, String data) {
	int index = importer_.indexOf(attribute);
	setData(index, data);
    }
}










