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

// This class controls restrictions (constraints) applied to loop
// translation that are always stored inside save frames.

// ------------------------------------------------------------------------

// $Id$

// 2002/09/10 Patrícia Kayser - kayser@cos.ufrj.br
// Third version: small changes to allow multiple columns in
// save frame constraints and "-nl" and "-al" options on 
// matchConstraint and isSelectedLoop methods

// 2002/09/03  Patrícia Kayser - kayser@cos.ufrj.br
// Class include on second version of this program.

// ------------------------------------------------------------------------

package bmrb.star2ad;

import EDU.bmrb.starlibj.*;
import java.util.*;

public class Star2AdConstraintManager {
    //===================================================================
    // VARIABLES
    private Vector constraints;    
    private String currentSaveFrameName;
    private boolean TranslateLoops;
    
    //===================================================================
    // CONSTRUCTOR METHOD
    public Star2AdConstraintManager() {
        constraints = new Vector();
    }
    
    //===================================================================
    // PUBLIC METHODS
    
    //-------------------------------------------------------------------
    public void setConstraint(String saveFrameName, String constraint) {
        Star2AdConstraint c = new Star2AdConstraint();
        c.setSaveFrame(saveFrameName);
        // !!!!!
        // Here: I must treat complex constraints
        // at the moment only accept one single column name
        c.setConstraintExpression(constraint);
        
        constraints.addElement(c);
    }
    
    //-------------------------------------------------------------------
    public void setNL(){
        TranslateLoops = false;
    }

    //-------------------------------------------------------------------
    public void setAL(){
        TranslateLoops = true;
    }

    //-------------------------------------------------------------------
    public boolean isAllLoops(){
        return TranslateLoops;
    }

    //-------------------------------------------------------------------
    public boolean hasConstraints(String saveFrameName) {
        this.currentSaveFrameName = saveFrameName;
        
        if (TranslateLoops) {
            Enumeration e = constraints.elements();
        
            while (e.hasMoreElements()) {
                Star2AdConstraint c = (Star2AdConstraint) e.nextElement();
                if (saveFrameName.equals(c.getSaveFrame())) {
                    return true;
                }
            }        
            return false;
        } else {
            return true;
        }
    }
    
    //-------------------------------------------------------------------
    public boolean isSelectedLoop(DataLoopNameListNode dlNameList) {
        Enumeration eNameListNode   = dlNameList.elements();
        // I suppose none nested loop...
        LoopNameListNode nameList=(LoopNameListNode)eNameListNode.nextElement();
        // Enumeration eNameList = nameList.elements();	

        if (matchConstraint(nameList, currentSaveFrameName)) {
           return true;
        }

        if (TranslateLoops) {
            return true;
        } else {
            return false;
        }
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private boolean matchConstraint(LoopNameListNode rowName, String saveFrameName) {
        Enumeration e = constraints.elements();
        
        while (e.hasMoreElements()) {
            Star2AdConstraint c = (Star2AdConstraint) e.nextElement();
            if (c.matchConstraint(rowName, saveFrameName)) {
                return true;
            }
        }        
        return false;
    }
    
}
