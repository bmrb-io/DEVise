// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Reads a star file (either local or via the web) and outputs data for
// DEVise.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9  2000/08/29 14:55:52  wenger
// Star2Devise can now extract relaxation parameters, H exchange rates, etc.
// from all appropriate NMR-STAR files, not just bmr4096.str (the sessions
// are not yet fully set up to handle this); fixed an error in the CSI
// calculations; improved test_summarize script.
//
// Revision 1.8  2000/08/17 21:16:14  wenger
// Got rid of Double.parseDouble() calls so this code can work with
// Java 1.1.
//
// Revision 1.7  2000/08/17 17:21:10  wenger
// Summary html file has cleaner format.
//
// Revision 1.6  2000/08/17 14:35:52  wenger
// Made code more tolerant of numerical parsing errors (fixes problem with
// bmr4267.str).
//
// Revision 1.5  2000/08/16 21:50:09  wenger
// Split calcChemShifts into several smaller methods to make it easier to
// deal with.
//
// Revision 1.4  2000/08/11 21:41:34  wenger
// Fixed problems with the code assuming that BMRB accession numbers always
// have four digits.
//
// Revision 1.3  2000/08/03 19:11:52  wenger
// Added S2DException class; better tolerance for certain missing data.
//
// Revision 1.2  2000/08/02 17:47:45  wenger
// Greatly improved error handling.
//
// Revision 1.1  2000/07/28 21:04:42  wenger
// Combined LocalStar2Devise and WebStar2Devise into Star2Devise, other
// cleanup.
//

// ========================================================================

import java.io.*;
import EDU.bmrb.starlibj.*;
import java.util.Vector;
import java.net.URL;

// for chemical shifts
import java.util.Enumeration;
import AssgDataManager.AssgEntry;
import ShiftDataManager.Pair;

public class Star2Devise {

    private static final int DEBUG = 0;

    private StarNode aStarTree;
    private String file_name;
    private Vector value_vector;
    
    //TEMP -- I don't trust this!
    // Location of RESIDUE_SEQ_CODE tag in average_refined_structure,
    //   the first LoopTableNode in value_vector
    private static final int RESIDUE_LOCATION = 2;

    // <-- These members are used to allow splitting up calcChemShifts()
    //   into a number of smaller methods.
    private String _accNum;
    private DataLoopNode _currentDataLoop;
    private ShiftDataManager _refTable;
    private FileWriter _error;
    private int _chemShiftValueIndex;
    private int _atomNameIndex;
    private int _residueLabelIndex;
    private int _residueSeqIndex;
    private int _atomTypeIndex;

    private double _currChemShiftValue;
    private int _currResSeqCode;
    private int _prevResSeqCode;

    private String _prevAtomName;
    // -->

    // ----------------------------------------------------------------------
    public static Star2Devise LocalStar2Devise(String file_name)
      throws S2DException
    {
        Star2Devise s2d = null;
	try {
            FileInputStream stream = new FileInputStream(file_name);
            s2d = new Star2Devise(file_name, stream);
	} catch(FileNotFoundException ex) {
	    System.err.println("Unable to open or read " + ex.getMessage());
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException("Unable to get data in star file " +
	      file_name);
	}

	return s2d;
    }

    // ----------------------------------------------------------------------
    public static Star2Devise WebStar2Devise(String file_name)
      throws S2DException
    {
        Star2Devise s2d = null;
	try {
	    java.net.URL starfile =
	      new java.net.URL(S2DNames.BMRB_STAR_URL + file_name);
	    s2d = new Star2Devise(file_name, starfile.openStream());
	} catch(java.io.IOException ex) {
	    System.err.println("Unable to open or read " + ex.getMessage());
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException("Unable to get data in star file " +
	      file_name);
	}

	return s2d;
    }
    
    // ----------------------------------------------------------------------
    public Star2Devise(String file_name, InputStream stream)
      throws S2DException
    {
	try {
	    this.file_name = file_name;

	    StarParser aParser = 
		new StarParser(stream);
	    
	    aParser.StarFileNodeParse(aParser);
	    
	    // 'aParser' needs to be sent as a parameter because
	    // javacc makes static methods that have no 'this'.
	    aStarTree = aParser.endResult();

	    value_vector = new Vector();
	} catch (ParseException e) {
	    System.err.println("NMR-Star file parse error: " +
	      e.getMessage() );
	    if (DEBUG >= 1) e.printStackTrace();
	    throw new S2DException("Unable to parse " + file_name);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException("Unable to parse " + file_name);
	}
    }


    // ----------------------------------------------------------------------
    /* Function outputData
     * Parameters:
     *   outFileName: the name of the output file, as ASCII text for DEVise
     * Return value: true iff data is successfully output; false otherwise.
     */
    public boolean outputData(String outFileName)
      throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.outputData(" + outFileName + ")");
	}

	if (value_vector.size() == 0) {
	    System.err.println("Error: No data to output!");
	    return false;
	}

	// build the final list of atoms
	LoopTableNode cumulative_vals = (LoopTableNode) value_vector.get(0);
	int[] plot_index = new int[value_vector.size()];
	int[] plot_value = new int[value_vector.size()];
	
	//**  doesn't work for coupling constants...
	for (int plot = 0; plot < value_vector.size(); plot++) {
	    plot_value[plot] = 
		Integer.parseInt
		(((LoopTableNode) value_vector.get(plot))
		 .elementAt(0).elementAt(0).getValue());
	}

	//  Add data into all columns
	int num_atoms = cumulative_vals.size();
	for (int atom = 0; atom < num_atoms; atom++) {
	    int curr_residue =
		Integer.parseInt(cumulative_vals.elementAt(RESIDUE_LOCATION)
				 .elementAt(0).getValue());
	    
	    for (int plot = 0; plot < value_vector.size(); plot++) {
		
		while (plot_index[plot] < 
		       ((LoopTableNode)value_vector.get(plot)).size()
		       && plot_value[plot] < curr_residue) {
		    plot_index[plot]++;

		    plot_value[plot] =
			Integer.parseInt
			(((LoopTableNode) value_vector.get(plot))
			 .elementAt(0).elementAt(0).getValue());
		}

	    }
	}


	try {
	    // output the data
	    File outFile = new File(outFileName);
	    FileOutputStream outStream = 
		new FileOutputStream(outFile);
	    StarUnparser theUnparser = new StarUnparser(outStream);
	    
	    // speed up computation time a bit
	    theUnparser.setFormatting( false );
	    
	    // writes out a LoopTableNode of values
	    theUnparser.writeOut(cumulative_vals, 0);
	    outStream.close();
	    
	    return true;
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    if (DEBUG >= 1) e.printStackTrace();
	    throw new S2DException("Unable to output data for " + file_name);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException("Unable to output data for " + file_name);
	}
    }

    // ----------------------------------------------------------------------
    /* Function findBackbone
     * Parameters:
     *   saveFrameName: the name of the save frame to output
     *   outFileName: the name of the output file, as ASCII text for DEVise
     * Return value: true iff backbone is found; false otherwise.
     *
     * Pulls just the CA atoms out of a residue's structure
     */
    public boolean findBackbone (String saveFrameName, String outFileName)
      throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.findBackbone(" +
	      saveFrameName + ")");
	}

	// ** values?  doesn't work... just find again...

	try {
	    
	    VectorCheckType saveFrameVec;
	    saveFrameVec = 
		aStarTree.searchByName(saveFrameName);

	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if (saveFrameVec.size() == 1) {
		// the last element should be a DataLoopNode
		// containing RESIDUE_SEQ_CODE
		DataLoopNode lastLoop =
		    (DataLoopNode)
		    ( (SaveFrameNode) saveFrameVec.elementAt(0) )
		    .lastElement();

		// make sure that this is the correct DataLoopNode
		// this should find just one instance, because this
		// tag only appears once in the saveframe
		VectorCheckType checkForTag =
		    lastLoop.searchByName(S2DNames.RESIDUE_SEQ_CODE);

		if (checkForTag.size() != 1)
		    throw new S2DException
			("Found "
			 + checkForTag.size()
			 + " " + S2DNames.RESIDUE_SEQ_CODE + " tags"
			 + ", expect 1.");
		
		LoopTableNode avg_struct_vals = lastLoop.getVals();
		final int ATOM_NAME_INDEX = 4;

		LoopTableNode backbone = new LoopTableNode();
		LoopRowNode prev_atom = null;
		// the vector is the atom with vector coordiates at
		// the end
		LoopRowNode curr_vector = null;

		int num_atoms = avg_struct_vals.size();

		for (int atom = 0; atom < num_atoms; atom++) {
		    // if the name of the current (LoopRowNode) atom
		    // is CA, add it to backbone
		    if (avg_struct_vals.elementAt(atom)
			.elementAt(ATOM_NAME_INDEX)
			.getValue().equals(S2DNames.BACKBONE_ATOM_NAME)) {
			
			curr_vector = new LoopRowNode();
			
			// copy all the elements
			curr_vector 
			    = new LoopRowNode(avg_struct_vals.elementAt(atom));

			// ** can I just do new LoopRowNode?
// 			curr_vector = new LoopRowNode();
// 			for (int inner=0;
// 			     inner < avg_struct_vals.elementAt(atom).size();
// 			     inner++) {
// 			    // ** is addElement sufficient instead of
// 			    // insertElementAt?
// 			    curr_vector.insertElementAt
// 				(avg_struct_vals.elementAt(atom).elementAt(inner),
// 				 inner);
// 			}
			
			if (prev_atom != null) {
			    // Compute vectors:
			    LoopRowNode prev_vector = 
				new LoopRowNode(prev_atom);
			    
			    //  x vector
			    double prev_val = 
				(new Double(prev_atom.elementAt(6)
					    .getValue())).doubleValue();
			    double curr_val =
				(new Double(curr_vector.elementAt(6)
					    .getValue())).doubleValue();
			    prev_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((curr_val - prev_val) / 2)).toString()));
			    curr_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((prev_val - curr_val) / 2)).toString()));
		    
			    //  y vector
			    prev_val = 
				(new Double(prev_atom.elementAt(7)
					    .getValue())).doubleValue();
			    curr_val =
				(new Double(curr_vector.elementAt(7)
					    .getValue())).doubleValue();
			    prev_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((curr_val - prev_val) / 2)).toString()));
			    curr_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((prev_val - curr_val) / 2)).toString()));
			
			    //  z vector
			    prev_val = 
				(new Double(prev_atom.elementAt(8)
					    .getValue())).doubleValue();
			    curr_val =
				(new Double(curr_vector.elementAt(8)
					    .getValue())).doubleValue();
			    prev_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((curr_val - prev_val) / 2)).toString()));
			    curr_vector.addElement
				(new DataValueNode
				 ((new Float
				   ((prev_val - curr_val) / 2)).toString()));

			    backbone.addElement(prev_vector);
			    backbone.addElement(curr_vector);
			} // end if prev_atom != null
			
			prev_atom = avg_struct_vals.elementAt(atom);
			
		    }
		    
		} // end for loop through all atoms

		// output the data
		File outFile = new File(outFileName);
		FileOutputStream outStream = 
		    new FileOutputStream(outFile);
		StarUnparser theUnparser = new StarUnparser(outStream);
		
		// speed up computation time a bit
		theUnparser.setFormatting( false );

		// writes out the LoopTableNode of backbone atoms
  		theUnparser.writeOut(backbone, 0);
		outStream.close();

		return true;
		
	    } else if (saveFrameVec.size() == 0) { // none found
		return false;

	    } else {		// multiple search strings found
		throw new S2DException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	    
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    if (DEBUG >= 1) e.printStackTrace();
	    throw new S2DException("Unable to find backbone for save frame " +
	      saveFrameName);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException("Unable to find backbone for save frame " +
	      saveFrameName);
	}
    } // end findBackbone

    // ----------------------------------------------------------------------
    /* Function calcChemShifts
     * takes no parameters; instead, output based on the accession
     * number concatenated with c.dat, d.dat, or p.dat
     * Return value: true iff chem shifts are calculated; false otherwise.
     */
    public boolean calcChemShifts (String acc_num)
      throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.calcChemShifts(" + acc_num + ")");
	}

	final String CHEMSHIFT_FILE = "chemshift.txt";
	final String ERROR_LOG = "error.log";
	
	if (acc_num == null) {
	    _accNum = S2DUtils.starName2Num(file_name);
	} else {
	    _accNum = acc_num;
	}
	    
	try {
	    //Output error log
	    _error = new FileWriter(ERROR_LOG);
	    
	    //Define the object that manages data of the
	    //chemical shift table
	    _refTable = new ShiftDataManager( CHEMSHIFT_FILE );
	    
	    //Create tagname and datavalue
	    final String tagName = S2DNames.SAVEFRAME_CATEGORY;
	    final String dataValue = S2DNames.ASSIGNED_CHEM_SHIFTS;
	    
	    //Find list of all occurences in the star file
	    
	    //Initialize a list of ASTnode;
	    SaveFrameNode currSaveFrame;
	    
	    // Get an enumeration of the save frames of the specified type.
	    Enumeration matches = aStarTree.searchForTypeByTagValue(
	      Class.forName( StarValidity.pkgName()+".SaveFrameNode"),
	      tagName, dataValue ).elements();
	
	    //For each match only process if item is in a saveframelist node
	    while (matches.hasMoreElements())
	    {		
		//
		// Get the next available save frame.
		//
		currSaveFrame = (SaveFrameNode)matches.nextElement();
		
	        String saveFrameName = null;
		if (currSaveFrame != null) {
		    saveFrameName 
			= ((SaveFrameNode)currSaveFrame).getLabel();
		    
		    //Strip the save_
		    saveFrameName = saveFrameName.substring(
		      S2DNames.SAVEFRAME_PREFIX.length());
		} else {
		    saveFrameName = "N/A" ;
		}

	        if (DEBUG >= 2) {
	            System.out.println("Processing save frame " +
		      saveFrameName);
	        }
	    
		//
		// Now to obtain a pointer to the dataloop
		// containing the specific tag.
		//
		VectorCheckType loopMatches;
		loopMatches 
		    = currSaveFrame.searchForTypeByName
		    ( Class.forName( StarValidity.pkgName()
				     + ".DataLoopNode"),
		      S2DNames.CHEM_SHIFT_VALUE);
		
		// There should be only one data loop.
		if (loopMatches.size() != 1) {
		    throw new S2DException("Found " + loopMatches.size() +
		      " _Chem_shift_value tags, only expect 1." );
		}
	    
		_currentDataLoop = (DataLoopNode)loopMatches.firstElement();
	    
		//
		// Get the indices to the various data elements that we
		// need.
		//
                try {
                    getIndices();
                } catch (S2DException ex) {
                    System.err.println("Error getting index: " +
                      ex.getMessage());
                    break;
                }

		//
		// Write the deltashift info.
		//
		writeDeltashifts();

		//
		// Write the CSI info.
		//
		writeCsi();

		//
		// Write the percent assignment info.
		//
		writeAssignments();
	    } // end while matches.hasMoreElements()
	    
	    _error.close();
	    return true;
	} catch (ClassNotFoundException e) {
	    System.err.println("ClassNotFoundException: " + e.getMessage() );
	    if (DEBUG >= 1) e.printStackTrace();
	    throw new S2DException(
	      "Unable to find/calculate chem shifts for " + _accNum);
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    if (DEBUG >= 1) e.printStackTrace();
	    throw new S2DException(
	      "Unable to find/calculate chem shifts for " + _accNum);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    if (DEBUG >= 1) ex.printStackTrace();
	    throw new S2DException(
	      "Unable to find/calculate chem shifts for " + _accNum);
	} finally {
	    _accNum = null;
	    _currentDataLoop = null;
	    _refTable = null;
	    _error = null;
	    _prevAtomName = null;
	}
    }

    // ----------------------------------------------------------------------
    // Get the indices we need for the chem shift info.
    private void getIndices() throws S2DException
    {
        // necessary for tagPositionDeep procedure
        RemoteInt returnIndex = new RemoteInt();
        RemoteInt nestLevel = new RemoteInt();

	//And now retrieve chemical shift values
	_currentDataLoop.getNames().tagPositionDeep(S2DNames.CHEM_SHIFT_VALUE,
	  nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for chemical shift values");
	} else {
	    _chemShiftValueIndex = returnIndex.num;
	}
		
	//Retrieve atom name
	_currentDataLoop.getNames().tagPositionDeep(S2DNames.ATOM_NAME,
	  nestLevel, returnIndex );

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for atom names");
	} else {
	    _atomNameIndex = returnIndex.num;
	}
		
	//Retrieve residue label
	_currentDataLoop.getNames().tagPositionDeep(S2DNames.RESIDUE_LABEL,
	  nestLevel, returnIndex );

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for residue labels");
	} else {
	    _residueLabelIndex = returnIndex.num;
	}
		  
	//Retrieve sequence numbers
	_currentDataLoop.getNames().tagPositionDeep(S2DNames.RESIDUE_SEQ_CODE,
	  nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for sequence numbers");
	} else {
	    _residueSeqIndex = returnIndex.num;
        }

	//Retrieve atom type
	_currentDataLoop.getNames().tagPositionDeep(S2DNames.ATOM_TYPE,
	  nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for atom types");
	} else {
	    _atomTypeIndex = returnIndex.num;
	}
    }

    // ----------------------------------------------------------------------
    // Write out the deltashift info.
    private void writeDeltashifts() throws IOException, S2DException
    {
	//Do the shift computations here
	FileWriter deltashift_writer = new FileWriter( _accNum +
	  S2DNames.DELTASHIFT_DAT_SUFFIX);
		
	_currResSeqCode = -1;
	_prevResSeqCode = -1;
	    
	//Processing values in the loop
	float ha_deltashift = 0, c_deltashift = 0, ca_deltashift = 0, 
	    cb_deltashift = 0;
	_prevAtomName = "uninitialized value";
	// prevResLabel = "uninitialized value",

	// get deltashift vals out first
	int maxRows  = _currentDataLoop.getVals().size();
	for (int currRowNum = 0; currRowNum < maxRows; currRowNum++)
	{
		    
	    LoopRowNode currRow 
		= _currentDataLoop.getVals().elementAt(currRowNum);
	    
	    try {
	        _currChemShiftValue = (new Double(currRow.elementAt(
		  _chemShiftValueIndex).getValue())).doubleValue();
	    } catch(Exception ex) {
	        System.err.println("Error parsing chem shift value: " +
		  ex.getMessage());
	        if (DEBUG >= 1) ex.printStackTrace();
		_currChemShiftValue = 0.0;
	    }
	    
	    try {
	    _currResSeqCode = Integer.parseInt(
	      currRow.elementAt(_residueSeqIndex).getValue());
	    } catch(Exception ex) {
	        System.err.println("Error parsing residue sequence code: " +
		  ex.getMessage());
	        if (DEBUG >= 1) ex.printStackTrace();
		_currResSeqCode = 0;
	    }
	    
	    String currAtomName 
		= currRow.elementAt(_atomNameIndex).getValue();
	    
	    if (_currResSeqCode != _prevResSeqCode &&
		_currResSeqCode != 1) 
	    { // assume code starts at 1
		
		deltashift_writer.write
		    (_prevResSeqCode + " " + ha_deltashift + " "
		     + c_deltashift + " " + ca_deltashift + " "
		     + cb_deltashift + "\n");
		
		// re-initialize values
		ha_deltashift = 0;
		c_deltashift = 0;
		ca_deltashift = 0;
		cb_deltashift = 0;
		
		_prevResSeqCode = _currResSeqCode;
		_prevAtomName = currAtomName;
		
	    } else if (_currResSeqCode == 1) 
	    { // init data
		_prevResSeqCode = _currResSeqCode;
		_prevAtomName
		    = currAtomName;
	    }
		     
	     String currResLabel 
		 = currRow.elementAt(_residueLabelIndex).getValue();
	     
	     //Computes delta delta
	     Pair  standardValue = 
		 _refTable.returnValues
		 (_error, currResLabel, currAtomName);
	     
	     if(standardValue.chemshift > 0.0) {
		 float deltashift 
		     =  (float)(_currChemShiftValue 
				- standardValue.chemshift);
		 
		 //The special cases of combining HA with HA2
		 //and HA3 with CB as per algorithm
		 if(currAtomName.compareTo("HA") == 0
		    || (currAtomName.compareTo("HA2") == 0
			&& currResLabel.compareTo("GLY") == 0))
		 {
		     ha_deltashift = deltashift;
		     _prevAtomName = "HA";
		 } else if (currAtomName.compareTo("C") == 0) {
		     c_deltashift = deltashift;
		 } else if (currAtomName.compareTo("CA") == 0) {
		     ca_deltashift = deltashift;
		 } else if (currAtomName.compareTo("CB") == 0
			  || (currAtomName.compareTo("HA3") == 0
			      && currResLabel.compareTo("GLY") == 0))
		 {
		     cb_deltashift = deltashift;
		     _prevAtomName = "CB";
		 }
			
	     } // end if chemshift value > 0.0
	    
	 } // for loop that iterates through this DataLoop
		
	// write the data for the last entry
	deltashift_writer.write
	    (_prevResSeqCode + " " 
	     + ha_deltashift + " " + c_deltashift + " "
	     + ca_deltashift + " " + cb_deltashift + "\n");
	deltashift_writer.close();

        if (DEBUG >= 2) {
  	    System.out.println("Finished deltashift vals.");
        }
    }

    // ----------------------------------------------------------------------
    // Write out the CSI info.
    private void writeCsi() throws IOException, S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("writeCsi()");
	}

	FileWriter csi_writer = new FileWriter( _accNum +
	  S2DNames.CSI_DAT_SUFFIX );

	LineTokens deltaShiftLine = new LineTokens(new StreamTokenizer(
	  new FileReader(_accNum + S2DNames.DELTASHIFT_DAT_SUFFIX)));

	// The CSI values (initialized "for real" inside the loop).
	int ha_csi = 0, c_csi = 0, ca_csi = 0, cb_csi = 0, cons_csi = 0;

	int prevCsSeq = -1;
	int currCsSeq = -1;
	int currDelSeq = -1;
        boolean hasCsiInfo = false;

	int maxRows  = _currentDataLoop.getVals().size();
	for (int currRowNum = 0; currRowNum < maxRows; currRowNum++) {

	    //
	    // Get the next row of chem shift info and get its sequence number.
	    //
	    LoopRowNode currRow =
	      _currentDataLoop.getVals().elementAt(currRowNum);

	    try {
	        currCsSeq = Integer.parseInt(
		  currRow.elementAt(_residueSeqIndex).getValue());
	    } catch(Exception ex) {
	        System.err.println("Error parsing residue sequence code: " +
		  ex.getMessage());
	        if (DEBUG >= 1) ex.printStackTrace();
		currCsSeq = -1;
	    }

	    //
	    // If the current chem shift row's sequence number is different
	    // from the previous one, we are done with the previous sequence
	    // number (if there is one) and we need to write out the
	    // chem shift info and read the deltashifts for the new
	    // sequence number.
	    //
            if (currCsSeq != prevCsSeq && currCsSeq != -1) {
                if (hasCsiInfo) {
		    csi_writer.write(prevCsSeq + " " + ha_csi + " " + c_csi +
		      " " + ca_csi + " " + cb_csi + " " + cons_csi + "\n");
	        }

	        while (currDelSeq < currCsSeq) {
		    deltaShiftLine.readAndTokenLine();
		    currDelSeq = deltaShiftLine.seqNumber;
	        }

		// Initialize.
	        prevCsSeq = currCsSeq;
	        hasCsiInfo = false;

		ha_csi = 0;
		c_csi = 0;
		ca_csi = 0;
		cb_csi = 0;
		cons_csi = deltaShiftLine.csi;
            }

	    //
	    // If the chem shift sequence number matches the deltashift
	    // sequence number, do the calculations for this chem shift
	    // row.
	    //
            if (currCsSeq == currDelSeq) {
                hasCsiInfo = true;
        
	        try {
	            _currChemShiftValue = (new Double(currRow.elementAt(
		      _chemShiftValueIndex).getValue())).doubleValue();
	        } catch(Exception ex) {
	            System.err.println("Error parsing chem shift value: " +
		      ex.getMessage());
	            if (DEBUG >= 1) ex.printStackTrace();
		    _currChemShiftValue = 0.0;
	        }

	        String currAtomName =
		  currRow.elementAt(_atomNameIndex).getValue();

 	        String currResLabel =
		  currRow.elementAt(_residueLabelIndex).getValue();

	        // Computes delta delta
	        Pair  standardValue = _refTable.returnValues(_error,
		  currResLabel, currAtomName);
	    
	        if(standardValue.chemshift > 0.0) {
		    float deltashift = (float)(_currChemShiftValue -
		      standardValue.chemshift);
			
		    // And now to compute the CSI's
		    int csi ;
		    if (deltashift > standardValue.offset) {
		        csi = 1;
		    } else if(deltashift < -1.0*standardValue.offset) {
		        csi = -1;
		    } else {
		        csi = 0;
		    }
		    
		    // The special cases of combining HA with HA2
		    // and HA3 with CB as per algorithm
		    if(currAtomName.compareTo("HA") == 0 ||
		      (currAtomName.compareTo("HA2") == 0 &&
		      currResLabel.compareTo("GLY") == 0))
		    {
		        ha_csi = csi;
		    } else if (currAtomName.compareTo("C") == 0) {
		        c_csi = csi;
		    } else if (currAtomName.compareTo("CA") == 0) {
		        ca_csi = csi;
		    } else if (currAtomName.compareTo("CB") == 0
			 || (currAtomName.compareTo("HA3") == 0
			     && currResLabel.compareTo("GLY") == 0)) {
		        cb_csi = csi;
		    }
	        } // if chemshift value > 0.0
            }
        }

	//
	// Write out the chem shift info for the last sequence.
	//
        if (hasCsiInfo) {
	    csi_writer.write(prevCsSeq + " " + ha_csi + " " + c_csi + " " +
	      ca_csi + " " + cb_csi + " " + cons_csi + "\n");
        }

	csi_writer.close();
    }

    // ----------------------------------------------------------------------
    // Write out the percent assignment info.
    private void writeAssignments() throws IOException, S2DException
    {
	//Do the atom assignment computation here
	int starnumH, starnumC, starnumN, numH, numC, numN;
	  
	FileWriter percent_writer = new FileWriter( _accNum +
	  S2DNames.PERCENT_ASSIGN_DAT_SUFFIX );
	
	AssgDataManager assgTable 
	    = new AssgDataManager( S2DNames.CHEMASSG_FILE );
	
	int maxRows  = _currentDataLoop.getVals().size();
	int currRowNum = 0;
	while (currRowNum < maxRows)
	{
	    LoopRowNode currRow 
		= _currentDataLoop.getVals().elementAt(currRowNum);
	    
	    //Process a block with same residue label and
	    //sequence code
	    String currResidueLabel
		= currRow.elementAt(_residueLabelIndex).getValue();
	    String currSeqNum
		= currRow.elementAt(_residueSeqIndex).getValue();

	      starnumH = 0;
 	      starnumC = 0;
 	      starnumN = 0;
		      
	      AssgEntry assignments =
		  assgTable.returnAssg(_error, currResidueLabel);
	      numH = assignments.numH;
	      numC = assignments.numC;
	      numN = assignments.numN;
		      
 	      if(numH != -1 && numC != -1 && numN != -1)
	      {
 		  //Only if an entry exists in the chemical
 		  //assignment table file
		  while (currRowNum < maxRows &&
			 currResidueLabel.equals(
			    currRow.elementAt(_residueLabelIndex)
			    .getValue()) &&
			 currSeqNum.equals(
			      currRow.elementAt(_residueSeqIndex)
			      .getValue()))
		  {			      
		      String new_atom =
			  currRow.elementAt(_atomTypeIndex).getValue();
		      
		      if (new_atom.equals("H")) {
			  starnumH++;
		      } else if (new_atom.equals("C")) {
			  starnumC++;
		      } else if (new_atom.equals("N")) {
			  starnumN++;
		      }

		      if (++currRowNum < maxRows) {
			  currRow
			      = _currentDataLoop.getVals()
			      .elementAt(currRowNum);
		      }
		  }

		  percent_writer.write(currSeqNum + " "
				       + currResidueLabel + " "
				       + 100 * (float)starnumH / numH 
				       + " "
				       + 100 * (float)starnumC / numC
				       + " "
				       + 100 * (float)starnumN / numN
				       + " "
				       + "\n");
			  
 		} else
		{ // numH || numC || numN == -1
		    currRowNum++;
 		}
		      
	} // end while currRowNum < maxRows
		
	percent_writer.close();
//  	System.out.println("Finished percent assignments.");
    }

    // ----------------------------------------------------------------------
    /* Function summarize
     * Return value: true iff summary is successful; false otherwise.
     */
    public boolean summarize( String outFileName )
      throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.summarize(" + outFileName + ")");
	}

	try {
	    VectorCheckType saveFrameVec =
		aStarTree.searchByName( S2DNames.SAVE_ENTRY_INFO );
	    if (DEBUG >= 2) {
	        System.out.println("  summarizing " + S2DNames.SAVE_ENTRY_INFO);
	    }

	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if ( saveFrameVec.size() != 1 ) {
		throw new S2DException
		    ("Found " + saveFrameVec.size()
		     + " save frames for save_entry_information;"
		     + " expect exactly 1.");
	    }
	    
	    // This finds the Entry Title
	    String title = "unknown";
	    try {
	        title = ((DataItemNode)
		  ((SaveFrameNode) saveFrameVec.firstElement())
		  .searchByName(S2DNames.ENTRY_TITLE) .firstElement()).
		  getValue();
            } catch(Exception ex) {
	        System.err.println("Exception getting title: " +
		  ex.getMessage());
	        if (DEBUG >= 1) ex.printStackTrace();
	    }

            String system_name = "unknown";
	    try {
	        system_name = ((DataItemNode)
		  aStarTree.searchByName(S2DNames.MOL_SYSTEM_NAME)
		  .firstElement()).getValue();
            } catch(Exception ex) {
	        System.err.println("Exception getting system name: " +
		  ex.getMessage());
	        if (DEBUG >= 1) ex.printStackTrace();
	    }

	    VectorCheckType category_vec =
		aStarTree.searchForTypeByName
		( Class.forName( StarValidity.pkgName()+".DataLoopNode"),
		  "_Saveframe_category_type" );

	    if ( category_vec.size() != 1 ) {
		throw new S2DException
		    ("Found " + saveFrameVec.size()
		     + " save frames for _Saveframe_category_type,"
		     + " expect exactly 1.");
	    }

	    DataLoopNode category_loop = 
		((DataLoopNode) category_vec.firstElement() );

	    LoopTableNode category_vals = category_loop.getVals();

	    // generate a web page here
	    String the_number = null;
	    if (outFileName == null) {
	        the_number = S2DUtils.starName2Num(file_name);
	    } else {
	        the_number = outFileName;
	    }

  	    FileOutputStream outStream 
		= new FileOutputStream(the_number +
		S2DNames.SUMMARY_HTML_SUFFIX);
 	    PrintWriter summary_writer 
 		= new PrintWriter(new BufferedWriter
 				  ( new OutputStreamWriter ( outStream ))) ;
	    summary_writer.println("<html>\n<head>\n<title>Summary for " +
				   file_name + "</title>\n</head>\n" + 
				   "<body bgcolor = white>\n");
	    summary_writer.println("<h3>DEVise plots for:");
	    summary_writer.println(system_name + "</h3>");
	    summary_writer.println("Title: <tt>" + title + "</tt>");
	    
	    // new display- pages generated my make_view
	    String display_link_base = "<br><br><a href=\"" + the_number;
	    

            //
	    // Save the data available in this STAR file.
	    //
	    boolean savedChemShifts = false;
	    boolean savedRelax = false;
	    boolean savedHExch = false;
	    boolean savedCoupling = false;
	    boolean savedS2Params = false;

	    // DataLoopNode contains DataLoopNameListNode, and LoopTableNode
	    for ( int i = 0; i < category_vals.size(); i++) {
		String current_tag =
		    category_vals.elementAt(i).firstElement().getValue();
		if (DEBUG >= 2) {
		    System.out.println("Tag = " + current_tag);
		}

		if (current_tag.equals(S2DNames.ASSIGNED_CHEM_SHIFTS)) {
		    savedChemShifts = calcChemShifts(null);
		    
		} else if (current_tag.equals(S2DNames.T1_RELAX)) {
		    try {
		        saveGeneric(the_number, S2DNames.T1_RELAX,
		          S2DNames.RESIDUE_SEQ_CODE, S2DNames.T1_DAT_SUFFIX,
			  null);
		        savedRelax = true;
		    } catch (Exception ex) {
		        System.err.println("Exception saving T1 relaxation: "
			  + ex.getMessage());
		    }

		} else if (current_tag.equals(S2DNames.T2_RELAX)) {
		    try {
		        saveGeneric(the_number, S2DNames.T2_RELAX,
		          S2DNames.RESIDUE_SEQ_CODE, S2DNames.T2_DAT_SUFFIX,
			  null);
		        savedRelax = true;
		    } catch (Exception ex) {
		        System.err.println("Exception saving T2 relaxation: "
			  + ex.getMessage());
		    }

		} else if (current_tag.equals(S2DNames.HETERONUCLEAR_NOE)) {
		    try {
		        saveGeneric(the_number, S2DNames.HETERONUCLEAR_NOE,
		          S2DNames.RESIDUE_SEQ_CODE, "n", null);
		        savedRelax = true;
		    } catch (Exception ex) {
		        System.err.println(
			  "Exception saving heteronuclear NOE: " +
			  ex.getMessage());
		    }

		// Watch out for RATE vs. RATES!!
		} else if (current_tag.equals(S2DNames.H_EXCHANGE_RATE)) {
		    try {
		        saveGeneric(the_number, S2DNames.H_EXCHANGE_RATES,
		          S2DNames.RESIDUE_SEQ_CODE, "r",
			  S2DNames.H_EXCHANGE_RATE_VALUE);
		        savedHExch = true;
		    } catch (Exception ex) {
		        System.err.println(
			  "Exception saving H exchange rates: " +
			  ex.getMessage());
		    }

		} else if (current_tag.equals(S2DNames.COUPLING_CONSTANTS)) {
		    try {
		        saveGeneric(the_number, S2DNames.COUPLING_CONSTANTS,
		          S2DNames.COUPLING_CONSTANT_CODE, "g", null);
		        savedCoupling = true;
		    } catch (Exception ex) {
		        System.err.println(
			  "Exception saving coupling constants: " +
			  ex.getMessage());
		    }

		} else if (current_tag.equals(S2DNames.H_EXCHANGE_PROT_FACT)) {
		    try {
		        saveGeneric(the_number, S2DNames.H_EXCHANGE_PROT_FACT,
		          S2DNames.RESIDUE_SEQ_CODE, "f",
			  S2DNames.H_EXCHANGE_PROT_FACT_VALUE);
		        savedHExch = true;
		    } catch (Exception ex) {
		        System.err.println(
			  "Exception saving H exchange protection factors: " +
			  ex.getMessage());
		    }

		} else if (current_tag.equals(S2DNames.S2_PARAMS)) {
		    try {
		        saveGeneric(the_number, S2DNames.S2_PARAMS,
		          S2DNames.RESIDUE_SEQ_CODE, "s", null);
		        savedS2Params = true;
		    } catch (Exception ex) {
		        System.err.println("Exception saving S2 parameters: "
			  + ex.getMessage());
		    }
		}
	    }


	    //
	    // Now make links in the summary page to the html files
	    // that display the data we saved.
	    //
	    String display_link;
	    boolean plotsAvailable = false;

            if (savedChemShifts) {
		plotsAvailable = true;

		display_link = display_link_base +
		  S2DNames.CHEM_SHIFT_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Chemical Shift Index</a>");

	        display_link = display_link_base + S2DNames.DELTA_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Chemical Shift Delta</a>");

	        display_link = display_link_base +
		  S2DNames.PCT_ASSIGNED_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Percent Assigned Atoms</a>");
	    }

	    if (the_number.equals("4096")) { //TEMP -- current code/installation
	      // treats 4096 as a special case
	    if (savedRelax) {
		plotsAvailable = true;

	        display_link = display_link_base + S2DNames.RELAX_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Relaxation Parameters</a>");
	    }

	    if (savedHExch) {
		plotsAvailable = true;

	        display_link = display_link_base +
		  S2DNames.H_EXCH_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">H-Exchange Rates</a>");
	    }

	    if (savedCoupling) {
		plotsAvailable = true;

	        display_link = display_link_base +
		  S2DNames.COUPLING_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Coupling Constants</a>");
	    }

	    if (savedS2Params) {
		plotsAvailable = true;

	        display_link = display_link_base + S2DNames.ORDER_HTML_SUFFIX;
		summary_writer.print(display_link);
		summary_writer.println("\">Order Parameters</a>");
	    }
	    } //TEMP

	    if (!plotsAvailable) {
	        summary_writer.print("<p>No DEVise plots currently" +
		  " available for this data\n");
	    }

	    summary_writer.print("</body>\n</html>\n");

	    summary_writer.close();
 	    outStream.close();
	    
	    return true;

       	} catch (FileNotFoundException e) {
 	    System.err.println("File not found exception:  "
 			       + e.getMessage() );
	    throw new S2DException("Unable to summarize " + file_name);
       	} catch (IOException e) {
 	    System.err.println("IO exception:  "
 			       + e.getMessage() );
	    throw new S2DException("Unable to summarize " + file_name);
 	} catch (Exception ex) {
 	    System.err.println("Exception:  " + ex.getMessage() );
	    throw new S2DException("Unable to summarize " + file_name);
	}
    }

    // ----------------------------------------------------------------------
    /* Function sqaure
     *  takes a Float, and squares the number
     */
    public float square(float x) {
	return x*x;
    }


    // ----------------------------------------------------------------------
    /* Function countConstraints
     *
     *  Count the number of distance constraints for each residue
     * Return value: true iff constraints are counted successfully;
     *   false otherwise.
     */

    public boolean countConstraints(String the_number)
      throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.countConstraints(" +
	      the_number + ")");
	}

	int[] constraint_count = null;
	float[] rms;		// for later

	// find number of residues
	VectorCheckType saveFrameVec;
	saveFrameVec = 
	    aStarTree.searchByName(S2DNames.SAVE_GMG4);

	if (saveFrameVec.size() == 0) {
	    return false;
	} else if (saveFrameVec.size() != 1) {
	    throw new S2DException
		("In countConstraints, found " + saveFrameVec.size()
		 + " save frames, expect exactly 1.");
        }

	int num_residues =
	    Integer.parseInt
	    (((DataItemNode)
	       ( ((SaveFrameNode) saveFrameVec.firstElement())
		 .searchByName(S2DNames.RESIDUE_COUNT) ).firstElement() )
	      .getValue());
	
	if (DEBUG >= 2) {
	    System.out.println(" " + num_residues + " residues.");
        }

	// count the distance constraints per residue
	saveFrameVec =
	    aStarTree.searchByName(S2DNames.SAVE_DIST_CONSTRAINTS);
	 
	if (saveFrameVec.size() == 0) // none found
	    return false;
	
	else if (saveFrameVec.size() != 1) { // search strings found
	    throw new S2DException
		("Found " + saveFrameVec.size()
		 + " save frames,"
		 + " expect exactly 1.");
	} else {
	    constraint_count = new int[num_residues + 1];
	    
	    // SaveFrameNode.DataLoopNode.LoopTableNode
	    LoopTableNode lastLoop =
		((DataLoopNode)
		 ( (SaveFrameNode) saveFrameVec.elementAt(0) )
		 .lastElement()).getVals();

	    for (int id = 0; id < lastLoop.size(); id++) {

		// LoopTableNode.LoopRowNode.DataValueNode
		String first_residue = 
		    lastLoop.elementAt(id).elementAt(2).getValue();
		String second_residue = 
		    lastLoop.elementAt(id).elementAt(5).getValue();

		if (lastLoop.elementAt(id).elementAt(1).getValue()
		    .equals(S2DNames.HEME)) {
		    
		    if (! lastLoop.elementAt(id).elementAt(4).equals(
		      S2DNames.HEME)) {
			
			constraint_count[Integer.parseInt(second_residue)]++;
		    }

		} else {
		    constraint_count[Integer.parseInt(first_residue)]++;

		if (! first_residue.equals(second_residue)
		    && ! lastLoop.elementAt(id).elementAt(4).getValue()
		    .equals(S2DNames.HEME))
		    
		    constraint_count[Integer.parseInt(second_residue)]++;
		}
	    
	    } // end loop through all distance constraints
	    
	} // end if exactly one save frame found

	// calculate root mean squared deviation for CA atom coordinates
	float[][] avg_coords = new float[num_residues + 1][3];
	saveFrameVec =
	    aStarTree.searchByName
	    (S2DNames.SAVE_CONF_STAT);


//  	for (int rcount = 1; rcount < num_residues + 1; rcount++) {
// 	    for (int dir = 0; dir < 3; dir++) {
// 		avg_coords[rcount][dir] =
// 		    Float.parseFloat(avg_struct_vals.elementAt(rcount)
// 				     .elementAt(6 + dir).getValue());
// 	    }
// 	}
	
	// find family of structures
	// ** current working pos
	saveFrameVec =
	    aStarTree.searchByName
	    (S2DNames.SAVE_CONF_STAT);
	
	if (saveFrameVec.size() == 0) // none found
	    return false;

	else if (saveFrameVec.size() != 1) { // search strings found
	    throw new S2DException
		("Found " + saveFrameVec.size()
		 + " save frames,"
		 + " expect exactly 1.");
	}
	
	float[][] total_error = new float[num_residues + 1][3];
	int num_conformers =
	    Integer.parseInt
	    (((DataItemNode)
	      ( ((SaveFrameNode) saveFrameVec.firstElement())
		.searchByName(S2DNames.CONF_SUB_TOTAL) )
	      .firstElement() )
	     .getValue());
	
 	if (DEBUG >= 3) {
 	    System.out.println(" " + num_conformers + " conformers");
	}
	
	//TEMP -- this is probably wrong!
	saveFrameVec =
	    aStarTree.searchByName("save_GMH4CO_conformer_family");
	
	if (saveFrameVec.size() == 0) { // none found
	    return false;
	} else if (saveFrameVec.size() != 1) { // search strings found
	    throw new S2DException
		("Found " + saveFrameVec.size()
		 + " save frames,"
		 + " expect exactly 1.");
	} else {
	    float[][][] all_coords = 
		new float[num_conformers][num_residues + 1][3];
	
	    // SaveFrameNode.DataLoopNode.LoopTableNode
	    LoopTableNode family_coords =
		((DataLoopNode)
		 ( (SaveFrameNode) saveFrameVec.elementAt(0) )
		 .lastElement()).getVals();

	    for (int len = 0; len < family_coords.size(); len++) {
		LoopRowNode curr_row = family_coords.elementAt(len);

		if (curr_row.elementAt(5).getValue().equals("CA")) {
		    // store coordinate values
		    // find average structure values
		    int curr_conformer_1 = // conformer # - 1
			Integer.parseInt(curr_row.elementAt(1).getValue()) - 1;
		    int curr_rsc = // residue sequence code
			Integer.parseInt(curr_row.elementAt(3).getValue());

		    for (int dir = 0; dir < 3; dir++) {
			
			all_coords[curr_conformer_1][curr_rsc][dir] =
			    Float.parseFloat
			    (curr_row.elementAt(7 + dir).getValue());

			avg_coords[curr_rsc][dir] += 
			    all_coords[curr_conformer_1][curr_rsc][dir];
		    }
		} // end if current atom is CA

	    } // end loop through family of coordinates
	    if (DEBUG >= 2) {
		System.out.println(" Read " + num_conformers +
				   " conformers into memory");
            }

// 		    // update avgs
// 		    int res_code = 
// 			Integer.parseInt(curr_row.elementAt(3).getValue());
// 		    for (int dir = 0; dir < 3; dir++) {
// 			total_error[res_code][dir] += 
// 			    square(Float.parseFloat(curr_row
// 						    .elementAt(7 + dir)
// 						    .getValue()));
//                  }

	    
	    // finish calculating averages
	    // calculate RMS
	    rms = new float[num_residues + 1];

	    for (int rcount = 1; rcount < num_residues + 1; rcount++) {
		for (int dir = 0; dir < 3; dir++) {
		    avg_coords[rcount][dir] /= (float)num_conformers;
		    rms[rcount] += square(avg_coords[rcount][dir]);
 		    if (DEBUG >= 3) {
 			System.out.println(" avg[" + rcount + "][" + dir +
 					   "] = " + avg_coords[rcount][dir]);
		    }
		}
	    }

	    
	    for (int rcount = 1; rcount < num_residues + 1; rcount++) {
		rms[rcount] = 
                  	(float)Math.sqrt(rms[rcount]) / num_conformers;
 		if (DEBUG >= 3) {
 		    System.out.println(" RMS[" + rcount + "] = " +
 				       rms[rcount]);
	        }
	    }
	    
	} // end if exactly one save frame with this name

	try {
	    PrintWriter out_writer = 
		new PrintWriter
		(new BufferedWriter
		 (new FileWriter(the_number +
		 S2DNames.CONSTRAINTS_DAT_SUFFIX)));
		    
	    for (int outcount = 1; 
		 outcount < num_residues + 1; 
		 outcount++) {
		out_writer.println(outcount + " " + 
				   constraint_count[outcount] +
				   rms[outcount]);
	    }
		    
	    out_writer.close();
		
	} catch (IOException e) {
	    System.err.println("IO Exception: " + ": " + e.getMessage() );
	    throw new S2DException("Unable to count constraints for " +
	      the_number);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    throw new S2DException("Unable to count constraints for " +
	      the_number);
	}

	return true;
    }
    
    // ----------------------------------------------------------------------
    protected void finalize() throws Throwable {
	super.finalize();
    }

    // ----------------------------------------------------------------------
    // Save a "generic" save frame.
    // Parameters:
    //   accNo: accession number
    //   categoryType: NMR-STAR file save frame category type, e.g.,
    //     "heteronuclear_NOE"
    //   loopTag: a tag in the data loop we want to save, e.g.
    //     "_Residue_seq_code"
    //   logOperand: loop value to take the logarithm of, if any (can be null)
    private void saveGeneric(String accNo, String categoryType,
      String loopTag, String suffix, String logOperand)
      throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("saveGeneric(" + categoryType + ")");
        }

        VectorCheckType saveFrames = getSaveFramesByTagValue(
	  S2DNames.SAVEFRAME_CATEGORY, categoryType);

        if (saveFrames.size() <= 0) {
	    throw new S2DException(categoryType + " save frame not found");
	}

        for (int index = 0; index < saveFrames.size(); index++) {
	    SaveFrameNode saveFrame =
	      (SaveFrameNode)saveFrames.elementAt(index);

	    String indexOut;
	    if (index == 0) {
	        indexOut = "";
	    } else {
	        indexOut = String.valueOf(index);
	    }
	    outputSaveFrame(saveFrame, accNo + suffix + indexOut + ".dat",
	      loopTag, logOperand);
        }
    }

    // ----------------------------------------------------------------------
    // Get a list of save frames (if any) with the specified tag/value
    // combination.
    private VectorCheckType getSaveFramesByTagValue(String tag, String value)
    {
        VectorCheckType result = new VectorCheckType();
	try {
	    //
	    // Set up the result VectorCheckType.
	    //
	    result.addType(Class.forName(
	      StarValidity.pkgName() + ".SaveFrameNode"));
	    result.freezeTypes();

	    //
	    // Find any DataItemNodes with the specified tag/value combo.
	    //
            VectorCheckType dataNodes = aStarTree.searchForTypeByTagValue(
	      Class.forName(StarValidity.pkgName() + ".DataItemNode"),
	      tag, value);

	    //
	    // Get the save frames for the DataItemNodes, and add them to the
	    // VectorCheckType that we will return.
	    //
            for (int index = 0; index < dataNodes.size(); index++) {
	        Class desiredClass = Class.forName(StarValidity.pkgName() +
	          ".SaveFrameNode");
		StarNode node = (StarNode)dataNodes.elementAt(index);
		while (node != null && node.getClass() != desiredClass) {
		    node = node.getParent();
		}

		if (node == null) {
		    throw new S2DException("Can't get save frame for " +
		      tag + ", " + value);
		}

	        SaveFrameNode saveFrame = (SaveFrameNode)node;
	        result.addElement(saveFrame);
            }
	} catch (Exception ex) {
	    System.err.println("Exception getting save frames(" + tag + ", " +
	      value + "): " + ex.getMessage());
	}

	return result;
    }

    // ----------------------------------------------------------------------
    // Output the given save frame.
    // Parameters:
    //   saveFrame: the save frame to output
    //   outFileName: the file to output to
    //   loopTag: a tag in the data loop we want to save, e.g.
    //     "_Residue_seq_code"
    //   logOperand: loop value to take the logarithm of, if any (can be null)
    private void outputSaveFrame(SaveFrameNode saveFrame, String outFileName,
      String loopTag, String logOperand)
      throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("outputSaveFrame(" + saveFrame.getLabel() +
	      ", " + outFileName + ", " + loopTag + ", " + logOperand + ")");
        }

	try {
	    //
	    // Find the data loop with the given tag in this save frame.
	    //
	    VectorCheckType tagList = saveFrame.searchByName(loopTag);
	    if (tagList.size() != 1) {
	        throw new S2DException(
		  "Should have found one tag; found " + tagList.size());
	    }

	    Class desiredClass = Class.forName(StarValidity.pkgName() +
	      ".DataLoopNode");
            StarNode node = (StarNode)tagList.elementAt(0);
	    while (node != null && node.getClass() != desiredClass) {
	        node = node.getParent();
	    }

	    if (node == null) {
	        throw new S2DException("Could not find data loop with tag " +
		  loopTag + " in save frame " + saveFrame.getLabel());
	    }

	    DataLoopNode dataLoop = (DataLoopNode)node;
	    LoopTableNode values = dataLoop.getVals();

	    if (logOperand != null) {
 		DataLoopNameListNode names = dataLoop.getNames();
		doLogarithm(logOperand, names, values);
	    }

	    //
	    // Output the data.
	    //
	    File outFile = new File(outFileName);
	    FileOutputStream outStream = 
	        new FileOutputStream(outFile);
	    StarUnparser theUnparser = new StarUnparser(outStream);
	
	    // Speed up computation time a bit.
	    theUnparser.setFormatting( false );

	    // Write out a LoopTableNode of values.
  	    theUnparser.writeOut(values, 0);

	    outStream.close();
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    throw new S2DException("Unable to process save frame " +
	      saveFrame.getLabel());
	} catch (Exception e) {
	    System.err.println("Exception: " + e.getMessage() );
	    throw new S2DException("Unable to process save frame " +
	      saveFrame.getLabel());
	}
    }

    // ----------------------------------------------------------------------
    // Take the logarithm of the given column in a table.
    // Parameters:
    //   operand: loop value to take the logarithm of, if any (can be null)
    //   names: the column names of the table
    //   values: the values of the table
    private void doLogarithm(String operand, DataLoopNameListNode names,
      LoopTableNode values)
      throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("doLogarithm(" + operand + ")");
        }

		// find value
		VectorCheckType operands =
		    names.searchByName(operand);

		if (operands.size() != 1)
		    throw new S2DException
			("Found "
			 + operands.size()
			 + " " + operand + " tags"
			 + ", expect 1.");
		
		int op_index = 
		    names.firstElement().indexOf 
		    ( ((DataNameNode) operands.firstElement()) );

		// now edit the values.  values is a LoopTableNode of
		// LoopRowNodes, each of which contains many
		// DataValueNodes.  We need to copy all the values and
		// create a new LoopRowNode, because we can't insert
		// values into row already in a DataLoopNode.
		Double origValue;

		for (int outer = 0; outer < values.size(); outer++) {
		    LoopRowNode row_copy = new LoopRowNode();
		    // copy all old elements
		    for (int inner=0; inner < values.elementAt(outer).size();
			 inner++) {
 			row_copy.insertElementAt
			    (values.elementAt(outer).elementAt(inner), inner);
		    }
		    
		    // convert value from a String to a Double object
		    if (row_copy.elementAt(op_index).getValue().equals("."))
			row_copy.addElement( new DataValueNode("."));
		    else {
			origValue = new Double(row_copy.elementAt(op_index)
					       .getValue() );
			
			// take the log, and convert back to a String
			row_copy.addElement( new DataValueNode
					     ((new Float
					       ((Math.log
						 (origValue.doubleValue() )
						 / Math.log(10.0) ) ))
					      .toString()));
		    }

		    // now replace the old value with the new one
		    values.setElementAt(row_copy, outer);
		}
    }
}
