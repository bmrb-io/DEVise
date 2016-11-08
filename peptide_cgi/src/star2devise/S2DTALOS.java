// ========================================================================
// (c) Copyright 2003-2008
// By the BioMagResBank
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the BioMagResBank.

// ------------------------------------------------------------------------

// $Id$

// ------------------------------------------------------------------------

//package BMRB.chemShift;
package star2devise;

import java.io.*;
import java.net.*;
import java.util.*;

import star2devise.*;
import EDU.bmrb.starlibj.*;

//TEMP -- need to find a BMRB/PDB pair with misaligned sequences to test TALOS matching
/**
* Uses tables in the .str format to
* find different kinds of information about
* chemical shifts.
*
* @author Dan Sinclair, BMRB 6/2003
*/
public class S2DTALOS {
    private static int DEBUG = 0;

    private S2DDataSequence theDataSequence;   // a sequence of the amino-acid codes in one-letter format

    /**
    * Default Class Constructor.
    *
    */
    public S2DTALOS() {
        theDataSequence = new S2DDataSequence();
    }

    /**
    * Set the S2DDataSequence of this object (added for S2D matching).
    * @param The S2DDataSequence for this object.
    */
    public void setDataSequence(S2DDataSequence ds) {
        theDataSequence = ds;
    }

    /**
    * FORMERLY: seq_compare.tcl. Compares the contents of this object's
    * {@link S2DDataSequence} with that of a {@link PDB} Object.
    *
    * @param  thePDB  the {@link PDB} with which to compare the {@link S2DDataSequence}s
    * @return         an error code corresponding to the success of the function.
    *                 ( < 0 if something went wrong )
    */
    public int seqCompare(S2DDataSequence otherDS, Set resMismatchList) {
        int errcode = 1;
        S2DDataSequence thePDBds = otherDS;

        if (DEBUG >= 1) {
            System.out.println("\n\n** TALOS.seqCompare");
        }

        while ( (errcode > 0) && (errcode < 3) ) {

            if (DEBUG >= 1) {
                System.out.println("--------------------------");
                System.out.println("This is try No. " + errcode);

                System.out.println("--------------------------");
                System.out.println("This sequence");
                System.out.println("\n" + theDataSequence.toString() + "\n");

                System.out.println("--------------------------");
                System.out.println("Other sequence");
                System.out.println("\n" + thePDBds.toString() + "\n");
                System.out.println("--------------------------");
            }

            if (theDataSequence.equals(thePDBds)) {
                if (DEBUG >= 1) {
                    System.out.println(" The two data sequences are identical");
                }
                errcode = 0;
            } else if ( theDataSequence.size() == thePDBds.size() ) {
                errcode = 0;
                if (DEBUG >= 1) {
                    System.out.println(" The two data sequences are not " +
                                       "equivalent:\n---");
                }
                //TEMP -- if we get to here, we're treating this as a match -- is that correct????

                int numMismatches = 0;
                for (int i = 0; i < theDataSequence.size(); i++ ) {
                    if ( !((String)theDataSequence.get(i)).equals(
                                (String)thePDBds.get(i) ) ) {
                        if (DEBUG >= 1) {
                            System.out.println("  REMARK Mismatch: Residue " + (i + 1) +
                                               "   BMRB: " + theDataSequence.get(i) + "   PDB: " +
                                               thePDBds.get(i) + "\n---");
                        }
                        // +1 is because i starts at 0, but residue numbers start at 1.
                        resMismatchList.add(new Integer(i+1));
                        numMismatches++;
                    }
                }
                if (numMismatches > 0) {
                    if (DEBUG >= 1) {
                        System.out.println("\n  REMARK Total " + numMismatches +
                                           " mismatch(es).");
                    }
                }
            } else {
                errcode++;
                if (DEBUG >= 1) {
                    System.out.println(" The two data sequences have different " +
                                       "lengths. They are being corrected.");
                }
                String sTALOSds = theDataSequence.toString();
                String sPDBds = thePDBds.toString();
                if (DEBUG >= 3) {
                    System.out.println("DIAG sTALOSds = " + sTALOSds);
                    System.out.println("DIAG sPDBds = " + sPDBds);
                }

                final int LENGTH = 7;

                //TEMP -- this should probably get cleaned up some.
                if (sTALOSds.length() < LENGTH || sPDBds.length() < LENGTH) {
                    // Sequences are too short to try to match.
                    errcode = 3;
                    break;
                }

                String beginTALOS = sTALOSds.substring(0, LENGTH-1);
                String beginPDB = sPDBds.substring(0, LENGTH-1);
                if (DEBUG >= 3) {
                    System.out.println("DIAG beginTALOS = " + beginTALOS);
                    System.out.println("DIAG beginPDB = " + beginPDB);
                }

                String endTALOS = sTALOSds.substring(sTALOSds.length() - LENGTH);
                String endPDB = sPDBds.substring(sPDBds.length() - LENGTH);
                if (DEBUG >= 3) {
                    System.out.println("DIAG endTALOS = " + endTALOS);
                    System.out.println("DIAG endPDB = " + endPDB);
                }

                //
                // Try to match the beginning of each sequence to the other
                // sequence.
                //
                int indexInTALOS = sTALOSds.indexOf(beginPDB);
                int indexInPDB = sPDBds.indexOf(beginTALOS);
                if (DEBUG >= 3) {
                    System.out.println("DIAG searching from beginning...");
                    System.out.println("DIAG indexInTALOS = " + indexInTALOS);
                    System.out.println("DIAG indexInPDB = " + indexInPDB);
                }

                if (indexInTALOS != -1 ) {
                    if (DEBUG >= 3) {
                        System.out.println("DIAG Found PDB in TALOS");
                    }

                    // Not needed for S2D, since we're just testing whether we can
                    // match the sequences.  wenger 2008-11-26.
                    // this.seqRenumber(indexInTALOS);

                    theDataSequence.truncateFront(indexInTALOS);
                    sTALOSds = theDataSequence.toString();

                    if (DEBUG >= 1) {
                        System.out.println("  --------------------------");
                        System.out.println("  The corrected TALOS:\n  " +
                                           theDataSequence.toString());
                        System.out.println("\n  The PDB:\n  " + thePDBds.toString());
                    }
                } else if (indexInPDB != -1 ) {
                    if (DEBUG >= 3) {
                        System.out.println("DIAG Found TALOS in PDB");
                    }

                    // Not needed for S2D, since we're just testing whether we can
                    // match the sequences.  wenger 2008-11-26.
                    // thePDB.seqRenumber(indexInPDB);

                    thePDBds.truncateFront(indexInPDB);
                    sPDBds = thePDBds.toString();

                    if (DEBUG >= 1) {
                        System.out.println("  --------------------------");
                        System.out.println("  The corrected PDB:\n  " +
                                           thePDBds.toString());
                        System.out.println("\n  The TALOS:\n  " +
                                           theDataSequence.toString());
                    }
                }

                //
                // Try to match the end of each sequence to the other
                // sequence.
                //
                indexInTALOS = sTALOSds.lastIndexOf(endPDB);
                indexInPDB = sPDBds.lastIndexOf(endTALOS);
                if (DEBUG >= 3) {
                    System.out.println("DIAG searching from end...");
                    System.out.println("  indexInTALOS = " + indexInTALOS);
                    System.out.println("  indexInPDB = " + indexInPDB);
                }

                if (indexInTALOS != -1) {
                    if (DEBUG >= 3) {
                        System.out.println("DIAG Found PDB in TALOS");
                    }
                    theDataSequence.truncateEnd(indexInTALOS + LENGTH);
                    sTALOSds = theDataSequence.toString();

                    if (DEBUG >= 1) {
                        System.out.println("  --------------------------");
                        System.out.println("  The corrected TALOS:\n  " +
                                           theDataSequence.toString());
                        System.out.println("\n  The PDB:\n  " + thePDBds.toString());
                    }

                } else if (indexInPDB != -1) {
                    if (DEBUG >= 3) {
                        System.out.println("DIAG Found TALOS in PDB");
                    }
                    thePDBds.truncateEnd(indexInPDB + LENGTH);
                    sPDBds = thePDBds.toString();

                    if (DEBUG >= 1) {
                        System.out.println("  --------------------------");
                        System.out.println("  The corrected PDB:\n  " +
                                           thePDBds.toString());
                        System.out.println("\n  The TALOS:\n  " +
                                           theDataSequence.toString());
                    }
                }
            }
        }

        if (errcode >= 3) {
            if (DEBUG >= 1) {
                System.out.println("  The Sequences could not be corrected.");
            }
        }

        adjustMismatchList(resMismatchList);

        return errcode;
    }

    /**
     * Adjust the list of residues we want to exclude because of mismatches.
     * As of right now, we want to exclude any residue that is immediately
     * between two others that we're excluding (re email from Gabriel
     * Cornilescu on 2004-02-03.  wenger 2004-02-04.
     * @param Set containing residues to exclude because of mismatches.
     */
    private static void adjustMismatchList(Set resMismatchList)
    {
        TreeSet tmpList = new TreeSet();
        Iterator it = resMismatchList.iterator();
        while (it.hasNext()) {
            int val = ((Integer)it.next()).intValue();
            Integer testVal = new Integer(val + 2);
            if (resMismatchList.contains(testVal)) {
                tmpList.add(new Integer(val + 1));
            }
        }
        resMismatchList.addAll(tmpList);
    }
}
