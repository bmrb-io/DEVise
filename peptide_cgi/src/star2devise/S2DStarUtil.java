// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This is a class that contains some low-level utilities for dealing
// with Star files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.2  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.1  2003/04/22 21:58:16  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.10.1  2003/01/14 16:51:49  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.8.2  2002/10/31 22:17:35  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.1.8.1  2002/07/19 18:38:12  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.1.6.1  2002/06/20 20:59:47  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.1.4.1  2002/05/01 22:26:46  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.1.2.2  2002/04/04 15:40:41  wenger
// The basic grabbing of atomic coordinates from mmCIF files at PDB is
// now working; still needs cleanup, more testing, better error handling,
// etc.
//
// Revision 1.1.2.1  2002/04/01 19:56:48  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
//

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;

public class S2DStarUtil
{
    public static Class _frameClass = null;
    public static Class _loopClass = null;

    private static boolean _initialized = false;

    // ----------------------------------------------------------------------
    public static void initialize() throws S2DException
    {
        if (!_initialized) {
	    try {
	        _frameClass = Class.forName(StarValidity.pkgName() +
	          ".SaveFrameNode");
	        _loopClass = Class.forName(StarValidity.pkgName() +
	          ".DataLoopNode");
	        _initialized = true;
	    } catch (ClassNotFoundException ex) {
	        System.err.println("Exception getting class: " +
		  ex.toString());
	        throw new S2DError("Unable to get star file class");
	    }
	}
    }

    // ----------------------------------------------------------------------
    // Return the first parent, grandparent, etc. of the given node that
    // is of the given class.
    public static StarNode getParentByClass(StarNode node, Class desiredClass)
    {
	StarNode result = null;

	StarNode tmpNode = node;
        while (tmpNode != null) {
	    tmpNode = tmpNode.getParent();
	    if (tmpNode != null && tmpNode.getClass() == desiredClass) {
	        result = tmpNode;
		break;
	    }
	}

	return result;
    }

    // ----------------------------------------------------------------------
    // Finds the loop containing the given loopId tag in the given save
    // frame.
    //TEMPTEMP -- change name of 'frame'?
    public static DataLoopNode findLoop(StarNode frame, String loopId)
      throws S2DException
    {
        VectorCheckType loopList = frame.searchForTypeByName(_loopClass,
	  loopId);

        // There should be only one data loop.
        if (loopList.size() != 1) { 
           throw new S2DError("Found " + loopList.size() +
              " " + loopId + " tags, only expect 1." );
        }

        DataLoopNode loop = (DataLoopNode)loopList.firstElement();

	return loop;
    }

    // ----------------------------------------------------------------------
    // Gets the index for the named value in the given data loop.
    public static int getIndex(DataLoopNode loop, String name)
      throws S2DException
    {
        // Necessary for tagPositionDeep procedure.
        RemoteInt returnIndex = new RemoteInt();
        RemoteInt nestLevel = new RemoteInt();

	// Get the index.
	loop.getNames().tagPositionDeep(name, nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DError("Can't get index for " + name + " values");
	}

        return returnIndex.num;
    }

    // ----------------------------------------------------------------------
    // Convert a StarNode to a string, including line and column numbers.
    public static String node2String(StarNode node)
    {
        ByteArrayOutputStream os = new ByteArrayOutputStream();

	StarUnparser up = new StarUnparser(os);
	up.writeOut(node, 0);

	String result = "star file at line " + node.getLineNum() +
	  ", column " + node.getColNum() + ": " + os.toString();

	return result;
    }
}

// ========================================================================
