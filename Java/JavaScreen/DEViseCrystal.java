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
    double boxLength, viewDistance, halfViewDistance, boxViewRatio = 1.0, pixelToDataUnit = 0.1;
    double newBoxLength, newTotalX, newTotalY, newTotalZ, maxiX = -1000000.0, miniX = 10000000.0, maxiY = -10000000.0, miniY = 10000000.0, maxiZ = -10000000.0, miniZ = 1000000.0;

    double totalX, totalY, totalZ, totalScaleFactor = 1.0, totalXRotation, totalYRotation;
    int shiftedX, shiftedY, totalShiftedX, totalShiftedY, totalAtomNumber;

    double minBondLength, maxBondLength;
    public int bondWidth = 2;

    public Color selectColor = Color.cyan;
    public Color axisColor = Color.red;

    Vector atomTypeList = new Vector();
    Vector atomList = new Vector();
    Stack deletedAtomIndexStack = new Stack();

    int zSortMapSizeIncrement = 64, zSortMapSize, deletedAtomNumber;
    int[] zSortMap = new int[64];

    public static Hashtable AtomTable = new Hashtable();
    static {
        AtomTable.put("se", new DEViseAtomType("Se", 1.25, 200, 200, 0));
        AtomTable.put("zn", new DEViseAtomType("Zn", 1.17, 192, 192, 192));
    }

    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS, double min, double max)
    {
        YGlobals.yassert(LCS != null, "Null argument", "DEViseCrystal::constructor");
        YGlobals.yassert(width > 0 && height > 0, "Invalid size argument", "DEViseCrystal::constructor");

        viewArea.width = width;
        viewArea.height = height;
        viewShift = shift;
        minBondLength = min;
        maxBondLength = max;
        oldlcs = LCS;

        for (int i = 0; i < zSortMap.length; i++) {
            zSortMap[i] = -1;
        }

        newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS, double min, double max, String[] atomName, double[][] atomPos)
    {
        YGlobals.yassert(LCS != null, "Null argument", "DEViseCrystal::constructor");
        YGlobals.yassert(width > 0 && height > 0, "Invalid size argument", "DEViseCrystal::constructor");

        viewArea.width = width;
        viewArea.height = height;
        viewShift = shift;
        minBondLength = min;
        maxBondLength = max;
        oldlcs = LCS;

        for (int i = 0; i < zSortMap.length; i++) {
            zSortMap[i] = -1;
        }

        if (atomName != null && atomPos != null && atomName.length >= atomPos.length) {
            int i;

            for (i = 0; i < atomName.length - 1; i++) {
                addAtom(atomName[i], atomPos[i][0], atomPos[i][1], atomPos[i][2]);
            }

            addAtom(atomName[i], atomPos[i][0], atomPos[i][1], atomPos[i][2], 2);
        }

        //newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    public DEViseCrystal(int width, int height, int shift, DEVise3DLCS LCS, double min, double max, String[] atomName, double[][] atomPos, Color[] atomColor, double[][][] bondPos, Color[] bondColor)
    {
        YGlobals.yassert(LCS != null, "Null argument", "DEViseCrystal::constructor");
        YGlobals.yassert(width > 0 && height > 0, "Invalid size argument", "DEViseCrystal::constructor");

        viewArea.width = width;
        viewArea.height = height;
        viewShift = shift;
        minBondLength = min;
        maxBondLength = max;
        oldlcs = LCS;

        for (int i = 0; i < zSortMap.length; i++) {
            zSortMap[i] = -1;
        }

        if (atomName != null && atomPos != null && atomColor != null && atomName.length >= atomPos.length && atomColor.length >= atomPos.length) {
            int i;

            for (i = 0; i < atomName.length - 1; i++) {
                addAtom(atomName[i], atomPos[i][0], atomPos[i][1], atomPos[i][2], atomColor[i]);
            }

            addAtom(atomName[i], atomPos[i][0], atomPos[i][1], atomPos[i][2], atomColor[i], 2);
        }

        if (bondPos != null && bondColor != null && bondColor.length >= bondPos.length) {
            int i, index1, index2;
            for (i = 0; i < bondPos.length - 1; i++) {
                index1 = addAtom(bondColor[i].toString(), bondPos[i][0][0], bondPos[i][0][1], bondPos[i][0][2], bondColor[i]);
                index2 = addAtom(bondColor[i].toString(), bondPos[i][1][0], bondPos[i][1][1], bondPos[i][1][2], bondColor[i]);
                DEViseAtomInCrystal atom1 = getAtom(index1), atom2 = getAtom(index2);
                atom1.addBond(index2);
                atom2.addBond(index1);
            }
            index1 = addAtom(bondColor[i].toString(), bondPos[i][0][0], bondPos[i][0][1], bondPos[i][0][2], bondColor[i]);
            index2 = addAtom(bondColor[i].toString(), bondPos[i][1][0], bondPos[i][1][1], bondPos[i][1][2], bondColor[i], 2);
            DEViseAtomInCrystal atom1 = getAtom(index1), atom2 = getAtom(index2);
            atom1.addBond(index2);
            atom2.addBond(index1);
        }

        //newBoxLength = oldlcs.pointDistance(0, 0, 0, 1, 1, 1);
    }

    public synchronized void paint(Component component, Graphics gc, boolean isMove)
    {
        if (component == null || gc == null || !isReady) {
            return;
        }

        if (isTransformed) {
            for (int i = 0; i < atomList.size(); i++) {
                DEViseAtomInCrystal atom = (DEViseAtomInCrystal)atomList.elementAt(i);
                if (!atom.status) { // ignore deleted atom
                    continue;
                }

                lcs.point(atom.pos, atom.lcspos);
            }

            resort();

            isTransformed = false;
        }

        int x, y, x1, y1, xm, ym, index, index1;
        double z;
        double[] pos = new double[3];
        byte[] flag = new byte[atomList.size()];
        DEViseAtomInCrystal atom = null, atom1 = null;

        Color oldcolor = gc.getColor(), color = null, color1 = null;
        boolean isBond = false;

        for (int i = 0; i < zSortMapSize; i++) {
            index = zSortMap[i];
            if (index < 0) { // ignore deleted atom
                continue;
            }

            atom = (DEViseAtomInCrystal)atomList.elementAt(index);
            x = (int)(atom.lcspos[0] + 0.5) + shiftedX;
            y = (int)(atom.lcspos[1] + 0.5) + shiftedY;

            flag[index] = 1;

            if (atom.color == null) {
                color = atom.type.color;
            } else {
                color = atom.color;
            }

            isBond = false;

            // draw the bond first
            for (int j = 0; j < atom.bondNumber; j++) {
                isBond = true;
                index1 = atom.bond[j];
                if (flag[index1] == 1) { // bond already drawed
                    continue;
                }

                atom1 = (DEViseAtomInCrystal)atomList.elementAt(index1);
                x1 = (int)(atom1.lcspos[0] + 0.5) + shiftedX;
                y1 = (int)(atom1.lcspos[1] + 0.5) + shiftedY;

                //xm = (x + x1) / 2;
                //ym = (y + y1) / 2;

                //color1 = atom1.type.color;

                //if (atom.selectedBond[j]) {
                //    color = selectColor;
                //    color1 = selectColor;
                //}

                gc.setColor(color);
                gc.drawLine(x, y, x1, y1);

                //gc.drawLine(x, y, xm, ym);
                //for (int k = 1; k < bondWidth + 1; k++) {
                //    gc.drawLine(x, y - k, xm, ym - k);
                //    gc.drawLine(x, y + k, xm, ym + k);
                //    gc.drawLine(x - k, y, xm - k, ym);
                //    gc.drawLine(x + k, y, xm + k, ym);
                //}

                //gc.setColor(color1);
                //gc.drawLine(xm, ym, x1, y1);
                //for (int k = 1; k < bondWidth + 1; k++) {
                //    gc.drawLine(xm, ym - k, x1, y1 - k);
                //    gc.drawLine(xm, ym + k, x1, y1 + k);
                //    gc.drawLine(xm - k, ym, x1 - k, y1);
                //    gc.drawLine(xm + k, ym, x1 + k, y1);
                //}
            }

            z = (halfViewDistance - atom.lcspos[2]) / viewDistance * totalScaleFactor;

            if (!isBond) {
            if (atom.isSelected == 0) {
                if (isMove) {
                    atom.type.paint(component, gc, x, y, z, 2);
                } else {
                    atom.type.paint(component, gc, x, y, z, 0);
                }
            } else {
                if (isMove) {
                    atom.type.paint(component, gc, x, y, z, 3);
                } else {
                    atom.type.paint(component, gc, x, y, z, 1);
                }
            }
            }
            atom.drawX = x;
            atom.drawY = y;
            atom.drawSize = atom.type.drawSize;
        }

        gc.setColor(axisColor);
        Font oldfont = gc.getFont(), font = new Font("Serif", Font.PLAIN, 12);
        gc.setFont(font);

        z = 20 / pixelToDataUnit;
        pos = lcs.point(z, 0, 0, false);
        x = (int)(pos[0] + 0.5) + shiftedX;
        y = (int)(pos[1] + 0.5) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("a", x, y);
        pos = lcs.point(0, z, 0, false);
        x = (int)(pos[0] + 0.5) + shiftedX;
        y = (int)(pos[1] + 0.5) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("b", x, y);
        pos = lcs.point(0, 0, z, false);
        x = (int)(pos[0] + 0.5) + shiftedX;
        y = (int)(pos[1] + 0.5) + shiftedY;
        gc.drawLine(shiftedX, shiftedY, x, y);
        gc.drawString("c", x, y);

        gc.setColor(oldcolor);
        gc.setFont(oldfont);
    }

    public double[] getPos(double[] pos)
    {
        return oldlcs.point(pos);
    }

    public int addAtom(String name, double x, double y, double z)
    {
        return addAtom(name, x, y, z, null, 0);
    }

    public int addAtom(String name, double x, double y, double z, Color color)
    {
        return addAtom(name, x, y, z, color, 0);
    }

    public int addAtom(String name, double x, double y, double z, int status)
    {
        return addAtom(name, x, y, z, null, status);
    }

    public int addAtom(String name, double x, double y, double z, Color color, int status)
    {
        if (name == null) {
            name = new String("UnknownAtom");
        }

        // find the atom type for the new atom, if not found, add a new atom type
        DEViseAtomType at = null, type = null;
        for (int i = 0; i < atomTypeList.size(); i++) {
            at = (DEViseAtomType)atomTypeList.elementAt(i);
            if ((name.toLowerCase()).equals(at.name.toLowerCase())) {
                type = at;
                break;
            }
        }

        if (type == null) {
            at = (DEViseAtomType)AtomTable.get(name.toLowerCase());
            if (at == null) {
                if (color != null) {
                    at = new DEViseAtomType(name, color);
                } else {
                    at = new DEViseAtomType(name);
                }
            }

            atomTypeList.addElement(at);
            type = at;
        }

        // add new atom into atom list, if possible, overwrite deleted atom
        DEViseAtomInCrystal newAtom = new DEViseAtomInCrystal(type, x, y, z);
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

        // find the bond information for the new atom
        DEViseAtomInCrystal atom = null;
        //double dx, dy, dz, d;
        double[] pos1 = new double[3], pos2 = new double[3];
        oldlcs.point(newAtom.pos, pos1);
        /*
        boolean checkBond = (minBondLength > 0.0 && maxBondLength > 0.0 && maxBondLength > minBondLength);
        for (int i = 0; i < atomList.size(); i++) {
            atom = (DEViseAtomInCrystal)atomList.elementAt(i);

            if (!atom.status) { // ignore deleted atom
                continue;
            }

            // calculate the distance between two atoms
            oldlcs.point(atom.pos, pos2);

            dx = pos1[0] - pos2[0];
            dy = pos1[1] - pos2[1];
            dz = pos1[2] - pos2[2];
            d = Math.sqrt(dx * dx + dy * dy + dz * dz);

            // decide whether or not the two atoms has a bond

            if (checkBond && d > minBondLength && d < maxBondLength) {
                // should we check whether or not number of bond for an atom is exceeding 8?
                newAtom.addBond(i);
                atom.addBond(index);
            }

            // update the maximum distance between any two atoms
            if (d > newBoxLength) {
                newBoxLength = d;
            }
        }
        */
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

        // if status = 1, we need to resort the zSortMap
        // if status = 2, we need to put the information of the center position
        // and the maximum distance into transformation matrix
        if (status == 1) {
            resort();
            lcs.point(newAtom.pos, newAtom.lcspos);
        } else if (status == 2) {
            double dx = Math.abs(maxiX - miniX), dy = Math.abs(maxiY - miniY), dz = Math.abs(maxiZ - miniZ), maxi = dx;
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

        return index;
    }

    public synchronized void removeAtom(int index)
    {
        // index is the index in zSortMap

        DEViseAtomInCrystal atom = (DEViseAtomInCrystal)atomList.elementAt(zSortMap[index]);
        atom.status = false;
        for (int i = 0; i < atom.bondNumber; i++) {
            DEViseAtomInCrystal a = (DEViseAtomInCrystal)atomList.elementAt(atom.bond[i]);
            a.removeBond(zSortMap[index]);
        }
        atom.bondNumber = 0;

        oldlcs.point(atom.pos, atom.lcspos);
        newTotalX -= atom.pos[0];
        newTotalY -= atom.pos[1];
        newTotalZ -= atom.pos[2];
        //newBoxLength = ?

        deletedAtomIndexStack.push(new Integer(zSortMap[index]));
        zSortMap[index] = -1;
        deletedAtomNumber++;
        totalAtomNumber--;
    }

    public DEViseAtomInCrystal getAtom(int index)
    {
        if (index < 0 || index >= atomList.size()) {
            return null;
        } else {
            return (DEViseAtomInCrystal)atomList.elementAt(index);
        }
    }

    public synchronized void resize(int width, int height)
    {
        if (width > 0 && height > 0) {
            viewArea.width = width;
            viewArea.height = height;
            resetAll(false);
        }
    }

    private synchronized void resetAll(boolean flag)
    {
        if (flag) {
            boxLength = newBoxLength;
            totalX = newTotalX;
            totalY = newTotalY;
            totalZ = newTotalZ;
        }

        int size = (viewArea.width > viewArea.height) ? viewArea.height : viewArea.width;
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

        double x = totalX / totalAtomNumber, y = totalY / totalAtomNumber, z = totalZ / totalAtomNumber;
        lcs.translate(-x, -y, -z);
        lcs.scale(pixelToDataUnit);

        lcs.scale(totalScaleFactor);
        lcs.xrotate(totalXRotation);
        lcs.yrotate(totalYRotation);

        viewDistance = boxViewRatio * boxLength * pixelToDataUnit * totalScaleFactor;
        halfViewDistance = 0.5 * viewDistance;

        isReady = true;
        isTransformed = true;
    }

    private void resort()
    {
        quickindex(zSortMapSize, zSortMap);

        zSortMapSize -= deletedAtomNumber;
        deletedAtomNumber = 0;
    }

    private void quickIndexSwap(int i, int j)
    {
        int t = zSortMap[i];
        zSortMap[i] = zSortMap[j];
        zSortMap[j] = t;
    }

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

        DEViseAtomInCrystal atom1 = (DEViseAtomInCrystal)atomList.elementAt(i), atom2 = (DEViseAtomInCrystal)atomList.elementAt(j);
        if (atom1.lcspos[2] > atom2.lcspos[2]) {
            return -1;
        } else if (atom1.lcspos[2] < atom2.lcspos[2]) {
            return 1;
        } else {
            return 0;
        }
    }

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
                    throw new YError("NSTACK = " + NSTACK + " is too small in quickindex()");

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

    public synchronized void translate(int dx, int dy)
    {
        totalShiftedX += dx;
        totalShiftedY += dy;
        shiftedX += dx;
        shiftedY += dy;

        isTransformed = true;
    }

    public synchronized void scale(int dx, int dy)
    {
        double factor = Math.sqrt((double)dx * dx + (double)dy * dy) / Math.sqrt((double)viewArea.width * viewArea.width + (double)viewArea.height * viewArea.height);
        if (dx < 0) {
            factor = -factor;
        }
        factor = factor + totalScaleFactor;

        double d = factor / totalScaleFactor;
        lcs.scale(d, d, d);
        halfViewDistance *= d;
        viewDistance *= d;

        totalScaleFactor = factor;
        isTransformed = true;
    }

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

        double anglex = Math.asin((double)dyy / viewArea.width) * YGlobals.rad;
        double angley = Math.asin((double)dxx / viewArea.height) * YGlobals.rad;

        lcs.xrotate(anglex);
        lcs.yrotate(angley);

        totalXRotation += anglex;
        totalYRotation += angley;

        isTransformed = true;
    }

    public int find(int x, int y)
    {
        int index1 = -1, index2 = -1;
        int tmp;
        double r1, r2, dx, dy;
        DEViseAtomInCrystal atom1 = null, atom2 = null;
        byte[] flag = new byte[atomList.size()];

        for (int i = zSortMapSize - 1; i >= 0; i--) {
            index1 = zSortMap[i];
            if (index1 < 0) { // ignore deleted atom
                continue;
            }

            atom1 = (DEViseAtomInCrystal)atomList.elementAt(index1);

            r1 = atom1.drawSize / 2.0;
            dx = x - atom1.drawX;
            dy = y - atom1.drawY;
            r2 = Math.sqrt(dx * dx + dy * dy);
            if (r2 < r1) {
                break;
            }

            flag[index1] = 1;

            // check the bonds
            for (int j = 0; j < atom1.bondNumber; j++) {
                index2 = atom1.bond[j];

                if (flag[index2] != 1) {
                    index2 = -1;
                    continue;
                }

                atom2 = (DEViseAtomInCrystal)atomList.elementAt(index2);

                if ((x >= atom2.drawX - bondWidth && x <= atom1.drawX + bondWidth) || (x >= atom1.drawX - bondWidth && x <= atom2.drawX + bondWidth)) {
                    tmp = (int)((double)(atom2.drawY - atom1.drawY) / (atom2.drawX - atom1.drawX) * (x - atom1.drawX)) + atom1.drawY;

                    if (y >= (tmp - bondWidth) && y <= (tmp + bondWidth)) {
                        atom1.lastSelectedBondIndex = index2;
                        break;
                    } else {
                        index2 = -1;
                    }
                } else {
                    index2 = -1;
                }
            }

            if (index2 != -1) {
                break;
            } else {
                index1 = -1;
            }
        }

        return index1;
    }

    public void setSelect()
    {
        DEViseAtomInCrystal atom = null;

        for (int i = 0; i < atomList.size(); i++) {
            atom = (DEViseAtomInCrystal)atomList.elementAt(i);

            if (!atom.status) { // ignore deleted atom
                continue;
            }

            atom.isSelected = 0;
            atom.color = null;
        }
    }

    public void setSelect(double x, double y, double z, Color c, boolean isBond)
    {
        DEViseAtomInCrystal atom = null;
        double xx, yy, zz;

        for (int i = 0; i < atomList.size(); i++) {
            atom = (DEViseAtomInCrystal)atomList.elementAt(i);

            if (!atom.status) { // ignore deleted atom
                continue;
            }

            xx = Math.abs(x - atom.pos[0]);
            yy = Math.abs(y - atom.pos[1]);
            zz = Math.abs(z - atom.pos[2]);
            if (xx < 1.0e-5 && yy < 1.0e-5 && zz < 1.0e-5) {
                if (!isBond) {
                    atom.isSelected = 1;
                    atom.type.setSelectColor(c);
                } else {
                    //atom.type.setColor(c);
                    atom.color = c;
                }
            }
        }
    }

    public int getNumberOfAtoms()
    {
        return atomList.size();
    }
}


