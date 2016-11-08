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
// Revision 1.4.40.1  2014/06/09 21:04:00  wenger
// Peak list processing now reports a fatal error if there is no
// _Spectral_dim information.
//
// Revision 1.4  2010/04/23 16:51:08  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.3.20.2  2010/04/22 16:38:11  wenger
// Various minor cleanups of distance restraint code.
//
// Revision 1.3.20.1  2010/02/24 22:37:03  wenger
// Implemented basic distance restraint processing of remediated
// restraints files, added a test using remediated restraints via
// the BMRB web site.
//
// Revision 1.3  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
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
// ...

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
    // Determines whether a loop or loops containing the given loopId
    // tag exist in the given save frame.
    //TEMP -- change name of 'frame'?
    public static boolean loopExists(StarNode frame, String loopId)
    throws S2DException
    {
        VectorCheckType loopList = frame.searchForTypeByName(_loopClass,
                                   loopId);

        return (loopList.size() > 0);
    }

    // ----------------------------------------------------------------------
    // Finds the loop containing the given loopId tag in the given save
    // frame.
    //TEMP -- change name of 'frame'?
    public static DataLoopNode findLoop(StarNode frame, String loopId)
    throws S2DException
    {
        VectorCheckType loopList = frame.searchForTypeByName(_loopClass,
                                   loopId);

        // There should be only one data loop.
        if (loopList.size() != 1) {
            throw new S2DError("Found " + loopList.size() +
                               " " + loopId + " tags, expect exactly 1." );
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
