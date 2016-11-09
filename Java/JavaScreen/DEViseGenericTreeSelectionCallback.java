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

// Callback interface for generic tree GUI -- nodesSelected() is called
// when the user changes the selection in the tree.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseGenericTreeSelectionCallback.java

package JavaScreen;

import java.util.*;

public interface DEViseGenericTreeSelectionCallback
{
    public void nodesSelected(String name, Vector nodes);
}

