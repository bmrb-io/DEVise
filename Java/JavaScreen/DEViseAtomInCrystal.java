// DEViseAtomInCrystal.java
// last updated on 10/05/99


public class DEViseAtomInCrystal
{
    public DEViseAtomType type = null;

    public boolean status, isSelected;

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
        isSelected = false;
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

