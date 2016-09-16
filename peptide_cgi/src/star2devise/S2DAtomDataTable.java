// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// An interface implemented by tables holding "additional data" to be
// added to coordinates.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.4.1  2005/03/22 20:34:37  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.1  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//

// ========================================================================

package star2devise;

public interface S2DAtomDataTable {

    // Get the suffix corresponding to this table's data.
    public String getSuffix();

    // Get the string with the attribute names corresponding to this
    // table's data.
    public String getSchemaStr();

    // Get the string containing this table's data corresponding to
    // the given atom.
    public String getCoordDataStr(int residueNum, String atomName);
}
