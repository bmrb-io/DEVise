// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
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

// ========================================================================

package JavaScreen;

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


