//------------------------------------------------------------------------------------
// $Id$
//------------------------------------------------------------------------------------
package xman.xvis;

import java.io.*;
import java.util.*;
import java.text.DecimalFormat;

public class MFVisual
{
    private boolean del = false;
    private DeviseManager devServer = null;
    private int numPh = 0, mfId = -1;
    
    // unique string associated with this metric / focus pair. Wei's tool passes
    // this. This could be replaced by mfId in future
    private String mfDescStr = null;
    
    private PhaseInfo [] phaseInfos = null;
    private File mfDir = null;
    private static String phaseSep = ":";
    private File phaseTdataSchemaFile = null, mainVTdataFile = null;
    private String phaseTdataSchemaType = null;
    private String mapClName = null, mainVName = null, linkName = null;
    private double maxM, maxT;
    private String mainSchemaType = null, mainVTdataName = null, mainVMapName = null;
    private File mainSchemaFile = null;
    private String  dataFiles = null;
    
    // Color map
    private static int [] colorMap = { 23, 7, 31, 35, 3, 19, 5,
				       11, 32, 8, 25, 6, 21, 4, 18 };
    
    // Info view stuff
    private File infoVTdataFile = null, infoSchemaFile = null;
    private String infoVName = null, infoSchemaType = null, infoVMapName = null;
    private String infoVTdataName = null;
    
    // Legend Box view stuff
    private File lgndBVTdataFile = null, lgndBVSchemaFile = null;
    private String lgndBVName = null, lgndBVSchemaType = null, lgndBVMapName = null;
    private String lgndBVTdataName = null;
    
    // Legend Text view stuff
    private File lgndTVTdataFile = null, lgndTVSchemaFile = null;
    private String lgndTVName = null, lgndTVSchemaType = null, lgndTVMapName = null;
    private String lgndTVTdataName = null;
    
    /**
     * Constructor
     *
     * @param id unique id associated with this metric/focus
     * @param f directory for scratch work
     * @param s devise server to be used
     * @param b delete on exit or not?
     */
    public MFVisual (int id, File f, DeviseManager s, boolean b) throws YException
    {
	mfId = id;
	mfDir = f;
	devServer = s;
	del = b;
    }
    
    /**
     * Constructor
     *
     * @param ds some descriptive string associated with this metric/focus
     * @param id unique id associated with this metric/focus
     * @param f directory for scratch work
     * @param s devise server to be used
     * @param b delete on exit or not?
     */
    public MFVisual (String ds, int id, File f, DeviseManager s, boolean b) 
	throws YException
    {
	this (id, f, s, b);
	mfDescStr = ds;
    }
    
    /**
     * @param f string containing name(s) of file(s) that have performance data for one
     *          metric/focus
     */
    public synchronized void show (String f) throws YException
    { 
	//
	// Get number of phases
	//
	dataFiles = f;
	StringTokenizer st = new StringTokenizer (f, phaseSep);
	numPh = st.countTokens ();
	if (numPh < 1) throw new YException ("Could not parse input string");
	
	String [] s = new String [numPh];
	for (int i = 0; i < numPh; i++) s [i] = st.nextToken ();
	
	preparePhases (s);
	
	generateDevCode ();
    }
    
    /**
     * @param f string containing name(s) of file(s) that have performance data for one
     *          metric/focus
     * @param str string to be prepended to the title
     */
    public synchronized void show (String f, String str) throws YException
    { 
	mfDescStr = str;
	show (f);
    }
    
    private void preparePhases (String [] s) throws YException
    {
	phaseInfos = new PhaseInfo [s.length];
	
	for (int i = 0; i < phaseInfos.length; i++) {
	    
	    // Create new phase
	    phaseInfos [i] = new PhaseInfo ();
	    phaseInfos [i].fileName = s [i];
	    phaseInfos [i].id = i;            
	    
	    // For now the color mapping is simple
	    phaseInfos [i].color = colorMap [i %  colorMap.length];
	    
	    phaseInfos [i].vName = "MF_" + mfId + "_Phase_" + phaseInfos [i].id +
		"_View";
	    phaseInfos [i].mapName = "MF_" + mfId + "_Phase_" + phaseInfos [i].id +
		"_Map";
	    phaseInfos [i].tDataName = "MF_" + mfId + "_Phase_" + phaseInfos [i].id +
		"_Tdata";
	    
	    phaseInfos [i].VSVName = "MF_" + mfId + "_Phase_" + phaseInfos [i].id +
		"_VSView";
	    phaseInfos [i].VSMapName = "MF_" + mfId + "_Phase_" + phaseInfos [i].id +
		"_VSMap";
	    
	    phaseInfos [i].pFile = new PdynFile (phaseInfos [i].fileName);
	    phaseInfos [i].pFile.createDEVFile (mfDir.toString (), del);
	}
    }
    
    /**
     * This method creates a standard session
     */
    private void generateDevCode () throws YException 
    {
	int i; 
	double d;
	DecimalFormat df = new DecimalFormat("######.####");
	 
	// Create tdata schema file for phases
	createPhaseTdataSchema ();
	
	// Create interpreted mapping class
	mapClName = "MF_" + mfId + "_MapCl";
	if (!devServer.createMappingClass (mapClName)) 
	    throw new YException ("createMappingClass");
	
	// Get maximum time and metric
	maxT = 0; maxM = 0;
	for (i = 0; i < phaseInfos.length; i++) {
	    d = phaseInfos [i].pFile.getMaxTime (); if (maxT < d) maxT = d;
	    d = phaseInfos [i].pFile.getMaxMetric (); if (maxM < d) maxM = d;
	}
	
	// Need to do this or else devised would crash
	if (maxM <= 0) maxM = 0.1;
	
	// Make catalog entry
	try {	    
	    for (i = 0; i < phaseInfos.length; i++) 
		if (!devServer.dteInsertCatalogEntry (phaseInfos [i].tDataName, 
						      (phaseInfos [i].pFile.getLastDEVFileWritten ()).getName (),
						      phaseTdataSchemaType, 
						      phaseTdataSchemaFile.getCanonicalPath (),  
						      mfDir.getCanonicalPath ()))
		    throw new YException ("Error Detected");
	}
	catch (IOException e) { throw new YException (); }
	
	// Create views
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.createScatterView (phaseInfos [i].vName, 
					      0,
					      maxT, 
					      0, 
					      maxM, 
					      30, 
					      0))
		throw new YException ("Error detected");
	
	// Create mapping instances (GData) 
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.createMapping (mapClName, 
					  phaseInfos [i].tDataName, 
					  phaseInfos [i].mapName,
					  "Time", 
					  "Metric", 
					  colorMap [i % colorMap.length], 
					  2, 
					  phaseInfos [i].pFile.getIncrement ()))
		throw new YException ("Error detected");
	
	
	// Create view slider views
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.createScatterView (phaseInfos [i].VSVName, 
					      0,
					      maxT, 
					      0, 
					      maxM, 
					      30, 
					      0))
		throw new YException ("Error detected");
	
	// Create mapping instances for View Slider views
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.createMapping (mapClName, 
					  phaseInfos [i].tDataName, 
					  phaseInfos [i].VSMapName,
					  "Time", 
					  "Metric", 
					  colorMap [i % colorMap.length], 
					  2, 
					  phaseInfos [i].pFile.getIncrement ()))
		throw new YException ("Error detected");
	
	// Set axis
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.setViewXAxisDisplay (phaseInfos [i].vName, true) ||
		!devServer.setViewYAxisDisplay (phaseInfos [i].vName, true))
		throw new YException ("Error detected");
	
	// Set Font
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.setViewFont (phaseInfos [i].vName, "title", 2, 12,1,0) ||
		!devServer.setViewFont (phaseInfos [i].vName, "x axis", 0, 10, 1, 0) ||
		!devServer.setViewFont (phaseInfos [i].vName, "y axis", 0, 10, 1, 0))
		throw new YException ("Error detected");
	
	// We may like to set these values from SessnMaker since, at this level
	// we have no knowledge about the maximum metric in all metric focus pairs
	// Set view filter
	for (i = 0; i < phaseInfos.length; i++) {
	    if (!devServer.setViewFilter (phaseInfos [i].vName, 0, 0, maxT, maxM))
		throw new YException ("Error detected");
	    
	    if (!devServer.viewSetHome (phaseInfos [i].vName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    0,
				    0,
				    maxT, maxM))
	    throw new YException ("Error detected");
	}
	
	// Insert mapping into views
	for (i = 0; i < phaseInfos.length; i++)
	    if (!devServer.insertMapping (phaseInfos [i].vName,
					  phaseInfos [i].mapName) ||
		!devServer.insertMapping (phaseInfos [i].VSVName,
					  phaseInfos [i].VSMapName))
		throw new YException ("Error detected");
	
	// Put statistics
	//	for (i = 0; i < phaseInfos.length; i++)
	//  if (!devServer.setViewStatistics (phaseInfos [i].vName, 1, 0, 0, 0))
	//throw new YException ("Error detected");
	
	// Insert Title
	for (i = 0; i < phaseInfos.length; i++) {
	    String title = phaseInfos [i].pFile.getMetric () + " Vs time";
	    
	    if (mfDescStr != null) title = mfDescStr + ": " + title;
	    
	    if (!devServer.setLabel (phaseInfos [i].vName, title)) 
		throw new YException ("Error detected");
	}
	
	// Create link
	linkName = "MF_" + mfId + "_Time_Link";
	if (!devServer.createLink (linkName)) throw new YException ("Error detected");
	
	// Link phases
	for (i = 0; i < phaseInfos.length; i++) {
	    if (!devServer.insertLink (linkName, phaseInfos [i].vName)) 
		throw new YException ("Error detected");
	}
	
	//----------------------------------------------------------------------------
	// Sort phases acording to increment value (granularity)
	//----------------------------------------------------------------------------
	
	// Create vector to store phaseInfos array
	Vector v = new Vector (phaseInfos.length);
	for (i = 0; i < phaseInfos.length; i++) v.addElement (phaseInfos [i]);
	
	// Sort views in descending order on increment
        Collections.sort (v, new Comparator() {
            public int compare(Object o1, Object o2) {
		double inc1 = ((PhaseInfo)o1).pFile.getIncrement ();
		double inc2 = ((PhaseInfo)o2).pFile.getIncrement ();
		
		if (inc1 > inc2) return -1;
		else if (inc1 < inc2) return 1;
		else return 0;
            }
        });
	
	//----------------------------------------------------------------------------
	// Create view containing information on this metric
	//----------------------------------------------------------------------------
	
	int numL = 3;
	double y, tHt = 4;
	
	String [] tu = new String [3 + v.size ()];
	
	y = tHt * (numL - 1);
	
	tu [0] = "0*" + y + "*Metric: " + phaseInfos [0].pFile.getMetric () + "*";
	y -= tHt;
	tu [1] = "0*" + y + "*Focus: " + phaseInfos [0].pFile.getFocus () + "*";
	y -= tHt;
	tu [2] = "0*" + y + "*Number of phases: " + v.size () + "*";
	
	for (i = 0; i < v.size (); i++) {
	    y -= tHt;
	    
	    PhaseInfo p = (PhaseInfo)v.elementAt (i);
	    tu [3 + i] = "0*" + y + "*Phase: " + p.id + 
		", beg: " + df.format (p.pFile.getStTime ()) +
		", end: " + df.format (p.pFile.getMaxTime ()) +
		", inc: " + df.format (p.pFile.getIncrement ()) + "*";
	}	
	
	infoVTdataFile = new File (mfDir, "infoVTdata"); 
	writeFile (infoVTdataFile.toString (), tu, true);
	
	// Create info view
	infoVName = "MF_" + mfId + "_Info_View";
	if (!devServer.createScatterView (infoVName, 
					  0, 
					  10, 
					  0, 
					  tHt * numL,
					  0,
					  24))
	    throw new YException ("Could not create main view");
	
	if (!devServer.viewSetHome (infoVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    0,
				    0,
				    10, tHt * numL))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (infoVName, 1, 1, 1, 0))
	    throw new YException ("Error detected");
	
	// Make schema file for TData of info view
	infoSchemaType = "MF_" + mfId + "_Info_Type";
	String u [] = { "type " + infoSchemaType + " ascii", "comment #", 
			"whitespace '*'",
			"attr X float",
			"attr Y float",
			"attr Text string 256" };
	
	infoSchemaFile = new File (mfDir, "infoVSchema");
	writeFile (infoSchemaFile.toString (), u, true);
	
	// Make catalog entry for info view tdata
	infoVTdataName = "MF_" + mfId + "_Info_View_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (infoVTdataName, 
						  infoVTdataFile.getName (),
						  infoSchemaType,
						  infoSchemaFile.getCanonicalPath (),
						  mfDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Make mapping for info view
	infoVMapName = "MF_" + mfId + "_Info_View_Map";
	if (!devServer.createTextMapping (mapClName, 
					  infoVTdataName,
					  infoVMapName,
					  "$X",
					  "$Y",
					  0,                // Color
					  360,              // Orientation
					  "$Text",
					  tHt - 1,          // Height
					  -2,
					  0))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (infoVName, infoVMapName))
	    throw new YException ("Error detected");
	
	//----------------------------------------------------------------------------
	// Create view with legend
	//----------------------------------------------------------------------------
	
	makeLgndView ();

	//----------------------------------------------------------------------------
	// Now create a view that contains the piled views
	//----------------------------------------------------------------------------
	
	
	// write tdata tuples to tdata file
	String l [] = new String [3 + v.size ()];
	
	double x, mfHt, mfWt, inHt, inWt, lgHt, lgWt, vWt, vHt;
	
	mfHt = 1;
	mfWt = 1;
	inHt = 0.2;
	inWt = 1;
	lgHt = 0.2;
	lgWt = 1;
	
	vHt = inHt + mfHt + lgHt;
	vWt = mfWt;
	
	// Make tuple for info view
	x = inWt / 2; y = (inHt / 2);
	l [0] = new String (x + " " + y + " " + infoVName + 
			    " MF_" + mfId + "_Pile1 " + 
			    inWt + " " + inHt);
	
	// Make tuple for legend box view
	x = lgWt / 2; y = inHt + (lgHt / 2);
	l [1] = new String (x + " " + y + " " + lgndBVName + 
			    " MF_" + mfId + "_Pile3 " + 
			    lgWt + " " + lgHt);
	
	// Make tuple for legend text view
	x = lgWt / 2; y = inHt + (lgHt / 2);
	l [2] = new String (x + " " + y + " " + lgndTVName + 
			    " MF_" + mfId + "_Pile3 " + 
			    lgWt + " " + lgHt);
	
	// Make tuples for each phase view
	x = mfWt / 2; y = mfHt / 2 + lgHt + inHt;
	for (i = 0; i < v.size (); i++) {
	    PhaseInfo p = (PhaseInfo)v.elementAt (i);
	    l [i + 3] = new String (x + " " + y + " " + p.vName + " " + 
				    "MF_" + mfId + "_Pile2 "
				    + mfWt + " " + mfHt);
	}
	
	mainVTdataFile = new File (mfDir, "mainVTdata"); 
	writeFile (mainVTdataFile.toString (), l, true);
	
	// Create main view
	mainVName = "MF_" + mfId + "_Main_View";
	if (!devServer.createScatterView (mainVName, 
					  0, 
					  vWt, 
					  0, 
					  vHt))
	    throw new YException ("Could not create main view");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (mainVName, 1, 1, 1, 1))
	    throw new YException ("Error detected");

	// Make schema file for TData of main view
	mainSchemaType = "MF_" + mfId + "_Schema_Type";
	String m [] = { "type " + mainSchemaType + " ascii", "comment #", 
			"whitespace ' '", 
			"attr X float",
			"attr Y float", 
			"attr ViewName string 30",
			"attr PileName string 30",
			"attr W float", 
			"attr H float"};
	mainSchemaFile = new File (mfDir, "mainVSchema");
	writeFile (mainSchemaFile.toString (), m, true);
	
	// Make catalog entry for main view tdata
	mainVTdataName = "MF_" + mfId + "_Main_View_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (mainVTdataName, 
						  mainVTdataFile.getName (),
						  mainSchemaType,
						  mainSchemaFile.getCanonicalPath (),
						  mfDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Make mapping for main view
	mainVMapName = "MF_" + mfId + "_Main_View_Map";

	if (!devServer.createPiledViewMapping (mapClName, 
					       mainVTdataName,
					       mainVMapName,
					       "$X",
					       "$Y",
					       "$ViewName",
					       "$W",
					       "$H",
					       "$PileName",
					       -1))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (mainVName, mainVMapName))
	    throw new YException ("Error detected");
    }	
    
    private void createPhaseTdataSchema () throws YException 
    {
	phaseTdataSchemaType = "MF_" + mfId + "_Schema";
	String l [] = { "type " + phaseTdataSchemaType + " ascii", "comment #",
			"whitespace ' '", "attr Time float", "attr Metric float"};
	
	phaseTdataSchemaFile = new File (mfDir, "phaseTdataSchema"); 
	writeFile (phaseTdataSchemaFile.toString (), l, del);
    }
    
    
    private void writeFile (String name, String [] lines, boolean delete)
	throws YException
    {
	File f = new File (name);
	
	try {
	    f.createNewFile ();
	    
	    if (delete) f.deleteOnExit ();
	    
	    Writer out = new BufferedWriter(new FileWriter (f));
	    
	    for (int i = 0; i < lines.length; out.write (lines [i++] + "\n"));
	    
	    out.close ();
	}catch (IOException e) {
	    throw new YException ("IOException while writing " + f.toString (), 
				  "SessnMaker.writeFile");
	}
    } 
    
    public void cleanUp () 
    {
	// Remove all devise objects created
	try {
	    if (devServer != null) {
		
		// Only views that are directly in a window are to be
		// removed
		//		devServer.removeView (mainVName);
		
		if (phaseInfos != null)
		    for (int i = 0; i < phaseInfos.length; i++) {
			devServer.destroyView (phaseInfos [i].VSMapName, 
					       phaseInfos [i].VSVName);
			
			devServer.destroyView (phaseInfos [i].mapName, 
					       phaseInfos [i].vName,
					       phaseInfos [i].tDataName);
		    }
		
		devServer.destroy (linkName);
		
		devServer.destroyView (infoVMapName, infoVName, infoVTdataName);
		devServer.destroyView (lgndBVMapName, lgndBVName, lgndBVTdataName);
		devServer.destroyView (lgndTVMapName, lgndTVName, lgndTVTdataName);
		devServer.destroyView (mainVMapName, mainVName, mainVTdataName);
		
		// 05/13/2000: Does not get destroyed
		//devServer.destroyMapCl (mapClName);
	    }
	} catch (YException e) {e.printStackTrace ();}
	
	// Remove all files created
	if (phaseInfos != null)
	    for (int i = 0; i < phaseInfos.length; i++)
		phaseInfos [i].pFile.deleteDEVFile ();
	
	lgndBVTdataFile.delete ();
	lgndBVSchemaFile.delete ();
	lgndTVTdataFile.delete ();
	lgndTVSchemaFile.delete ();
	
	infoSchemaFile.delete ();
	infoVTdataFile.delete (); 
	
	phaseTdataSchemaFile.delete ();
	
	mainSchemaFile.delete ();
	mainVTdataFile.delete ();
    }
    
    /**
     * Create view containing legend
     * In this method we will make a legend for a metric Vs Time histogram. Each line
     * of the legend consists of 2 entities.
     * 1. box of a certain color
     * 2. A textual what that color stands for in the histogram.
     * 
     * Since, these 2 entities are of different types we cannot (at least without 
     * undergoing much pain) draw them on the same view. Hence, we will draw the
     * colored boxes on one view and the descriptive text on another view and then
     * pile both of them one on top of the other.
     */
    private void makeLgndView () throws YException
    {
	double bHt = 0.8;               // Height of box
	double bWt = 1;                 // Width of box
	double tHt = 1;                 // Height of text
	double lHt = 1.2;               // Height of line
	int numL = 3;                   // # of legend lines visible in legend view
	double x, y;
	double vWt;                     // Width of both views
	double vHt;                     // Height of both views
	
	vWt = bWt * 12;
	vHt = lHt * numL;
	
	//----------------------------------------------------------------------------
	// Make view containing boxes
	//----------------------------------------------------------------------------
	
	// tData
	String [] l = new String [numPh];
	
	y = (lHt / 2) + (lHt * (numL - 1));
	
	for (int i = 0; i < numPh; i++) {
	    l [i] = (bWt / 2) + " " + y + " " + phaseInfos [i].color + " " + bWt +
		" " + bHt;
	    y -= lHt;
	}
	
	lgndBVTdataFile = new File (mfDir, "lgndBVTdata"); 
	
	writeFile (lgndBVTdataFile.toString (), l, true);
	
	// view
	lgndBVName = "MF_" + mfId + "_Lgnd_B_View";
	if (!devServer.createScatterView (lgndBVName,
					  0, 
					  vWt,
					  0, 
					  vHt,
					  36,
					  0))
	    throw new YException ("Could not create main view");
	
	if (!devServer.viewSetHome (lgndBVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    0,
				    0,
				    vWt, vHt))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (lgndBVName, 1, 1, 1, 0))
	    throw new YException ("Error detected");
	
	// schema file for TData of box
	lgndBVSchemaType = "MF_" + mfId + "_Lgnd_B_V_Schema";
	String m [] = { "type " + lgndBVSchemaType + " ascii",
			"whitespace ' '",
			"attr X float",
			"attr Y float",
			"attr Color int",
			"attr W float",
			"attr H float" };
	
	lgndBVSchemaFile = new File (mfDir, "lgndBVSchema");
	writeFile (lgndBVSchemaFile.toString (), m, true);
	
	// Make catalog entry
	lgndBVTdataName = "MF_" + mfId + "_Lgnd_B_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (lgndBVTdataName, 
						  lgndBVTdataFile.getName (),
						  lgndBVSchemaType,
						  lgndBVSchemaFile.getCanonicalPath (),
						  mfDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Make mapping
	lgndBVMapName = "MF_" + mfId + "_Lgnd_B_V_Map";
	if (!devServer.createRectMapping (mapClName, 
					  lgndBVTdataName,
					  lgndBVMapName,
					  "$X",
					  "$Y",
					  "$Color",
					  "$W",
					  "$H"))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (lgndBVName, lgndBVMapName))
	    throw new YException ("Error detected");
	
	if (!devServer.setViewFilter (lgndBVName, 0, 0, vWt, vHt))
	    throw new YException ("Error detected");
	
	//----------------------------------------------------------------------------
	// Make view containing text
	//----------------------------------------------------------------------------
	
	// tData
	String [] o = new String [numPh];
	
	// Reference point for text can be south-west corner
	y = (lHt / 2) + (lHt * (numL - 1)) - (tHt / 2);
	
	for (int i = 0; i < numPh; i++) {
	    o [i] = (bWt + bWt/4) + "*" + y + "*" + "Phase " + i + "*" + tHt + "*";
	    y -= lHt;
	}
	
	lgndTVTdataFile = new File (mfDir, "lgndTVTdata"); 
	
	writeFile (lgndTVTdataFile.toString (), o, true);
	
	// view
	lgndTVName = "MF_" + mfId + "_Lgnd_T_View";
	if (!devServer.createScatterView (lgndTVName,
					  0, 
					  vWt,
					  0, 
					  vHt,
					  30,
					  0))
	    throw new YException ("Could not create main view");
	
	if (!devServer.viewSetHome (lgndTVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    0,
				    0,
				    vWt, vHt))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (lgndTVName, 1, 1, 1, 0))
	    throw new YException ("Error detected");
	
	// schema file for TData
	lgndTVSchemaType = "MF_" + mfId + "_Lgnd_T_V_Schema";
	String n [] = { "type " + lgndTVSchemaType + " ascii",
			"whitespace '*'",
			"attr X float",
			"attr Y float",
			"attr Text string 30",
			"attr H float" };
	
	lgndTVSchemaFile = new File (mfDir, "lgndTVSchema");
	writeFile (lgndTVSchemaFile.toString (), n, true);
	
	// Make catalog entry
	lgndTVTdataName = "MF_" + mfId + "_Lgnd_T_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (lgndTVTdataName, 
						  lgndTVTdataFile.getName (),
						  lgndTVSchemaType,
						  lgndTVSchemaFile.getCanonicalPath (),
						  mfDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Make mapping
	lgndTVMapName = "MF_" + mfId + "_Lgnd_T_V_Map";
	if (!devServer.createTextMapping (mapClName, 
					  lgndTVTdataName,
					  lgndTVMapName,
					  "$X",
					  "$Y",
					  30,
					  360,
					  "$Text",
					  "$H",
					  -2,
					  1))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (lgndTVName, lgndTVMapName))
	    throw new YException ("Error detected");
	
	if (!devServer.setViewFilter (lgndTVName, 0, 0, vWt, vHt))
	    throw new YException ("Error detected");
    }
    
    public String getVName () { return mainVName; }
    public String getPhVName () { return phaseInfos [0].vName; }
    public double getMaxT () { return maxT; }
    public double getMaxM () { return maxM; }
    
    public PhaseInfo [] getPhaseInfos () { return phaseInfos; }
}

