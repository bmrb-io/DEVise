//----------------------------------------------------------------------
//
// File Name: XManAttrribute.java
//
// Purpose:   Object used to describe experiment attribute
//
// D.O.B.:    3/10/2000
//
//----------------------------------------------------------------------

package xman.importers;

public class XManAttribute 
    extends Object {

    // Some private attribute data
    private String  attributeName_;
    private Boolean editable_;
    private Boolean displayable_;
    private Boolean canBeDeleted_;
    private String  default_;
    
    public XManAttribute(String attributeName, String defVal) {
	attributeName_ = new String(attributeName);
	editable_      = new Boolean(true);
	displayable_   = new Boolean(true);
	canBeDeleted_  = new Boolean(true);
	setDefaultValue(defVal);
    }

    public String defaultValue() {
	return default_;
    }

    public String attributeName() {
	return attributeName_;
    }

    public Boolean isEditable() {
	return editable_;
    }

    public Boolean isDisplayable() {
	return displayable_;
    }

    public Boolean isDeletable() {
	return canBeDeleted_;
    } 

    public void setDefaultValue(String defValue) {
	if (defValue.length() > 0) {
	    default_ = defValue;
	}
	else {
	    default_ = new String();
	}
    }

    public void setAttributeName(String attribute) {
	attributeName_ = attribute;
    }
    
    public void setEditable(boolean editable) {
	editable_ = new Boolean(editable);
    }

    public void setDisplayable(boolean displayable) {
	displayable_ = new Boolean(displayable);
    }


    public XManAttribute(String  attributeName,
			 String  defVal,
			 boolean isEditable,
			 boolean isDisplayable,
			 boolean canBeDeleted) {

	attributeName_ = new String(attributeName);
	editable_      = new Boolean(isEditable);
	displayable_   = new Boolean(isDisplayable);
	canBeDeleted_  = new Boolean(canBeDeleted);
	setDefaultValue(defVal);
    }
}

