//------------------------------------------------------------------------------------
// $Id$
//------------------------------------------------------------------------------------
package xman.xvis;

import java.io.*;
import java.util.*;
import java.text.DecimalFormat;

public class SessnMaker
{
    private DeviseManager devServer;    // devise manager object
    private File   mainDir;            // Working directory
    private boolean del;               // delete file on exit?
    private static String mfSep = ";"; // separater for metric/focus groups
    private static String mfDescStrSep = "@"; // separator for label
    
    private String mainMapClName;      // devise name of main view's mapping class
    private String mainVSchemaType;    // devise name of main view's schema's type
    private String mainVTdataName;     // devise name of main view's tdata
    private String mainVMapName;       // devise name of main view's mapping name
    private File   mainVTdataFile;     // File object of main view's tdata 
    private String mainVName;          // devise name of main view
    private File   mainVSchemaFile;    // File object of main view's schema file
    
    private File[] mfVisualDirs ;     // working directory names for MFVisual objects
    private MFVisual [] mfVisuals;     // MFVisual objects
    private int    numMF;              // number of metric/focus pairs
    
    private String iconVName;          // devise name of icon's view
    private String iconPath;           // path of icon's gif/jpeg file
    private File   iconVTdataFile;     // File object of icon's view's tdata file
    private String iconVSchemaType;    // devise name of icon's view's schema's type
    private String iconVTdataName;     // devise name of icon's view's tdata
    private File   iconVSchemaFile;    // File object of icon's view's schema file
    private String iconVMName;         // devise name of icon's view's mapping
    
    private String linkName;           // devise name of link
    
    private String timeCursorName;     // devise name of time cursor
    private String timeCursorVName;    // devise name of time cursor's view
    private String timeCursorVSchemaType;// devise name of time cursor's view's schema's type
    private File timeCursorVTdataFile; // File object of time cursor's view's tdata
    private String timeCursorVTdataName;// devise name of time cursor's view's tdata 
    private String timeCursorVMName;   // devise name of time cursor's view's mapping
    private File timeCursorVSchemaFile;// File object of time cursor's view's schema file
    
    // specifies dimensions of each child view of MFContainerV
    private double mfConVChildHt;
    private double mfConVChildWt;
    
    // specifies dimensions of MFContainerV
    private double mfConVHt;
    private double mfConVWt;
    
    private String mfConVSchemaType;   // devise name of MFContainer view's schema type
    private File   mfConVSchemaFile;   // File object of mfCon's view's schema file
    private File   mfConVTdataFile;    // File object of mfCon's view's tdata
    private String mfConVName;         // devise name of mfCon's view
    private String mfConVTdataName;    // devise name of mfCon's view's tdata
    private String mfConVMName;        // devise name of mfCon's view's mapping
    // Vector containing info on each child view of MF Container
    private Vector mfConVInfoList;
    
    private String mfConVCopyVSchemaType;// devise name of mfConVCopyV's schema type
    private File mfConVCopyVTdataFile; // File object of mfConVCopyV's tdata file
    private String mfConVCopyVTdataName; // devise name of mfConVCopyV's tdata 
    private String mfConVCopyVMName;   // devise name of mfConVCopyV mapping
    private String mfConVCopyVName;    // devise name of mfConVCopyV 
    private File mfConVCopyVSchemaFile;// File object of mfConVCopyV's schema file
    
    private String mfConVYCursorName;  // devise name of mfConVYCursor
    private String mfConVYCursorVName; // devise name of mfConVYCursor view
    private String mfConVYCursorMName; // devise name of mfConVYCursor view's mapping
    
    /**
     * Constructor
     *
     * @param f is working directory
     * @param s is DeviseManger object
     * @param b is that specifies whether files created should be destroyed
     *          upon the termination of this process or not.
     * @exception YException
     */
    public SessnMaker (File f, DeviseManager s, boolean b) throws YException
    {
	mainDir = f;
	devServer = s;
	del = b;
	iconPath = "images/xman.jpg";
    }
    
    /**
     * @param f string containing names of files that have performance data for one
     *          metric/focus pairs
     * @param winName is name of devise window that would contain this session
     * @exception YException
     */
    public synchronized void show (String f, String winName) throws YException
    { 
	// NEED TO DO: Cleanup earlier written session if any
	
	// Create MFVisual objects
	createMFVisuals (f);
	
	
	// Create mapping class for main window components
	mainMapClName = "Main_Mapping_Class";
	if (!devServer.createMappingClass (mainMapClName)) 
	    throw new YException ("Could not create mapping class");
	
	createIcon ();
	
	// Create link
	linkName = "Main_Time_Link";
	if (!devServer.createLink (linkName)) 
	    throw new YException ("Could not create link");
	
	// Link time axis of piled views of all MFVisual objects
	for (int i = 0; i < numMF; i++)
	    if (!devServer.insertLink (linkName, mfVisuals [i].getPhVName ()))
		throw new YException ("Cound not insert link");
	
	createTimeCursor ();
	
	// Create View that contains all MFVisual objects
	createMFContainerV ();
	
	// Create Y for MFContainerV
	mfConVYCursorMaker ();
	
	// Create one view containing all visuals: mainV
	createMainV (winName);
    }
    
    /**
     * @exception Yexception
     */
    public void createMainV (String winName) throws YException
    {
	// Main view has at most 5 components
	// 1. MFContainerV
	// 2. Time cursor
	// 3. icon
	// 4. MFContainerV's copy      [if numMF > 1]
	// 5. MFContainerV's Y cursor  [if numMF > 1]
	
	// Create tuples for main view tdata.
	String l [];
	
	if (numMF < 2) 
	    l = new String [3];
	else 
	    l = new String [5];
	
	
	// The dimensions of each of the 5 components of the main view are given in
	// mainV's coordinates
	
	double mfConVWt_;           // mfContainer view's height
	double mfConVHt_;           // mfContainer view's width
	
	double timeCursorVHt_;      // timeCursor view's height
	double timeCursorVWt_;      // timeCursor view's width
	
	double iconVHt_;            // icon view's height
	double iconVWt_;            // icon view's width
	
	double mfConCopyVHt_;       // mfContainer view copy view's height
	double mfConCopyVWt_;       // mfContainer view copy view's width
	
	double mfConYCVHt_;         // mfContainer view cursor view's height
	double mfConYCVWt_;         // mfContainer view cursor view's width
	
	double mainVHt_;            // main view's height
	double mainVWt_;            // main view's width
	
	double vDisp;
	double hDisp;
	
	vDisp                = 0.1;
	hDisp                = 0.1;
	
	mfConVWt_            = 10; 
	mfConVHt_            = 10;
	
	timeCursorVHt_       = 1.5;
	timeCursorVWt_       = mfConVWt_;
	
	mfConCopyVHt_        = mfConVHt_;
	mfConCopyVWt_        = 1;
	
	mfConYCVHt_          = mfConCopyVHt_;
	mfConYCVWt_          = mfConCopyVWt_ / 2;
	
	iconVHt_             = timeCursorVHt_;
	iconVWt_             = mfConCopyVWt_ + mfConYCVWt_;
		    
	mainVHt_             = timeCursorVHt_ + vDisp + mfConVHt_;
	mainVWt_             = mfConYCVWt_ + mfConCopyVWt_ + hDisp + mfConVWt_;
	
	double x, y;
	
	// Tuple for time slider
	x     = mainVWt_ - timeCursorVWt_ / 2;
	y     = mainVHt_ - timeCursorVHt_ / 2;
	l [0] = new String (x + " " + y + " " + timeCursorVName + " " + timeCursorVWt_
			    + " " + timeCursorVHt_);
	
	// Tuple for Icon
	x     = iconVWt_ / 2;
	y     = mainVHt_ - iconVHt_ / 2;
	l [1] = new String (x + " " + y + " " + iconVName + " " + iconVWt_ + " " + 
			    iconVHt_);
	
	// Tuple for view containing all MF Views
	// KLUDGE: If no mfConVYCursor is to be drawn extend mfConV till the western
	// border of the window
	if (numMF < 2) mfConVWt_ = mainVWt_;
	x     = mainVWt_  - mfConVWt_ / 2;
	y     = mfConVHt_ / 2;
	l [2] = new String (x + " " + y + " " + mfConVName + " " + mfConVWt_ + " " + 
			    mfConVHt_);
	
	if (numMF > 1) {
	
	    // Tuple for mfConCopy
	    x     = mfConYCVWt_ + mfConCopyVWt_ / 2;
	    y     = mfConCopyVHt_ / 2;
	    l [3] = new String (x + " " + y + " " + mfConVCopyVName + " " + 
				mfConCopyVWt_ + " " + mfConCopyVHt_);
	    
	    // Tuple for mfConCY
	    x     = mfConYCVWt_ / 2;
	    y     = mfConYCVHt_ / 2;
	    l [4] = new String (x + " " + y + " " + mfConVYCursorVName + " " + 
				mfConYCVWt_ + " " + mfConYCVHt_);
	}
	
	// write tdata tuples to tdata file
	mainVTdataFile = new File (mainDir, "mainVTdata"); 
	writeFile (mainVTdataFile.toString (), l, true);
	
	// Create main view
	mainVName = "Main_V";
	if (!devServer.createScatterView (mainVName, 0, mainVWt_, 0, mainVHt_, 4, 37))
	    throw new YException ("Could not create main view");
	
	// Make schema file for TData of main view
	String m []    = { "type " + mainVSchemaType + " ascii", "comment #", 
			   "whitespace ' '", "attr X float", "attr Y float",
			   "attr ViewName string 30", "attr W float", "attr H float" };
	mainVSchemaFile = new File (mainDir, "mainVSchema");
	writeFile (mainVSchemaFile.toString (), m, true);
	
	// Make catalog entry for main view tdata
	mainVTdataName = "Main_View_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (mainVTdataName, 
						  mainVTdataFile.getName (),
						  mainVSchemaType,
						  mainVSchemaFile.getCanonicalPath (),
						  mainDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	// Make mapping for main view
	mainVMapName = "Main_View_Map";
	if (!devServer.createViewMapping (mainMapClName, 
					  mainVTdataName,
					  mainVMapName,
					  "$X",
					  "$Y",
					  "$ViewName",
					  "$W",
					  "$H"))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (mainVName, mainVMapName))
	    throw new YException ("Error detected");

	// Disable unwanted actions
	if (!devServer.setViewActions (mainVName, 1, 1, 1, 1))
	    throw new YException ("Error detected");
	
	// Insert main view into created window
	if (!devServer.insertWindow (mainVName, winName))
	    throw new YException ("Error detected");
    }	
    
    // This method writes specified lines to file "name"
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
    
    /**
     * create view that contains all metric focus pairs
     * @exception YException
     */
    private void createMFContainerV () throws YException
    {
	int numCols   = 1;
	int numRows   = (numMF / numCols);
	int rem       = (numMF % numCols);
	
	if (rem > 0) numRows ++;
	
	// set dimension of child view of MFContainer
	mfConVChildWt = 5;
	mfConVChildHt = 4;
	
	// set dimension of MFContainerV
	mfConVHt      = numRows * mfConVChildHt;
	mfConVWt      = numCols * mfConVChildWt;
	
	// Make schema file for TData of MFContainer view
	mfConVSchemaType = "MFCon_V_Schema";
	String m [] = { "type " + mfConVSchemaType + " ascii", "comment #", 
			"whitespace ' '", "attr X float", "attr Y float",
			"attr ViewName string 30", "attr W float", "attr H float" };
	mfConVSchemaFile = new File (mainDir, "mfConVSchema");
	writeFile (mfConVSchemaFile.toString (), m, true);
	
	// Make tdata file for MFContainer view
	String l []   = new String [numMF];
	
	double x      = mfConVChildWt;
	double y      = mfConVHt;
	
	mfConVInfoList = new Vector ();
	
	for (int i = 0; i < numMF; i++) 
	    {
		l [i] = new String (x + " " + y + " " + mfVisuals [i].getVName ()
				    + " " + mfConVChildWt + " " + mfConVChildHt);
		// Insert entry into mfConVInfoList
		mfConVInfoList.add (new ViewInfo (x, y, mfVisuals [i].getVName (), 
						  mfConVChildWt, mfConVChildHt));
		if (((i+1) % numCols) == 0) { 
		    y -= mfConVChildHt; x = mfConVChildWt;
		}
		else 
		    x += mfConVChildWt;
	    }
	
	// write tdata tuples to tdata file. 
	// each line corresponds to one MFVisual object
	mfConVTdataFile = new File (mainDir, "mfConVTdata"); 
	writeFile (mfConVTdataFile.toString (),l, true);
	
	double dx     = mfConVChildWt / 2;
	double dy     = mfConVChildHt / 2;
	
	// Create view
	mfConVName = "MFCon_View";
	if (!devServer.createScatterView (mfConVName, 
					  dx, 
					  mfConVWt + dx, 
					  dy, 
		 			  mfConVHt + dy, 
					  4, 
					  37))
	    throw new YException ("Could not create view for mfContainer");
	
	// Make catalog entry
	mfConVTdataName = "MFCon_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (mfConVTdataName, 
						  mfConVTdataFile.getName (),
						  mfConVSchemaType,
						  mfConVSchemaFile.getCanonicalPath (),
						  mainDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Create mapping for icon view
	mfConVMName = "MFCon_V_Map";
	if (!devServer.createViewMapping (mainMapClName, 
					  mfConVTdataName,
					  mfConVMName,
					  "$X", 
					  "$Y",
					  "$ViewName", 
					  "$W",
					  "$H"))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (mfConVName, mfConVMName))
	    throw new YException ("Error detected");
	
	if (!devServer.viewSetHome (mfConVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    dx,
				    dy,
				    mfConVWt + dx, 
				    mfConVHt + dy))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (mfConVName,1, 1, 1, 0))
	    throw new YException ("Error detected");
    }
    
    /** 
     * Creates a Y cursor to pan MFContainerV
     * @exception YException
     */
    private void mfConVYCursorMaker () throws YException
    {
	// if only one MFVisual object do not draw cursor
	if (numMF < 2) return;
	
	// Make a YCursor containing a samller view of MFContainerV for MFContainerV.
	//
	// To do this, a smaller copy of MFContainerV and all its child views needs 
	// to be made. A cursor would then be placed over this smaller copy. The 
	// cursor will allow the user to select the region of the real MFContainerV
	// that is to be visible.
	//
	// This however, cannot be done in present DEVise since a cursor does not
	// work over a view containing child views. We remedy this by
	//
	// 1. Constructing a smaller copy of MFContainerV
	// 2. Placing a vertical cursor along the smaller copy's side.
	
	// create copy of MFContainerV
	createMFConVCopy ();
	
	// create vertical cursor. 
	createMFConVYCursor ();
    }
    
    /**
     * Creates a YCursor to pan MFContainerV
     * @exception YException
     */
    private void createMFConVYCursor () throws YException
    {
	// Create cursor
	mfConVYCursorName = "MFConVYCursor";
	if (!devServer.createCursor(mfConVYCursorName, 2) ||
	    !devServer.setCursorGrid (mfConVYCursorName, 1, 
				      (float)mfConVChildWt, 
				      (float)mfConVChildHt, 
				      1)) 
	    throw new YException ("Could not create cursor");
	
	double dx = mfConVChildWt / 2,  dy = mfConVChildHt / 2;
	
	// Create cursor view
	mfConVYCursorVName = "mfConVYCursorVName";
	if (!devServer.createScatterView (mfConVYCursorVName,
					  dx, 
					  (mfConVWt + dx), 
					  dy, 
					  mfConVHt + dy, 
					  30, 
					  0))
	    throw new YException ("Could not create view for mfConVYCursor");
	
	// Create mapping for cursor view. Use Tdata of MFContainerV for obtaining
	// the correct coordinates
	mfConVYCursorMName = "mfConVYCursorMName";
	if (!devServer.createEmpRectMapping (mainMapClName, 
					     mfConVTdataName,
					     mfConVYCursorMName,
					     "$X",
					     "$Y",
					     "30",
					     "$W",
					     "$H"))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
 	if (!devServer.insertMapping (mfConVYCursorVName, mfConVYCursorMName))
	    throw new YException ("Error detected");
	
	if (!devServer.viewSetHome (mfConVYCursorVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    dx,
				    dy,
				    (mfConVWt + dx), 
				    mfConVHt + dy))
	    throw new YException ("Error detected");
	
	// Put views in cursors
 	if (!devServer.setCursorSrc (mfConVYCursorName, mfConVName) ||
	    !devServer.setCursorDst (mfConVYCursorName, mfConVYCursorVName))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (mfConVYCursorVName, 1, 0, 1, 0))
	     throw new YException ("Error detected");
    }
    
    /**
     * Make copy of MFContainerV
     * @exception YException
     */
    private void createMFConVCopy () throws YException 
    {
	// Create schema file
	mfConVCopyVSchemaType = "MFContainer_V_Copy_V_Schema_Type";
	String m [] = { "type " + mfConVCopyVSchemaType + " ascii", "comment #", 
			"whitespace ' '", 
			"attr X float",
			"attr Y float", 
			"attr ViewName string 30",
			"attr PileName string 30",
			"attr W float", 
			"attr H float"};
	mfConVCopyVSchemaFile = new File (mainDir, "XYCursorVSchema");
	writeFile (mfConVCopyVSchemaFile.toString (), m, true);
	
	// Create tdata file
	String l [] = new String [mfConVInfoList.size ()];
	for (int i = 0; i < mfConVInfoList.size (); i++) 
	    {
		ViewInfo tdata = (ViewInfo)mfConVInfoList.elementAt (i);
		
		// for each piled view of this MF make an entry
		MFVisual mf = mfVisuals [i];
		PhaseInfo phInfos [] = mf.getPhaseInfos ();
		
		l [i] = new String (tdata.x_ + " " + 
				    tdata.y_ + " " + 
				    phInfos [0].VSVName + " " +
				    "pile" + i + " " +
				    tdata.w_ + " " + 
				    tdata.h_);
		
		// Disable unwanted actions
		if (!devServer.setViewActions (phInfos [0].VSVName, 1, 0, 1, 1))
		    throw new YException ("Error detected");
	    }
	mfConVCopyVTdataFile = new File (mainDir, "MFContainerVCopyVTdata");
	writeFile (mfConVCopyVTdataFile.toString (), l, true);
	
	// Make catalog entry for mfConVCoypV
	mfConVCopyVTdataName = "MFContainer_V_Copy_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (mfConVCopyVTdataName,
						  mfConVCopyVTdataFile.getName (),
						  mfConVCopyVSchemaType,
						  mfConVCopyVSchemaFile.getCanonicalPath (),
						  mainDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Create mapping for cursor
	mfConVCopyVMName = "MFContainer_V_Copy_V_Map";
	
	if (!devServer.createPiledViewMapping (mainMapClName,
					       mfConVCopyVTdataName,
					       mfConVCopyVMName,
					       "$X",
					       "$Y",
					       "$ViewName",
					       "$W",
					       "$H",
					       "$PileName",
					       -1))
	    throw new YException ("Error detected");
	
	// Create view for cursor
	double dx = mfConVChildWt / 2,  dy = mfConVChildHt / 2;
	mfConVCopyVName = "MFContainer_V_Copy_V";
	if (!devServer.createScatterView (mfConVCopyVName, 
					  dx, 
					  mfConVWt + dx, 
					  dy, 
					  mfConVHt + dy, 
					  4, 
					  37))
	    throw new YException ("Could not create view for mfConVCopyV");
	
	// Insert mapping into view
 	if (!devServer.insertMapping (mfConVCopyVName, mfConVCopyVMName))
	    throw new YException ("Error detected");
	
	if (!devServer.viewSetHome (mfConVCopyVName, 1, 1, 
				    2,
				    1,
				    1,
				    1,
				    dx,
				    dy,
				    mfConVWt + dx, 
				    mfConVHt + dy))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (mfConVCopyVName, 1, 0, 0, 0))
	    throw new YException ("Error detected");
    }
    
    public void cleanUp () {
	
	// Remove top level view from window
	try { devServer.removeView (mainVName); }
	catch (YException e) { e.printStackTrace (); }
	
	// cleanUp all metric/focus visuals
	if (mfVisuals != null || mfVisuals.length > 0) {
	    for (int i = 0; i < mfVisuals.length; i++) {
		mfVisuals [i].cleanUp ();
		mfVisualDirs [i].delete ();
	    }
	}
	
	// cleanUp devise objects
	try {
	    devServer.destroy (linkName);
	    
	    devServer.destroy (timeCursorName);
	    devServer.destroyView (timeCursorVMName, timeCursorVName,
				   timeCursorVTdataName);
	    devServer.destroyView (mfConVMName, mfConVName, mfConVTdataName);
	    devServer.destroyView (iconVMName, iconVName, iconVTdataName);
	    devServer.destroyView (mainVMapName, mainVName, mainVTdataName);
	    
	    // 05/13/2000: Does not get destroyed
	    //devServer.destroyMapCl (mainMapClName);
	    
	    if (numMF > 1) {
		// uses tdata of mfConVTdataName
		devServer.destroyView (mfConVCopyVMName, mfConVCopyVName);
		devServer.destroy (mfConVYCursorName);
		devServer.destroyView (mfConVYCursorVName, mfConVYCursorMName,
				       mfConVCopyVTdataName);
	    }
	
	} catch (YException e) { e.printStackTrace (); }
	
	// cleanUp all files 
	timeCursorVSchemaFile.delete (); 
	timeCursorVTdataFile.delete ();
	
	iconVSchemaFile.delete ();
	iconVTdataFile.delete (); 
	
	mfConVTdataFile.delete ();
	mfConVSchemaFile.delete ();
	    
	mainVSchemaFile.delete ();
	mainVTdataFile.delete (); 
	
	if (numMF > 1) {
	    mfConVCopyVTdataFile.delete ();
	    mfConVCopyVSchemaFile.delete ();
	}
	
	mainDir.delete ();
    }
    
    private class ViewInfo {
	public double x_;
	public double y_;
	public String viewName_;
	public double w_;
	public double h_;
	
	public ViewInfo (double x, double y, String viewName, double w, double h) {
	    x_ = x;
	    y_ = y;
	    viewName_ = viewName;
	    w_ = w;
	    h_ = h;
	}
    }
    
    /**
     * For each metric/focus pair create a single MFVisual object. The MFVisual object
     * contains a piled view that maybe a pile of 1 view or many views. Each view in
     * the pile corresponds to one phase.
     *
     * @param f string containing names of files that have performance data for one
     *          metric/focus pair
     * @exception YException
     */
    private void createMFVisuals (String f) throws YException 
    {
	StringTokenizer st = new StringTokenizer (f, mfSep);
	numMF              = st.countTokens ();
	if (numMF < 1) throw new YException ("Cannot parse input string");
	
	mfVisuals          = new MFVisual [numMF];
	mfVisualDirs       = new File     [numMF];
	
	for (int i = 0; i < numMF; i++) 
	    {
		// Create working directory for the MFVisual
		File d = new File (mainDir, "mf_" + i);
		d.mkdir ();
		if (del) d.deleteOnExit ();
		
		mfVisualDirs [i] = d;
		mfVisuals    [i] = new MFVisual (i, mfVisualDirs [i], devServer, del);
		
		String nextT = st.nextToken ();
		int index    = nextT.lastIndexOf (mfDescStrSep);
		
		if (index == -1) 
		    mfVisuals [i].show (nextT);
		else if (index < nextT.length () - 1)
		    mfVisuals [i].show (nextT.substring (0, index),
					nextT.substring (index + 1, nextT.length ()));
		else 
		    mfVisuals [i].show (nextT.substring (0, index));
	    }
    }
    
    /**
     * @exception YException
     */
    private void createIcon () throws YException 
    {
	// Create icon view
	iconVName            = "Icon_View";
 	if (!devServer.createScatterView (iconVName, 0, 5, 0, 5))
	    throw new YException ("Could not create view for icon");
	
	// Make tdata file for icon view
	String l []          = { iconPath };
	iconVTdataFile       = new File (mainDir, "mainIconTdata"); 
	writeFile (iconVTdataFile.toString (), l, true);
	
	// Make schema for icon view tdata
	iconVSchemaType      = "Icon_V_Schema";
	String m [] = { "type " + iconVSchemaType + " ascii", "comment #", 
			"whitespace ' '", "attr profile string 50"};
	iconVSchemaFile = new File (mainDir, "iconSchema"); 
	writeFile (iconVSchemaFile.toString (), m, true);
	
	// Make catalog entry for icon tdata
	iconVTdataName       = "Icon_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (iconVTdataName, 
						  iconVTdataFile.getName (),
						  iconVSchemaType,
						  iconVSchemaFile.getCanonicalPath (),
						  mainDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) { throw new YException ("IOException"); }
	
	// Create mapping for icon view
	iconVMName = "Icon_V_Map";
	if (!devServer.createImageMapping (mainMapClName, 
					   iconVTdataName,
					   iconVMName,
					   "2.5", 
					   "2.5",
					   2, 
					   5,              // Corresponds to height
					   "$profile",
					   1,
					   0))
					   throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (iconVName, iconVMName))
	throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (iconVName, 1, 1, 1, 1))
	    throw new YException ("Error detected");
    }
    
    /**
     * @exception YException
     */
    private void createTimeCursor () throws YException
    {
	double maxT        = 0;     // maximum time in all metric/focus pairs
	double maxM        = 0;     // maximum metric in all metric/focus pairs
	
	// set maxT & maxM
	for (int i = 0; i < numMF; i++) {
	    double d;
	    d = mfVisuals [i].getMaxT (); if (maxT < d) maxT = d;
	    d = mfVisuals [i].getMaxM (); if (maxM < d) maxM = d;
	}
	
	timeCursorName     = "Main_Time_Cursor";
	if (!devServer.createCursor(timeCursorName)) 
	    throw new YException ("Could not create cursor");
	
	// Create view for cursor
	timeCursorVName    = "Main_Time_Cursor_View";
	if (!devServer.createScatterView (timeCursorVName, 0, maxT, 0, maxM, 30, 0))
	    throw new YException ("Could not create view for cursor");
	
	// configure view
	if (!devServer.setLabel (timeCursorVName, "Time Slider") ||
	    !devServer.setViewFont (timeCursorVName, "title", 2, 10, 1, 0) ||
	    !devServer.setViewFont (timeCursorVName, "x axis", 0, 10, 1, 0) ||
	    !devServer.setViewXAxisDisplay (timeCursorVName, true))
	    throw new YException ("Could not configure view");
	
	// Make schema file for TData of cursor view
	timeCursorVSchemaType = "Main_Time_Cursor_V_Schema";
	String n [] = { "type " + timeCursorVSchemaType + " ascii", "comment #", 
			"whitespace ' '", "attr Time float", "attr Metric float"};
	timeCursorVSchemaFile = new File (mainDir, "timeCursorSchema"); 
	writeFile (timeCursorVSchemaFile.toString (), n, true);
	
	// Make tdata file for cursor view
	String o [] = { "0 1", maxT + " 1" };
	timeCursorVTdataFile = new File (mainDir, "mainCursorTdata"); 
	writeFile (timeCursorVTdataFile.toString (), o, true);
	
	// Make catalog entry for cursor tdata
	timeCursorVTdataName = "Main_Time_Cursor_V_Tdata";
	try {
	    if (!devServer.dteInsertCatalogEntry (timeCursorVTdataName, 
						  timeCursorVTdataFile.getName (),
						  timeCursorVSchemaType,
						  timeCursorVSchemaFile.getCanonicalPath (),
						  mainDir.getCanonicalPath ()))
		throw new YException ("dteInsertCatalogEntry");
	} catch (IOException e) {throw new YException ("IOException"); }
	
	// Create mapping for cursor
	timeCursorVMName = "Main_Time_Cursor_V_Map";
	if (!devServer.createMapping (mainMapClName, 
				      timeCursorVTdataName,
				      timeCursorVMName,
				      "Time", 
				      "Metric",
				      31, 
				      3, // Corresponds to line
				      -1))
	    throw new YException ("Error detected");
	
	// Insert mapping into view
	if (!devServer.insertMapping (timeCursorVName, timeCursorVMName))
	    throw new YException ("Error detected");
	
	
	// Put views in cursors
	if (!devServer.setCursorSrc (timeCursorName, mfVisuals [0].getPhVName()) ||
	    !devServer.setCursorDst (timeCursorName, timeCursorVName))
	    throw new YException ("Error detected");
	
	// Disable unwanted actions
	if (!devServer.setViewActions (timeCursorVName, 1, 0, 0, 0))
	    throw new YException ("Error detected");
	
	if (!devServer.setViewFilter (timeCursorVName, 0, 0, maxT, maxM))
	    throw new YException ("Error detected");
	
	if (!devServer.viewSetHome (timeCursorVName, 0, 0, maxT, maxM))
	    throw new YException ("Error detected");
    }
}
