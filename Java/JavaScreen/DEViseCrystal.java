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

// This class handles the drawing of 3D views.

// Note that if there is a pile of 3D views, there is only a single
// DEViseCrystal object for the entire pile.

// Note: this class should eventually be changed so that bonds are first-
// class objects, as opposed to being represented as part of the atoms.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.19  2000/06/21 18:37:29  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.18  2000/06/21 18:10:14  wenger
// Changes to 3D requested by BMRB: removed axes; up/down mouse movement
// does zooming; molecule doesn't move when changing atoms; 'r' resets
// location and zoom as well as rotation.
//
// Revision 1.17  2000/05/25 14:47:38  wenger
// 3D coordinate system remains unchanged when new GData arrives; 'r' or 'R'
// in view resets to default coordinates.
//
// Revision 1.16  2000/05/24 14:07:09  wenger
// Cleaned up and commented 3D-related classes (DEViseCrystal, DEViseAtomType,
// DEViseAtomInCrystal, DEVise3DLCS).
//
// Revision 1.15  2000/05/22 17:52:48  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.14  2000/05/12 20:43:56  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
// Revision 1.13  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.12  2000/04/03 22:24:53  wenger
// Added named constants for GData symbol types; 3D GData symbols are now
// differentiated by symbol type instead of string; removed some commented-
// out code.
//
// Revision 1.11  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.10  2000/02/04 15:51:53  hongyu
// *** empty log message ***
//
// Revision 1.9  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.8  1999/12/10 15:37:01  wenger
// Added standard headers to source files.
//
// ========================================================================


import java.awt.*;
import java.util.*;

public class DEViseCrystal
{
    boolean isTransformed = true, isReady = false;
    DEVise3DLCS lcs = null, oldlcs = null;

    Dimension viewArea = new Dimension(0, 0);
    int viewShift = 0;
    float boxLength, viewDistance, halfViewDistance, boxViewRatio = 1.0f,
      pixelToDataUnit = 0.1f;
    float newBoxLength, newTotalX, newTotalY, newTotalZ;
    private float maxiX = -1000000.0f, miniX = 10000000.0f,
      maxiY = -10000000.0f, miniY = 10000000.0f,
      maxiZ = -10000000.0f, miniZ = 1000000.0f;

    float totalX, totalY, totalZ, totalScaleFactor = 1.0f, totalXRotation,
      totalYRotation;
    int shiftedX, shiftedY, totalShiftedX, totalShiftedY, totalAtomNumber;

    // float minBondLength, maxBondLength;
    public int bondWidth = 2;

    public Color axisColor = Color.red;

    Vector atomTypeList = new Vector();
    Vector atomList = new Vector();
    Stack deletedAtomIndexStack = new Stack();

    // zSortMap is an array of atom indices, sorted in decreasing
    // *transformed* Z order.
    private static final int zSortMapSizeIncrement = 64;
    private int zSortMapSize = 0;
    private int[] zSortMap = new int[zSortMapSizeIncrement];

    // Note: _hasAtoms is false if we have atoms, but the atoms are used
    // only to define bonds and are not to be drawn.
    private boolean _hasAtoms = false;
    private boolean _hasBonds = false;

    // --------------------------------------------------------------------
    // min and max are minimum and maximum bond length for automatically
    // calculating bonds (no longer used).
    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS,
      float min, float max)
    {
        YGlobals.yassert(LCS != null, "Null argument",
	  "DEViseCrystal::constructor");
        YGlobals.yassert(width > 0 && height > 0, "Invalid size argument",
	  "DEViseCrystal::constructor");

        viewArea.width = width;
        viewArea.height = height;
        viewShift = shift;
        // minBondLength = min;
        // maxBondLength = max;
        oldlcs = LCS;

        for (int i = 0; i < zSortMap.length; i++) {
            zSortMap[i] = -1;
        }

        //newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    // --------------------------------------------------------------------
    // min and max are minimum and maximum bond length for automatically
    // calculating bonds (no longer used).
    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS,
      float min, float max, String[] atomName, float[][] atomPos)
    {
	this(width, height, shift, LCS, min, max);

        if (atomName != null && atomPos != null &&
	  atomName.length >= atomPos.length) {
	    _hasAtoms = true;
            int i;

            for (i = 0; i < atomName.length; i++) {
                addAtom(atomName[i], atomPos[i][0], atomPos[i][1],
		  atomPos[i][2]);
            }
	    center();
        }

	// resort();

        //newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    // --------------------------------------------------------------------
    // min and max are minimum and maximum bond length for automatically
    // calculating bonds (no longer used).
    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS,
      float min, float max, String[] atomName, float[][] atomPos,
      Color[] atomColor, float[][][] bondPos, Color[] bondColor)
    {
	this(width, height, shift, LCS, min, max);

        if (atomName != null && atomPos != null && atomColor != null &&
	  atomName.length >= atomPos.length &&
	  atomColor.length >= atomPos.length) {
	    _hasAtoms = true;
            int i;

            for (i = 0; i < atomName.length - 1; i++) {
                addAtom(atomName[i], atomPos[i][0], atomPos[i][1],
		  atomPos[i][2], atomColor[i]);
            }

            addAtom(atomName[i], atomPos[i][0], atomPos[i][1],
	      atomPos[i][2], atomColor[i], 2);
        }

        if (bondPos != null && bondColor != null &&
	  bondColor.length >= bondPos.length) {
	    _hasBonds = true;

            int i, index1 = 0, index2 = 0;
            for (i = 0; i < bondPos.length; i++) {
                index1 = addAtom(null, bondPos[i][0][0],
		  bondPos[i][0][1], bondPos[i][0][2], bondColor[i]);
                index2 = addAtom(null, bondPos[i][1][0],
		  bondPos[i][1][1], bondPos[i][1][2], bondColor[i]);
                DEViseAtomInCrystal atom1 = getAtom(index1),
		  atom2 = getAtom(index2);
                atom1.addBond(index2);
                atom2.addBond(index1);
            }
	    center();
            DEViseAtomInCrystal atom1 = getAtom(index1),
	      atom2 = getAtom(index2);
            atom1.addBond(index2);
            atom2.addBond(index1);
        }

	// resort();

        //newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    // --------------------------------------------------------------------
    public synchronized void paint(Component component, Graphics gc,
      boolean isMove)
    {
        if (component == null || gc == null || !isReady) {
            return;
        }

        if (isTransformed) {
	    // Coordinates have been transformed since the last time we
	    // calculated transformed atom positions, so we need to
	    // calculate them again.
            for (int i = 0; i < atomList.size(); i++) {
                DEViseAtomInCrystal atom =
		  (DEViseAtomInCrystal)atomList.elementAt(i);

		// Calculate atom's position after translation/rotation/
		// scaling.
                lcs.point(atom.pos, atom.lcspos);
            }

	    // Sort by Z value.
            if (_hasAtoms) resort();

            isTransformed = false;
        }

        int x, y, x1, y1, xm, ym, index, index1;
        float z;
        float[] pos = new float[3];

        byte[] atomDrawn = new byte[atomList.size()];

        DEViseAtomInCrystal atom = null;

        Color oldcolor = gc.getColor(), color = null, color1 = null;

        for (int i = 0; i < zSortMapSize; i++) {
            index = zSortMap[i];
            if (index < 0) { // ignore deleted atom
                continue;
            }

            atom = (DEViseAtomInCrystal)atomList.elementAt(index);
            x = (int)(atom.lcspos[0] + 0.5f) + shiftedX;
            y = (int)(atom.lcspos[1] + 0.5f) + shiftedY;

            atomDrawn[index] = 1;

            if (atom.color != null) {
	        color = atom.color;
	    } else if (atom.realColor != null) {
	        color = atom.realColor;
	    } else if (atom.type != null) {
	        color = atom.type.color;
	    } else {
		// Error!
	        color = Color.white;
	    }

            // Draw the bonds first.
            for (int j = 0; j < atom.bondNumber; j++) {
                index1 = atom.bond[j];
                if (atomDrawn[index1] == 1) { // bond already drawed
                    continue;
                }

                DEViseAtomInCrystal atom1 =
		  (DEViseAtomInCrystal)atomList.elementAt(index1);
                x1 = (int)(atom1.lcspos[0] + 0.5f) + shiftedX;
                y1 = (int)(atom1.lcspos[1] + 0.5f) + shiftedY;

                gc.setColor(color);
                gc.drawLine(x, y, x1, y1);
            }

            z = (halfViewDistance - atom.lcspos[2]) / viewDistance *
	      totalScaleFactor;

	    // Now draw the atom.  (If type == null, this atom is only
	    // here to define the end of a bond, and should not be drawn.)
            if (_hasAtoms && atom.type != null) {
                if (!atom.isSelected) {
                    if (isMove) {
                        atom.type.paint(component, gc, x, y, z,
		          DEViseAtomType.DRAW_PLAIN_UNSELECTED);
                    } else {
                        atom.type.paint(component, gc, x, y, z,
		          DEViseAtomType.DRAW_SHADED_UNSELECTED);
                    }
                } else {
                    if (isMove) {
                        atom.type.paint(component, gc, x, y, z,
		          DEViseAtomType.DRAW_PLAIN_SELECTED);
                    } else {
                        atom.type.paint(component, gc, x, y, z,
		          DEViseAtomType.DRAW_SHADED_SELECTED);
                    }
                }
                atom.drawX = x;
                atom.drawY = y;
                atom.drawSize = atom.type.drawSize;
            }
        }

/* BMRB people asked to get rid of the axes.  RKW 2000-06-21.
	// Draw the axes.
        gc.setColor(axisColor);
        Font oldfont = gc.getFont(), font = DEViseFonts.getFont(12,
	  DEViseFonts.SERIF, 0, 0);
        gc.setFont(font);

        z = 20 / pixelToDataUnit;
        pos = lcs.point(z, 0, 0, false);
        x = (int)(pos[0] + 0.5f) + shiftedX;
        y = (int)(pos[1] + 0.5f) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("a", x, y);
        pos = lcs.point(0, z, 0, false);
        x = (int)(pos[0] + 0.5f) + shiftedX;
        y = (int)(pos[1] + 0.5f) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("b", x, y);
        pos = lcs.point(0, 0, z, false);
        x = (int)(pos[0] + 0.5f) + shiftedX;
        y = (int)(pos[1] + 0.5f) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("c", x, y);

        gc.setColor(oldcolor);
        gc.setFont(oldfont);
*/
    }

    // --------------------------------------------------------------------
    public float[] getPos(float[] pos)
    {
        return oldlcs.point(pos);
    }

    // --------------------------------------------------------------------
    public int addAtom(String name, float x, float y, float z)
    {
        return addAtom(name, x, y, z, null, 0);
    }

    // --------------------------------------------------------------------
    public int addAtom(String name, float x, float y, float z, Color color)
    {
        return addAtom(name, x, y, z, color, 0);
    }

    // --------------------------------------------------------------------
    // If status = 1, we need to resort the zSortMap.
    // If status = 2, we need to put the information of the center
    public int addAtom(String name, float x, float y, float z, int status)
    {
        return addAtom(name, x, y, z, null, status);
    }

    // --------------------------------------------------------------------
    // If status = 1, we need to resort the zSortMap.
    // If status = 2, we need to put the information of the center
    public int addAtom(String name, float x, float y, float z, Color color,
      int status)
    {
        // Find the atom type for the new atom; if not found, add the
	// proper new atom type.
        DEViseAtomType type = null;
        if (name != null) {
            for (int i = 0; i < atomTypeList.size(); i++) {
                DEViseAtomType at = (DEViseAtomType)atomTypeList.elementAt(i);
                if ((name.toLowerCase()).equals(at.name.toLowerCase())) {
                    type = at;
                    break;
                }
            }

            if (type == null) {
                if (color != null) {
                    type = new DEViseAtomType(name, color);
                } else {
                    type = new DEViseAtomType(name);
                }

                atomTypeList.addElement(type);
            }
        }

        // add new atom into atom list, if possible, overwrite deleted atom
        DEViseAtomInCrystal newAtom = new DEViseAtomInCrystal(type, x, y, z);
	newAtom.color = color;
	newAtom.realColor = color;

        int index;
        if (!deletedAtomIndexStack.empty()) {
            index = ((Integer)deletedAtomIndexStack.pop()).intValue();
            atomList.setElementAt(newAtom, index);
        } else {
            atomList.addElement(newAtom);
            index = atomList.size() - 1;
        }

        // add new atom index into the end of zSortMap
        zSortMapSize++;
        if (zSortMapSize > zSortMap.length) {
            // zSortMap capacity is not large enough, need to increase its size
            int[] newMap = new int[zSortMap.length + zSortMapSizeIncrement];
            System.arraycopy(zSortMap, 0, newMap, 0, zSortMap.length);
            newMap[zSortMapSize - 1] = index;
            for (int i = zSortMapSize; i < newMap.length; i++) {
                newMap[i] = -1;
            }
            zSortMap = newMap;
        } else {
            zSortMap[zSortMapSize - 1] = index;
        }

        float[] pos1 = new float[3];
        oldlcs.point(newAtom.pos, pos1);

        // update the total size
        newTotalX += pos1[0];
        newTotalY += pos1[1];
        newTotalZ += pos1[2];
        if (pos1[0] > maxiX) {
            maxiX = pos1[0];
        }
        if (pos1[0] < miniX) {
            miniX = pos1[0];
        }
        if (pos1[1] > maxiY) {
            maxiY = pos1[1];
        }
        if (pos1[1] < miniY) {
            miniY = pos1[1];
        }
        if (pos1[2] > maxiZ) {
            maxiZ = pos1[2];
        }
        if (pos1[2] < miniZ) {
            miniZ = pos1[2];
        }
        totalAtomNumber++;

        // If status = 1, we need to resort the zSortMap.
        // If status = 2, we need to put the information of the center
	// position and the maximum distance into transformation matrix.
        if (status == 1) {
            resort();
            lcs.point(newAtom.pos, newAtom.lcspos);
        } else if (status == 2) {
	    center();
        } else if (status == 0) {
	    // do nothing
	} else {
	    throw new YError("Illegal status value: " + status);
	}

        return index;
    }

    // --------------------------------------------------------------------
    public DEViseAtomInCrystal getAtom(int index)
    {
        if (index < 0 || index >= atomList.size()) {
            return null;
        } else {
            return (DEViseAtomInCrystal)atomList.elementAt(index);
        }
    }

    // --------------------------------------------------------------------
    private void center()
    {
        float dx = (float)Math.abs(maxiX - miniX),
          dy = Math.abs(maxiY - miniY),
	  dz = Math.abs(maxiZ - miniZ), maxi = dx;
        if (dy > maxi) {
            maxi = dy;
        }
        if (dz > maxi) {
            maxi = dz;
        }
        //newBoxLength = 2 * maxi;
        newBoxLength = maxi;
        resetAll(true);
    }

    // --------------------------------------------------------------------
    public synchronized void resize(int width, int height)
    {
        if (width > 0 && height > 0) {
            viewArea.width = width;
            viewArea.height = height;
            resetAll(false);
        }
    }

    // --------------------------------------------------------------------
    public synchronized void resetAll(boolean flag)
    {
        if (flag) {
            boxLength = newBoxLength;
            totalX = newTotalX;
            totalY = newTotalY;
            totalZ = newTotalZ;
        }

        int size = (viewArea.width > viewArea.height) ? viewArea.height :
	  viewArea.width;
        pixelToDataUnit = size / boxLength;

        shiftedX = totalShiftedX + viewArea.width / 2 + viewShift;
        shiftedY = totalShiftedY + viewArea.height / 2 + viewShift;

        for (int i = 0; i < atomTypeList.size(); i++) {
            DEViseAtomType at = (DEViseAtomType)atomTypeList.elementAt(i);
            at.reset(pixelToDataUnit);
        }

        try {
            lcs = new DEVise3DLCS(oldlcs);
        } catch (YException e) { // will not happen

        }

        float x = totalX / totalAtomNumber,
	  y = totalY / totalAtomNumber,
	  z = totalZ / totalAtomNumber;
        lcs.translate(-x, -y, -z);
        lcs.scale(pixelToDataUnit);

        lcs.scale(totalScaleFactor);
        lcs.xrotate(totalXRotation);
        lcs.yrotate(totalYRotation);

        viewDistance = boxViewRatio * boxLength * pixelToDataUnit *
	  totalScaleFactor;
        halfViewDistance = 0.5f * viewDistance;

        isReady = true;
        isTransformed = true;
    }

    // --------------------------------------------------------------------
    // Sort atoms by transformed Z (increasing or decreasing???).
    private void resort()
    {
        quickindex(zSortMapSize, zSortMap);
    }

    // --------------------------------------------------------------------
    private void quickIndexSwap(int i, int j)
    {
        int t = zSortMap[i];
        zSortMap[i] = zSortMap[j];
        zSortMap[j] = t;
    }

    // --------------------------------------------------------------------
    private int quickIndexCompare(int i, int j)
    {
        if (i < 0) {
            if (j < 0) {
                return 0;
            } else {
                return -1;
            }
        } else {
            if (j < 0) {
                return 1;
            }
        }

        DEViseAtomInCrystal atom1 = (DEViseAtomInCrystal)atomList.elementAt(i),
	  atom2 = (DEViseAtomInCrystal)atomList.elementAt(j);
        if (atom1.lcspos[2] > atom2.lcspos[2]) {
            return -1;
        } else if (atom1.lcspos[2] < atom2.lcspos[2]) {
            return 1;
        } else {
            return 0;
        }
    }

    // --------------------------------------------------------------------
    private void quickindex(int n, int[] map)
    {
        // functions needed by this function
        // quickIndexCompare(int map[i], int map[j]):
        //       assume the value indexed by map[i] and map[j] is v_mi and v_mj,
        //       then this function return 0 if v_mi = v_mj, return negative if
        //       v_mi < v_mj (or v_mi > v_mj if you want the sorted order is
        //       decending instead of ascending), otherwise, return positive
        // quickIndexSwap(int i, int j):
        //       swap the value stored in map[i] and map[j]

        // M:       while the number of items is less than M, direct sort is
        //          actually faster than quick sort
        // NSTACK:  buffer size, the total number of items you can sort use
        //          this method should be less than 2^NSTACK
        int M = 7, NSTACK = 50, jstack = -1;
        int[] istack = new int[NSTACK];

        // no need to sort if number of items is less than 2
        if (n < 2) {
            return;
        }

        // tempory variables
        int index, i, j, k, ir = n - 1, l = 0;

        for (;;) {
            if (ir - l < M) {
                for (j = l + 1; j <= ir; j++) {
                    index = map[j];
                    for (i = j - 1; i >= l; i--) {
                        if (quickIndexCompare(map[i], index) <= 0)
                            break;
                        map[i + 1] = map[i];
                    }

                    map[i + 1] = index;
                }

                if (jstack == -1)
                    break;

                ir = istack[jstack--];
                l = istack[jstack--];
            } else {
                k = (l + ir) >> 1;

                quickIndexSwap(k, l + 1);
                if (quickIndexCompare(map[l], map[ir]) > 0) {
                    quickIndexSwap(l, ir);
                }
                if (quickIndexCompare(map[l + 1], map[ir]) > 0) {
                    quickIndexSwap(l + 1, ir);
                }
                if (quickIndexCompare(map[l], map[l + 1]) > 0) {
                    quickIndexSwap(l, l + 1);
                }

                i = l + 1;
                j = ir;
                index = map[l + 1];

                for (;;) {
                    do {
                        i++;
                    } while (quickIndexCompare(map[i], index) < 0);

                    do {
                        j--;
                    } while (quickIndexCompare(map[j], index) > 0);

                    if (j < i) {
                        break;
                    }

                    quickIndexSwap(i, j);
                }

                map[l + 1] = map[j];
                map[j] = index;

                jstack += 2;
                if (jstack >= NSTACK)
                    throw new YError("NSTACK = " + NSTACK +
		      " is too small in quickindex()");

                if (ir - i + 1 >= j - l) {
                    istack[jstack] = ir;
                    istack[jstack - 1] = i;
                    ir = j - 1;
                } else {
                    istack[jstack] = j - 1;
                    istack[jstack - 1] = l;
                    l = i;
                }
            }
        }
    }

    // --------------------------------------------------------------------
    // Translate the objects in the view; dx and dy are mouse movement in
    // pixels;
    public synchronized void translate(int dx, int dy)
    {
        totalShiftedX += dx;
        totalShiftedY += dy;
        shiftedX += dx;
        shiftedY += dy;

        isTransformed = true;
    }

    // --------------------------------------------------------------------
    // Scale the objects in the view; dx and dy are mouse movement in
    // pixels;
    public synchronized void scale(int dx, int dy)
    {
        float factor = (float)dy / (float)viewArea.height;

        factor = factor + totalScaleFactor;

        float d = factor / totalScaleFactor;
        lcs.scale(d, d, d);
        halfViewDistance *= d;
        viewDistance *= d;

        totalScaleFactor = factor;
        isTransformed = true;
    }

    // --------------------------------------------------------------------
    // Rotate the objects in the view; dx and dy are mouse movement in
    // pixels;
    public synchronized void rotate(int dx, int dy)
    {
        int dxx = Math.abs(dx * -2);
        int dyy = Math.abs(dy * 2);

        while (dxx > viewArea.height) {
            dxx -= viewArea.height;
            if (dx > 0) {
                lcs.yrotate(-90);
                totalYRotation -= 90;
            } else {
                lcs.yrotate(90);
                totalYRotation += 90;
            }
        }
        while (dyy > viewArea.width) {
            dyy -= viewArea.width;
            if (dy > 0) {
                lcs.xrotate(90);
                totalXRotation += 90;
            } else {
                lcs.xrotate(-90);
                totalXRotation -= 90;
            }
        }

        dxx = (dx > 0) ? -dxx : dxx;
        dyy = (dy > 0) ? dyy : -dyy;

        float anglex = (float)Math.asin((float)dyy / viewArea.width) *
	  YGlobals.rad;
        float angley = (float)Math.asin((float)dxx / viewArea.height) *
	  YGlobals.rad;

        lcs.xrotate(anglex);
        lcs.yrotate(angley);

        totalXRotation += anglex;
        totalYRotation += angley;

        isTransformed = true;
    }

    // --------------------------------------------------------------------
    // Set all atoms to not be selected.
    public void setSelect()
    {
        DEViseAtomInCrystal atom = null;

        for (int i = 0; i < atomList.size(); i++) {
            atom = (DEViseAtomInCrystal)atomList.elementAt(i);

            atom.isSelected = false;
            atom.color = null;
        }
    }

    // --------------------------------------------------------------------
    // Set the atom (if any) at the given (non-transformed) location
    // to be selected (highlighted).
    //TEMP -- linear search here! -- slow!!
    public void setSelect(float x, float y, float z, Color c, boolean isBond)
    {
        DEViseAtomInCrystal atom = null;
        float xx, yy, zz;

        for (int i = 0; i < atomList.size(); i++) {
            atom = (DEViseAtomInCrystal)atomList.elementAt(i);

            xx = (float)Math.abs(x - atom.pos[0]);
            yy = (float)Math.abs(y - atom.pos[1]);
            zz = (float)Math.abs(z - atom.pos[2]);
            if (xx < 1.0e-5f && yy < 1.0e-5f && zz < 1.0e-5f) {
                if (!isBond) {
                    atom.isSelected = true;
                    if (atom.type != null) atom.type.setSelectColor(c);
                } else {
                    //atom.type.setColor(c);
                    atom.color = c;
                }
            }
        }
    }

    // --------------------------------------------------------------------
    public int getNumberOfAtoms()
    {
        return atomList.size();
    }
}
