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

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
    public DEViseAtomType type = null;

    public boolean status;
    public int isSelected;
    public Color color = null;

    double[] pos = new double[3], lcspos = new double[3];

    public int bondNumber;
    // From physics point of view, number of bond for an atom will not exceed 8,
    // and is rarely exceed 4;
    public int[] bond = new int[4];
    public boolean[] selectedBond = new boolean[4];
    public int lastSelectedBondIndex = -1;

    public int drawX, drawY, drawSize;

    public DEViseAtomInCrystal(DEViseAtomType t, double x, double y, double z)
    {
        type = t;

        pos[0] = x;
        pos[1] = y;
        pos[2] = z;

        bondNumber = 0;

        status = true;
        isSelected = 0;
    }

    public void removeBond(int index)
    {
        int idx = -1;
        for (int i = 0; i < bondNumber; i++) {
            if (bond[i] == index) {
                idx = i;
                break;
            }
        }

        if (idx != -1) {
            for (int i = idx; i < bondNumber - 1; i++) {
                bond[i] = bond[i + 1];
                selectedBond[i] = selectedBond[i + 1];
            }

            bondNumber--;
        }
    }

    public void addBond(int index)
    {
        if (bondNumber == bond.length) {
            int[] tmp = new int[bondNumber * 2];
            boolean[] tmp1 = new boolean[bondNumber * 2];
            System.arraycopy(bond, 0, tmp, 0, bondNumber);
            System.arraycopy(selectedBond, 0, tmp1, 0, bondNumber);
            bond = tmp;
            selectedBond = tmp1;
        }

        bond[bondNumber] = index;
        selectedBond[bondNumber] = false;
        bondNumber++;
    }
}

