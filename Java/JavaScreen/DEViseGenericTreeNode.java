// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Nodes for generic tree GUI.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.1  2006/05/08 20:19:47  wenger
// Made the JavaScreen tree code itself generic (although the Jmol-
// related code that calls it is not).
//

// ========================================================================

// DEViseGenericTreeNode.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.util.*;

public class DEViseGenericTreeNode
{
    public String name;
    public Vector children; // Vector of DEViseGenericTreeNodes
    public Object treeInfo;

    public DEViseGenericTreeNode(String name)
    {
        this.name = name;
	children = new Vector();
    }

    public void addChild(DEViseGenericTreeNode child)
    {
        children.addElement(child);
    }

    public String toString()
    {
        return name;
    }
}

