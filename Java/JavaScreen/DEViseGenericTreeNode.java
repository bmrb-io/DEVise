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

// ========================================================================

// DEViseGenericTreeNode.java

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

