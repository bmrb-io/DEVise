// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002
// By the DEVise Development Group 
// Madison, Wisconsin
// All Rights Reserved. 
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise 
// Development Group.

// ------------------------------------------------------------------------

// This class represents a node in constraints array
// It stores informations about restrictions (constraints)
// applied to translate loops for a specific save frame

// ------------------------------------------------------------------------

// $Id$

// 2002/09/10 Patrícia Kayser - kayser@cos.ufrj.br
// Third version: small changes to allow multiple columns in
// save frame constraints and "-nl" and "-al" options on
// matchConstraint method. Attribute loopName changed from
// String to Vector.

// 2002/09/03  Patrícia Kayser - kayser@cos.ufrj.br
// Class include on second version of this program

// ------------------------------------------------------------------------

package bmrb.star2ad;

import EDU.bmrb.starlibj.*;
import java.util.*;

public class Star2AdConstraint {
    //===================================================================
    // VARIABLES
    private String saveFrameName;
    private Vector loopName;
    
    public Star2AdConstraint() {
        loopName = new Vector();
    }
    
    public void setSaveFrame(String saveFrameName) {
        this.saveFrameName = saveFrameName;
    }
    
    //===================================================================
    // PUBLIC METHODS
    
    //-------------------------------------------------------------------
    public String getSaveFrame() {
        return this.saveFrameName;
    }
    
    //-------------------------------------------------------------------
    public void setConstraintExpression(String expression) {
        StringTokenizer st = new StringTokenizer(expression,",");
        while (st.hasMoreTokens()) {
                loopName.addElement(st.nextToken());
        }
    }
    
    //-------------------------------------------------------------------
    /*
     public boolean matchConstraint(String rowName, String saveFrameName) {
        Enumeration e = loopName.elements();
        
        String columnName = (String) e.nextElement();
            
        if ((columnName.equals(rowName)) &&
            (this.saveFrameName.equals(saveFrameName))) {
            return true;
        } else {
            return false;
        }
    }
    */

    public boolean matchConstraint(LoopNameListNode nameList, String saveFrameName) {
        if (!(this.saveFrameName.equals(saveFrameName))) {
            return false;
        }
        
        //Enumeration eRowName = nameList.elements();
        
        Enumeration eLoop = loopName.elements();
        String columnName; 
        while (eLoop.hasMoreElements()) {
            columnName = (String) eLoop.nextElement();
            boolean matched = false;
            Enumeration e = nameList.elements();
            while (e.hasMoreElements()) {
                DataNameNode rowName = (DataNameNode) e.nextElement();                
                if (columnName.equals(rowName.getLabel())) {
                    matched = true;
                }
           }
           if (!matched) {
               return false;
           }
        }
        
        return true;        
    }
}
