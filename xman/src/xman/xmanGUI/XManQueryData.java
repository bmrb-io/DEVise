//----------------------------------------------------------------------
// 
// File Name:   XManQueryData.java
//
// Description: Query Data
//
// Author:      Hao Wang
//----------------------------------------------------------------------
package xman.xmanGUI;

import java.util.Vector;

public class XManQueryData 
    extends Object {
    private Vector attributes_;
    private Vector values_;
    public XManQueryData(Vector attributes,
			 Vector values) {
	attributes_ = attributes;
	values_     = values;
    }

    public Vector attributes() {
	return attributes_;
    }

    public Vector values() {
	return values_;
    }
}
