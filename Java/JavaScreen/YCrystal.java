// YCrystal.java
// last updated on 09/22/99


import java.awt.*;
import java.util.*;

public class YCrystal
{   
    public int viewAreaX, viewAreaY;
    public Y3DLCS lcs = null;
    public double pixelToX, pixelToY;
    public double shiftX, shiftY, shiftZ; 
    public double box, halfbox;
    
    public YAtom[] atomList = null;    
    public int[] atomType = null;
    public double[][] atomPosLCS = null;
    public double[][] atomPos = null;    
    public int[][] bondMap = null;
    public byte[] bondNum = null;
    private int[] zSortMap = null;
    public boolean isTransformed = false;     
        
    public static Hashtable atomTable = new Hashtable();
    public static YAtom defaultAtom = null;
    static { 
        defaultAtom = new YAtom(null, 10, 0, 0, 0);
        atomTable.put("zn", new YAtom("Zn", 20, 0, 191, 255));
        atomTable.put("se", new YAtom("Se", 20, 161, 161, 161));
    }
    
    public YCrystal(int x, int y, Y3DLCS LCS, String[] at, double[][] ap)
    {
        YGlobals.yassert(LCS != null && at != null && ap != null, "Null argument", "YCrystal constructor");
        YGlobals.yassert(at.length == ap.length, "Inconsistant number of atoms", "YCrystal constructor");
        
        viewAreaX = x;
        viewAreaY = y;
        lcs = LCS;
        
        atomType = new int[at.length];        
        atomPosLCS = ap;
        atomPos = new double[at.length][3];
        zSortMap = new int[at.length];
        bondMap = new int[at.length][8];
        bondNum = new byte[at.length];
        for (int i = 0; i < at.length; i++) {
            for (int j = 0; j < 8; j++) {
                bondMap[i][j] = -1;
            }
            bondNum[i] = 0;
        }
        
        Vector al = new Vector();
        for (int i = 0; i < at.length; i++) {
            YAtom atom = (YAtom)atomTable.get(at[i].toLowerCase());
            if (atom == null) {
                atom = defaultAtom;
            }
            
            if (!al.contains(atom)) {
                al.addElement(atom); // add to the end of Vector
            }
            
            atomType[i] = al.indexOf(atom);
            zSortMap[i] = i;
        }
        
        atomList = new YAtom[al.size()];
        for (int i = 0; i < atomList.length; i++) {
            atomList[i] = (YAtom)al.elementAt(i);
        }
        
        double d, dx, dy, dz, tx = 0.0, ty = 0.0, tz = 0.0, maxd = 0.0;
        for (int i = 0; i < atomType.length; i++) {
            lcs.point(atomPosLCS[i], atomPos[i]);
            tx += atomPos[i][0];
            ty += atomPos[i][1];
            tz += atomPos[i][2];                        
            for (int j = i - 1; j >= 0; j--) {
                dx = atomPos[i][0] - atomPos[j][0];
                dy = atomPos[i][1] - atomPos[j][1];
                dz = atomPos[i][2] - atomPos[j][2];
                d = dx * dx + dy * dy + dz * dz;
                if (d > 0.15 && d < 0.20) {
                    bondMap[i][bondNum[i]] = j;
                    bondNum[i]++;
                    bondMap[j][bondNum[j]] = i;
                    bondNum[j]++;
                }
                if (d > maxd) {
                    maxd = d;
                }
            }    
        }
        
        maxd = Math.sqrt(maxd);
        box = maxd;
        halfbox = 0.5 * maxd;
        pixelToX = maxd / (viewAreaX);
        pixelToY = maxd / (viewAreaY);
        shiftX = tx / atomType.length;
        shiftY = ty / atomType.length;
        shiftZ = tz / atomType.length; 
        lcs.translate(-shiftX, -shiftY, -shiftZ);           
        //System.out.println(pixelToX + " " + pixelToY + " " + shiftX + " " + shiftY + " " + shiftZ);
    }
    
    private void swap(int i, int j)
    {
        int t = zSortMap[i];
        zSortMap[i] = zSortMap[j];
        zSortMap[j] = t;
    }
    
    private void quickindex()
    {   
        int n = atomType.length;
        if (n < 2) {
            return;
        }
        
        int M = 7, NSTACK = 50;        
        int i, indxt, ir = n - 1, itemp, j, k, l = 0, jstack = -1;
        int[] istack = new int[50];       
        double a;
    
        for (j = 0; j < n; j++) 
            zSortMap[j] = j;
        
        for (;;) {
            if (ir - l < M) {
                for (j = l + 1; j <= ir; j++) {
                    indxt = zSortMap[j];
                    a = atomPos[indxt][2];
                    for (i = j - 1; i >= l; i--) {
                        if (atomPos[zSortMap[i]][2] <= a) 
                            break;
                        zSortMap[i + 1] = zSortMap[i];
                    }
                    
                    zSortMap[i + 1] = indxt;
                }
                
                if (jstack == -1) 
                    break;
                    
                ir = istack[jstack--];
                l = istack[jstack--];
            } else {
                k = (l + ir) >> 1;
                
                swap(k, l + 1);
                if (atomPos[zSortMap[l]][2] > atomPos[zSortMap[ir]][2]) {
                    swap(l, ir);
                }
                if (atomPos[zSortMap[l + 1]][2] > atomPos[zSortMap[ir]][2]) {
                    swap(l + 1, ir);
                }
                if (atomPos[zSortMap[l]][2] > atomPos[zSortMap[l + 1]][2]) {
                    swap(l, l + 1);
                }
                
                i = l + 1;
                j = ir;
                indxt = zSortMap[l + 1];
                a = atomPos[indxt][2];
                
                for (;;) {
                    do {
                        i++;
                    } while (atomPos[zSortMap[i]][2] < a);
                    
                    do { 
                        j--;
                    } while (atomPos[zSortMap[j]][2] > a);
                    
                    if (j < i) {
                        break;
                    }    
                        
                    swap(i, j);
                }
                
                zSortMap[l + 1] = zSortMap[j];
                zSortMap[j] = indxt;
                
                jstack += 2;
                if (jstack >= NSTACK)
                    throw new YError("NSTACK is too small", "YCrystal::quickindex()");
                    
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
                
    private void transform()
    { 
        if (isTransformed) {
            return;
        }
                
        for (int i = 0; i < atomType.length; i++) {
            lcs.point(atomPosLCS[i], atomPos[i]);
        }
        
        quickindex();    
    }
    
    public void paint(Component component, Graphics gc) 
    {
        if (component == null || gc == null) {
            return;
        }
        
        transform();
        
        YAtom atom = null, batom = null;
        int x, y, m, n, x1, y1;
        //int[] xp = new int[4], yp = new int[4];
        double z;
        byte[] flag = new byte[atomType.length];
        //System.out.println("Start");
        for (int i = atomType.length - 1; i >= 0; i--) {            
            n = zSortMap[i];
            flag[n] = 1;
            atom = (YAtom)atomList[atomType[n]]; 
            x = (int)((atomPos[n][0]) / pixelToX + 0.5) + viewAreaX / 2;
            y = (int)((atomPos[n][1]) / pixelToY + 0.5) + viewAreaY / 2;
            
            //xp[0] = x;
            //xp[2] = x;
            //yp[0] = y + 1;
            //yp[2] = y - 1;
            for (int j = 0; j < bondNum[n]; j++) {                
                m = bondMap[n][j];
                if (flag[m] == 1) {
                    continue;
                }
                batom = (YAtom)atomList[atomType[m]];
                x1 = (int)((atomPos[m][0]) / pixelToX + 0.5) + viewAreaX / 2;
                y1 = (int)((atomPos[m][1]) / pixelToY + 0.5) + viewAreaY / 2;
                
                gc.drawLine(x, y, x1, y1);
                gc.drawLine(x, y - 1, x1, y1 - 1);
                gc.drawLine(x, y + 1, x1, y1 + 1);
                gc.drawLine(x, y - 1, x1, y1 + 1);
                gc.drawLine(x, y + 1, x1, y1 - 1);
                
                //xp[1] = x1;
                //xp[3] = x1;
                //yp[1] = y1 + 1;
                //yp[3] = y1 - 1;
                //gc.fillPolygon(xp, yp, 4);
                
                //System.out.println("n, m, posn, posm " + n + " " + m + " " + atomPos[n][2] + " " + atomPos[m][2]);
            }    
                
            //System.out.println("" + atomPos[zSortMap[i]][0] + " " + atomPos[zSortMap[i]][1] + " " + atomPos[zSortMap[i]][2]);
            z = (halfbox - atomPos[n][2]) / box;
            atom.paint(component, gc, x, y, (float)z);
        }      
    }    
            
}    
        