// $Id$
package xman.xvis;

import java.util.*;
import java.io.*;
import java.text.DecimalFormat;

public class PdynFile 
{
    private File self, DEVFile;
    private String metric, focus;
    private Writer out;
    private LineNumberReader in;
    private double maxTime, maxMetric;
    double increment, startTime;
    
    public PdynFile (String name) throws YException 
    {
	self = new File (name);
	if (!self.canRead () || !self.isFile ()) throw new YException ("Cannot read " + self.toString (), "PdynFile.PdynFile");
    }
    
    public File getFile () { return self; }
    
    public File getLastDEVFileWritten () { return DEVFile; }
    
    public void deleteDEVFile () { if (DEVFile != null) DEVFile.delete (); }

    public double getMaxMetric () { return maxMetric; }
    
    public double getIncrement () { return increment; }
    
    public double getStTime () { return startTime; }
    
    public double getMaxTime () { return maxTime; }

    public String getMetric () { return metric; }
    
    public String getFocus () { return focus; }
    
    public void createDEVFile (String d, boolean b) throws YException 
    {
	File dir = new File (d);
	
	try {
	    if (!dir.isDirectory ()) 
		throw new YException ("Directory " + dir.toString () + " does not exist", "PdynFile.createDEVFile");
	    
	    DEVFile = new File (dir, self.getName () + ".p2d");
	    
	    System.out.println ("PdynFile: Converting Paradyn dataset " + self +
				" --> Devise dataset " + DEVFile + "\n");
	    
	    if (!DEVFile.exists ()) DEVFile.createNewFile ();
	    
	    if (b) DEVFile.deleteOnExit ();
	    
	    writeDEVFile (DEVFile);
	} 
	catch (IOException e) { throw new YException ("IOException while creating " + DEVFile.toString (), 
						      "PdynFile.createDEVFile"); }
	catch (YException e) { closeFiles (); throw e; }
    }

    private void writeDEVFile (File f) throws YException 
    {
	try {
	    // Open output file
	    out = new BufferedWriter(new FileWriter (f));
	    
	    // Open input file
	    in = new LineNumberReader (new FileReader (self));
	    
	    // Get metric line
	    String s;
	    
	    if ((s = in.readLine ()) == null) 
		throw new YException ("Could not read line specifying metric", "PdynFile.writeDEVFile");
	    StringTokenizer st = new StringTokenizer (s);
	    if (st.countTokens () != 2) 
		throw new YException ("histogram metric is not given", "PdynFile.writeDEVFile");
	    if (!(st.nextToken ()).equalsIgnoreCase ("histogram")) 
		throw new YException ("histogram missing", "PdynFile.writeDEVFile");
	    metric = st.nextToken ();
	    
	    // Get focus line
	    if ((focus = in.readLine ()) == null) 
		throw new YException ("Focus line missing", "PdynFile.writeDEVFile");
	    
	    // Get increment line
	    if ((s = in.readLine ()) == null) 
		throw new YException ("Numbins increment line missing", "PdynFile.writeDEVFile");
	    st = new StringTokenizer (s);
	    if (st.countTokens () < 3) 
		throw new YException ("Numbins and/or increment missing", "PdynFile.writeDEVFile");
	    st.nextToken ();
	    
	    increment = (new Double (st.nextToken ())).doubleValue ();
	    startTime = (new Double (st.nextToken ())).doubleValue ();
	    
	    DecimalFormat df = new DecimalFormat("######.####");
	    
	    int linesRead = 0; double met;
	    
	    maxTime = maxMetric = 0;
	    
	    // Read each line containing a data value
	    double time;
	    
	    for (time = startTime + (increment / 2.0); 
		 (s = in.readLine ()) != null; time += increment) {
		linesRead++;
		
		if (s.equalsIgnoreCase ("NaN")) continue;
		
		out.write (df.format (time) + " ");
		
		out.write (s + "\n");
		
		met = (new Double (s)).doubleValue ();
		
		if (met > maxMetric) maxMetric = met;
	    }
	    
	    maxTime = time - increment;
	    
	    //if (linesRead == 0) throw new YException ();
	
	}catch (IOException e) { throw new YException ("IOException occurred", "PdynFile.writeDEVFile"); }
	
	closeFiles ();
    }    
    
    private void closeFiles () { 
	try {
	    if (out != null) { out.close (); out = null; }
	    if (in != null) { in.close (); in = null; }
	}catch (IOException e) {}
    }
}
