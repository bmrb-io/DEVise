//-------------------------------------------------------------------------------------
// $Id$
//
// This is a LRU implementaion of a main memory cache that stores records having the
// following format:
//
// - File object (We may choose to not have this)
// - 1 or many integers associated with that file.
// - upper limit that specifies the range of each integer associated with that file
//-------------------------------------------------------------------------------------
package xman.xvis;

import java.io.*;
import java.util.*;

public class Inquirer
{
    private TreeSet cache = null;
    
    // For fast cache lookups the following variables have been declared here instead
    // of being declared in the functions in which they are being used
    private long t1, t2; 
    private LineNumberReader in = null;
    private String str;
    private int numBins, binsPerP, index, binIndex, p, x;
    private double sum, max, val;

    // Temporary stuff for debugging purposes only
    public int ccnt;
    
    public Inquirer () {
	
	cache = new TreeSet(new Comparator() {
	    public int compare(Object o1, Object o2) {
		t1 = ((CRec)o1).timestamp;
		t2 = ((CRec)o2).timestamp;
		
		if (t1 > t2) return -1;
		else if (t1 < t2) return 1;
		else return 0;
	    }});
    }
    
    /**
     * @param string filenames separated by ":"
     * @param array partitions
     * @param numP number of partitions
     * @param uL upper limit on value for each partition. Value in each partition is
     *           such that 0 <= value <= uL
     */
    public synchronized void lookUp (String s, int [] a, int numP, int uL) 
	throws YException 
    {
	if (numP < 2 || uL < 4 || a == null || a.length != numP || s == null) 
	    throw new YException ();
	
	StringTokenizer str = new StringTokenizer (s, ":");
	
	int numFiles = str.countTokens ();
	int [] ta = new int [numP];
	
	for (int i = 0; i < numP; i++) a [i] = 0;
	
	for (int i = 0; i < numFiles; i++) {
	    File f = new File (str.nextToken ());
	    
	    lookUp (f, ta, numP, uL);
	    
	    for (int j = 0; j < numP; j++) 
		if (a [j] < ta [j]) a [j] = ta [j];
	}
    }
    
    /**
     * This method uses variables common to the class. Access to it should be 
     * SYNCHRONIZED
     */
    public synchronized void lookUp (File f, int [] a, int numP, int uL) 
	throws YException 
    {
	CRec r;
	
	if (numP < 2 || uL < 4 || a == null || a.length != numP || f == null) 
	    throw new YException ();
	
	// Obtain an iterator on cache
	Iterator i = cache.iterator ();
	
	while (i.hasNext ()) {
	    
	    r = (CRec)i.next ();
	    
	    if (f.compareTo (r.file) == 0 &&
		r.bin.length == a.length &&
		r.uL == uL) {
		
		// Match found
		
		for (x = 0; x < numP; x++) a [x] = r.bin [x];  // Copy into result
		
		r.timestamp = (new Date ()).getTime ();        // Update timestamp
		
		cache.remove (r);     // Remove entry from cache
		
		cache.add (r);	      // Re-insert entry with present timestamp
		
		return;
	    }
	}
	
	ccnt++;
	// Control reaching here => desired file not found in cache
	// Check if cache is full
	if (cache.size () >= 100) {
	    
	    // Choose a victim based on LRU policy
	    r = (CRec)cache.first ();
	    cache.remove (r);
	}
	
	// Create new cache record
	checkFile (f, a, numP, uL);
	
	r = new CRec (f, a, uL);
	
	cache.add (r);
    }
    
    /**
     * Since, this method accesses common variables of the calls to it should be 
     * SYNCHRONIZED
     * 
     * This method goes through the specified Paradyn file and logically breaks up the 
     * values in bins into "numP" partitions. For each such partition then an integer
     * between 0 and uL, both inclusive, is returned.
     * 
     * @param f Paradyn file to be scanned
     * @param a array containing value for each partition  
     * @param numP number of partitions
     * @param uL Maximum value of a partition. This means that the value a partition
     *           can get is 0 <= value <= uL
     */
    private void checkFile (File f, int [] a, int numP, int uL) throws YException
    {
	in = null;
	
	// Check arguments first
	if (numP < 2 || uL < 4 || a == null || a.length != numP || f == null) 
	    throw new YException ();
	
	try {
	    in = new LineNumberReader (new FileReader (f));
	    
	    // Metric line
	    if (in.readLine () == null) 
		throw new YException ("Could not read line specifying metric", 
				      "Inquirer.checkFile");
	    
	    // Focus line
	    if (in.readLine () == null) 
		throw new YException ("Focus line missing", "Inquirer.checkFile");
	    
	    // Get increment line
	    if ((str = in.readLine ()) == null) 
		throw new YException ("Numbins increment line missing",
				      "Inquirer.checkFile");
	    
	    StringTokenizer s = new StringTokenizer (str);
	    
	    if (s.countTokens () < 3) 
		throw new YException ("Numbins and/or increment missing",
				      "Inquirer.checkFile");
	    
	    // Get number of bins
	    numBins = (new Integer (s.nextToken ())).intValue ();
	    
	    if (numBins < 1)
		throw new YException ("Incorrect value for number of bins",
				      "Inquirer.checkFile");
	    
	    // Obtain an integral number of bins per partition. Ignore the 
	    // remaining bins
	    binsPerP = numBins / numP;
	    
	    if (binsPerP < 1) {
		
		// Insufficient number of bins. Put 0 in each partition
		// and quit
		for (index = 0; index < numP; index++) a [index] = 0;
	    }
	    else {
		
		// For each partition obtain the average of the value contained in 
		// bins
		
		// Temporary storage for sums of each partition
		double [] sums = new double [numP]; 
		max = 0;
		
		for (index = 0; index < numP; index++) {
		    
		    sums [index] = 0;
		    
		    for (p = 0; p < binsPerP; p++) {
			
			str = in.readLine ();
			if (str.equalsIgnoreCase ("NaN")) continue;
			
			val = (new Double (str)).doubleValue ();
			
			if (max < val) max = val;
			
			sums [index] += val;
		    }
		}
		
		int remBins = numBins - (numP * binsPerP);
		index--;
		
		// If there are some bins left then account for their values in the
		// last partition
		if (remBins > 0) {
		    
		    for (p = 0; p < remBins; p++) {
			
			str = in.readLine ();
			if (str.equalsIgnoreCase ("NaN")) continue;
			
			val = (new Double (str)).doubleValue ();
			
			if (max < val) max = val;
			
			sums [index] += val;
		    }
		    
		    sums [index] = Math.round (((float)sums [index]) * binsPerP /
					       ((float)(binsPerP + remBins)));
		    
		}
		
		// compute average for each partition w.r.t. max value in all bins
		for (index = 0; index < numP; index++)  
		    a [index] = 
			(int)Math.rint ((sums [index] / (binsPerP * max)) * uL);
	    }
	} catch (IOException e) { 
	    if (in != null) { 
		try { in.close (); in = null;} 
		catch (IOException ioe) {} 
	    }	    
	    throw new YException ("IOException occurred", "Inquirer.checkFile"); 
	} catch (YException e) {
	    if (in != null) { 
		try { in.close (); in = null; }
		catch (IOException ioe) {}
	    }
	    throw e;
	}
	
	try {
	    if (in != null) { in.close (); in = null; }
	} catch (IOException e) {
	    throw new YException ("IOException occurred", "Inquirer.checkFile");
	}
    }
    
    // This class corresponds to a cache table entry
    private class CRec {
	File file;
	long timestamp;
	int bin [];
	int uL;
	
	public CRec (File f, int [] a, int u) {
	    file = f;
	    bin = new int [a.length];
	    for (int i = 0; i < a.length; i++) bin [i] = a [i];
	    timestamp = (new Date ()).getTime ();
	    uL = u;
	}
    }

    //---------------------------------------------------------------------------------
    /*    public static void main (String [] args) 
    {
	Inquirer inq = new Inquirer ();
	int [] a = new int [8];
	
	String s1 = "/p/paradyn/development/zhou/data/swim/hist_0:/p/paradyn/development/zhou/data/swim/hist_1:/p/paradyn/development/zhou/data/swim/hist_3";
	try {
	    inq.lookUp (s1, a, 8, 8);
	} catch (YException e) { System.out.println ("YException ocurred"); }
	
	for (int j = 0; j < a.length; j++)
	    System.out.print (a [j] + " ");
	
	boolean flag = true;
	
	if (flag) return;
	
	String s = "/p/paradyn/development/zhou/data/draco/SUN/most";
	File dir = new File (s);
	
	File [] f = dir.listFiles ();
	
	System.out.println ("Selected directory => " + dir + " has " + f.length +
	 		    " files.");
	
	for (int i = 0; i < f.length; i++) {
	    
	    if (((f [i].getName ()).toString ()).startsWith ("hist_")) {
		
		//System.out.println ("Caching -->" + f [i]);
		
		try {
		    inq.lookUp (f [i], a, 8, 8);
		} catch (YException e) { System.out.println ("YException ocurred"); }
		
		System.out.print ("File: " + f [i] + " ----> ");
		for (int j = 0; j < a.length; j++)
		    System.out.print (a [j] + " ");
		    System.out.println (" ");
		}
	}
	
	inq.ccnt = 0;
	int fcnt = 0;
	
	a = new int [4];
	
	for (int i = 0; i < f.length; i++) {
	    
	    if (((f [i].getName ()).toString ()).startsWith ("hist_")) {
		fcnt ++;
		//System.out.println ("Caching -->" + f [i]);
		
		try {
		    inq.lookUp (f [i], a, 4, 8);
		} catch (YException e) { System.out.println ("YException ocurred"); }
		System.out.print ("File: " + f [i] + " ----> ");
		
		for (int j = 0; j < a.length; j++)
		    System.out.print (a [j] + " ");
		System.out.println (" ");
	    }
	    }
	
	System.out.println ("# of files = " + fcnt + " | # of misses = " + inq.ccnt);
	}*/
    //---------------------------------------------------------------------------------
}
	
	

