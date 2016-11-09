// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Utility class.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseUtils.java

package JavaScreen;

import  java.io.*;
import  java.net.*;
import  java.util.Date;
import  java.util.Vector;
import  java.util.regex.Pattern;
import  java.util.regex.Matcher;

public class DEViseUtils
{
    private static class VisTypeName {
        public Pattern _p;
        public String _type;

        public VisTypeName(String regexp, String type) {
            _p = Pattern.compile(regexp);
            _type = type;
        }
    }

    private static Vector _visTypes = null;

    public static void printAllThreads(String msg)
    {
        int count = Thread.activeCount();
        System.out.println(count + " Threads (" + msg + "):");
        Thread[] threads = new Thread[count];
        int count2 = Thread.enumerate(threads);
        for (int index = 0; index < count2; index++) {
            System.out.println("  " + threads[index]);
        }
    }

    // Convert a session name (from Peptide-CGI) to a visualization
    // type.
    public static String getVisType(String sessionName)
    {
        if (_visTypes == null) {
            initializeVisTypes();
        }

        for (int index = 0; index <  _visTypes.size(); index++) {
            VisTypeName vtn = (VisTypeName)_visTypes.elementAt(index);
            Matcher m = vtn._p.matcher(sessionName);
            if (m.matches()) {
                String visType = vtn._type;
                if (m.groupCount() == 1) {
                    visType += " " + m.group(1);
                } else if (m.groupCount() == 2) {
                    visType = visType + " " + m.group(1) + "-" + m.group(2);
                } else if (m.groupCount() == 4) {
                    visType = m.group(1) + "/" + m.group(2) + visType +
                              m.group(3) + "/" + m.group(4);
                }
                return visType;
            }
        }

        return "";
    }

    private static void initializeVisTypes()
    {
        _visTypes = new Vector();

        // Note: order is significant here, because we will stop checking
        // as soon as we get a match.  So put the most "deterministic"
        // strings first.
        _visTypes.addElement(
            new VisTypeName("dna_histogram\\.ds", "DNA histogram"));
        _visTypes.addElement(
            new VisTypeName("rna_histogram\\.ds", "RNA histogram"));
        _visTypes.addElement(
            new VisTypeName("histogram2\\.ds", "dual-protein histograms"));
        _visTypes.addElement(
            new VisTypeName("histogram\\.ds", "protein histogram"));
        _visTypes.addElement(
            new VisTypeName(".*ac(\\d+)\\.ds", "atomic coordinates"));
        _visTypes.addElement(
            new VisTypeName(".*rrdistr(\\d+)\\.ds",
                            "distance restraints from remediated restraints"));
        _visTypes.addElement(
            new VisTypeName(".*rrtar(\\d+)\\.ds",
                            "torsion angle restraints from remediated restraints"));
        _visTypes.addElement(
            new VisTypeName(".*distr(\\d+)\\.ds",
                            "distance restraints from restraints grid"));
        _visTypes.addElement(
            new VisTypeName(".*tar(\\d+)\\.ds",
                            "torsion angle restraints from restraints grid"));
        _visTypes.addElement(
            new VisTypeName(".*csr1(\\d+)\\.ds", "chem shift ref histogram"));
        _visTypes.addElement(
            new VisTypeName(".*csr2(\\d+)\\.ds", "chem shift ref diffs"));
        _visTypes.addElement(
            new VisTypeName(".*csr3(\\d+)\\.ds", "observed vs. calculated CS"));
        _visTypes.addElement(
            new VisTypeName(".*lacs1\\.ds", "LACS (CA vs. CA-CB)"));
        _visTypes.addElement(
            new VisTypeName(".*lacs2\\.ds", "LACS (CB vs. CA-CB)"));
        _visTypes.addElement(
            new VisTypeName(".*lacs3\\.ds", "LACS (HA vs. CA-CB)"));
        _visTypes.addElement(
            new VisTypeName(".*lacs4\\.ds", "LACS (CO vs. CA-CB)"));
        _visTypes.addElement(
            new VisTypeName(".*sd(\\d+)\\.ds", "SPARTA chem shift deltas"));
        _visTypes.addElement(
            new VisTypeName(".*as(\\d+)\\.ds",
                            "chem shift dist by AA/nucleotide"));
        _visTypes.addElement(
            new VisTypeName("(.*)\\+(.*)hn(\\d+)\\+(\\d+)\\.ds",
                            " simulated 1H-15N spectrum "));
        _visTypes.addElement(
            new VisTypeName("(.*)\\+(.*)hc(\\d+)\\+(\\d+)\\.ds",
                            " simulated 1H-13C spectrum "));
        _visTypes.addElement(new VisTypeName(".*s2p(\\d+)-(\\d)\\.ds",
                                             "S2 predicted vs. experimental"));
        _visTypes.addElement(
            new VisTypeName(".*hn(\\d+)\\.ds", "simulated 1H-15N spectrum"));
        _visTypes.addElement(
            new VisTypeName(".*hc(\\d+)\\.ds", "simulated 1H-13C spectrum"));
        _visTypes.addElement(
            new VisTypeName(".*am(\\d+)\\.ds",
                            "assigned chem shift ambiguity codes"));
        _visTypes.addElement(
            new VisTypeName(".*ps(\\d+)\\.ds", "assignment figure of merit"));
        _visTypes.addElement(
            new VisTypeName(".*t1(\\d+)\\.ds", "T1 relaxation"));
        _visTypes.addElement(
            new VisTypeName(".*t2(\\d+)\\.ds", "T2 relaxation"));
        _visTypes.addElement(
            new VisTypeName(".*g(\\d+)\\.ds", "coupling constants"));
        _visTypes.addElement(
            new VisTypeName(".*n(\\d+)\\.ds", "heteronuclear NOE"));
        _visTypes.addElement(
            new VisTypeName(".*o(\\d+)\\.ds", "S2 order params"));
        _visTypes.addElement(
            new VisTypeName(".*d(\\d+)\\.ds", "chemical shift deltas"));
        _visTypes.addElement(
            new VisTypeName(".*c(\\d+)\\.ds", "chemical shift indexes"));
        _visTypes.addElement(
            new VisTypeName(".*p(\\d+)\\.ds", "percent assigned"));
    }
}
