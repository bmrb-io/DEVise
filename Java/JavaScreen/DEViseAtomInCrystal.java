// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Objects of this class hold information about atoms in 3D views
// (only information particular to a given *atom*; information related
// to all atoms of a given element is in DEViseAtomType.)

// There is one instance of this class for each atom in a 3D view.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.7  2000/05/24 14:07:09  wenger
// Cleaned up and commented 3D-related classes (DEViseCrystal, DEViseAtomType,
// DEViseAtomInCrystal, DEVise3DLCS).
//
// Revision 1.6  2000/04/05 06:25:36  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.5  2000/03/23 16:26:12  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.4  2000/02/04 15:51:44  hongyu
// *** empty log message ***
//
// Revision 1.3  1999/12/10 15:37:00  wenger
// Added standard headers to source files.
//
// ========================================================================

import java.awt.*;

public class DEViseAtomInCrystal
{
    // Note: type is null if atom is only here to define the end of a
    // bond.
    public DEViseAtomType type = null;

    public boolean isSelected;

    // The color this atom is currently being drawn with (may change
    // if the atom is selected).
    public Color color = null;

    // The "real" color of this atom, independent of whether it's selected.
    public Color realColor = null;

    // pos is position as specified in GData; lcspos is position after
    // translation, rotation, scaling.
    float[] pos = new float[3], lcspos = new float[3];

    public int bondNumber; // current number of bonds in bond array

    // From physics point of view, number of bond for an atom will not
    // exceed 8, and is rarely exceed 4;
    public int[] bond = new int[4];

    // Apparently the location (in transformed coordinates) and the size
    // (all in pixels) at which this atom should be drawn.
    public int drawX, drawY, drawSize;

    public DEViseAtomInCrystal(DEViseAtomType t, float x, float y, float z)
    {
        type = t;

        pos[0] = x;
        pos[1] = y;
        pos[2] = z;

        bondNumber = 0;

        isSelected = false;
    }

    // I think that index is the index in the DEViseCrystal's atomList
    // of the atom that this atom is bonded to.
    public void removeBond(int index)
    {
	// Remove the given bond (if we find it).
        int idx = -1;
        for (int i = 0; i < bondNumber; i++) {
            if (bond[i] == index) {
                idx = i;
                break;
            }
        }

	// Move the following bonds toward the front of the array
	// so that the bonds are contiguous in the array.
        if (idx != -1) {
            for (int i = idx; i < bondNumber - 1; i++) {
                bond[i] = bond[i + 1];
            }

            bondNumber--;
        }
    }

    // I think that index is the index in the DEViseCrystal's atomList
    // of the atom that this atom is bonded to.
    public void addBond(int index)
    {
	// Enlarge the bonds array if necessary.
        if (bondNumber == bond.length) {
            int[] tmp = new int[bondNumber * 2];
            boolean[] tmp1 = new boolean[bondNumber * 2];
            System.arraycopy(bond, 0, tmp, 0, bondNumber);
            bond = tmp;
        }

	// Enter the bond in the bonds array.
        bond[bondNumber] = index;
        bondNumber++;
    }
}

