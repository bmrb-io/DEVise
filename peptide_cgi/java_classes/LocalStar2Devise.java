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

// Reads a local star file and outputs data for DEVise.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

import java.io.*;
import EDU.bmrb.starlibj.*;
import java.util.Vector;

// for chemical shifts
import java.util.Enumeration;
import AssgDataManager.AssgEntry;
import ShiftDataManager.Pair;

public class LocalStar2Devise {

    private static final boolean DEBUG = true;
    private static final String RESIDUE_CODE = "_Residue_seq_code";

    private StarNode aStarTree;
    private String file_name;
    private Vector value_vector;
    
    // Location of RESIDUE_CODE tag in average_refined_structure,
    //   the first LoopTableNode in value_vector
    private static final int RESIDUE_LOCATION = 2;
    
    public LocalStar2Devise(String file_name) {
	try {
	    this.file_name = file_name;

	    StarParser aParser = 
		new StarParser(
			  new FileInputStream(file_name));
	    
	    aParser.StarFileNodeParse(aParser);
	    
	    // 'aParser' needs to be sent as a parameter because
	    // javacc makes static methods that have no 'this'.
	    aStarTree = aParser.endResult();

	    value_vector = new Vector();

	} catch (IOException e) {
	    System.err.println("IO Exception: "
			       + ": " + e.getMessage() );

	} catch (ParseException e) {
	    System.err.println("NMR-Star file parse error: " 
			       + e.getMessage() );
	}
    }


    /* Function outputData
     * Parameters:
     *   outFileName: the name of the output file, as ASCII text for DEVise
     */
    public boolean outputData (String outFileName) {
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
	    System.err.println("Unable to close file: " 
			       + e.getMessage() );
	}
	
	return false;		// search string not found
    }	


    /* Function processStrData
     * Parameters:
     *   saveFrameName: the name of the save frame to output
     *   outFileName: the name of the output file, as ASCII text for DEVise
     */
    public boolean processStrData( String saveFrameName, String outFileName ) {
	try {
	    
	    VectorCheckType saveFrameVec;
	    saveFrameVec = 
		aStarTree.searchByName(saveFrameName);
	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if (saveFrameVec.size() == 1) {
		// the last element should be a DataLoopNode
		// containing RESIDUE_CODE
		DataLoopNode lastLoop =
		    (DataLoopNode)
		    ( (SaveFrameNode) saveFrameVec.elementAt(0) )
		    .lastElement();
		// make sure that this is the correct DataLoopNode
		// this should find just one instance, because this
		// tag only appears once in the saveframe
		VectorCheckType checkForTag =
		    lastLoop.searchByName(RESIDUE_CODE);
// doesn't work for coupling constants
// 		if (checkForTag.size() != 1)
// 		    throw new IllegalArgumentException
// 			("Found "
// 			 + checkForTag.size()
// 			 + " " + RESIDUE_CODE + " tags"
// 			 + ", expect 1.");
		
		LoopTableNode values = lastLoop.getVals();
		

		// output the data
		File outFile = new File(outFileName);
		FileOutputStream outStream = 
		    new FileOutputStream(outFile);
		StarUnparser theUnparser = new StarUnparser(outStream);
		
		// speed up computation time a bit
		theUnparser.setFormatting( false );

		// writes out a LoopTableNode of values
  		theUnparser.writeOut(values, 0);
		outStream.close();

		return true;
		
	    } else if (saveFrameVec.size() == 0) // none found
		return false;

	    else {		// multiple search strings found
		throw new IllegalArgumentException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	    
	} catch (SecurityException e) {
	    System.err.println("Unable to open: " 
			       + e.getMessage() );
	} catch (IOException e) {
	    System.err.println("Unable to close file: " 
			       + e.getMessage() );
	}
	
	return false;		// search string not found
    } // end function processStrData()
    

    /* Function findBackbone
     * Parameters:
     *   saveFrameName: the name of the save frame to output
     *   outFileName: the name of the output file, as ASCII text for DEVise
     *
     * Pulls just the CA atoms out of a residue's structure
     */
    public boolean findBackbone (String saveFrameName, String outFileName) {
	// ** values?  doesn't work... just find again...

	try {
	    
	    VectorCheckType saveFrameVec;
	    saveFrameVec = 
		aStarTree.searchByName(saveFrameName);
	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if (saveFrameVec.size() == 1) {
		// the last element should be a DataLoopNode
		// containing RESIDUE_CODE
		DataLoopNode lastLoop =
		    (DataLoopNode)
		    ( (SaveFrameNode) saveFrameVec.elementAt(0) )
		    .lastElement();
		// make sure that this is the correct DataLoopNode
		// this should find just one instance, because this
		// tag only appears once in the saveframe
		VectorCheckType checkForTag =
		    lastLoop.searchByName(RESIDUE_CODE);

		if (checkForTag.size() != 1)
		    throw new IllegalArgumentException
			("Found "
			 + checkForTag.size()
			 + " " + RESIDUE_CODE + " tags"
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
			.getValue().equals("CA")) {
			
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
		throw new IllegalArgumentException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	    
	} catch (SecurityException e) {
	    System.err.println("Unable to open: " 
			       + e.getMessage() );
	} catch (IOException e) {
	    System.err.println("Unable to close file: " 
			       + e.getMessage() );
	}
	
	return false;		// search string not found
    } // end findBackbone




    /* Function addLogColumn
     * Parameters:
     *   inFileName: the name of the input file, in NMR-Star format
     *   operandName: the name of the field for which you wish to take the log.
     *   outFileName: the name of the output file, as ASCII text for DEVise
     */
    public boolean addLogColumn(String saveFrameName, String operandName, 
				String outFileName) {
	try {

	    VectorCheckType saveFrameVec;
	    saveFrameVec = 
		aStarTree.searchByName(saveFrameName);
	    
	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if (saveFrameVec.size() == 1) {
		// the last element should be a DataLoopNode
		// containing RESIDUE_CODE
		DataLoopNode valuableDataLoop =
		    (DataLoopNode)
		    ( (SaveFrameNode) saveFrameVec.firstElement() )
		    .lastElement();
		// make sure that this is the correct DataLoopNode
		// this should find just one instance, because this
		// tag only appears once in the saveframe
		VectorCheckType checkForTag =
		    valuableDataLoop.searchByName(RESIDUE_CODE);
		if (checkForTag.size() != 1)
		    throw new IllegalArgumentException
			("Found "
			 + checkForTag.size()
			 + " " + RESIDUE_CODE + " tags"
			 + ", expect 1.");
		
		LoopTableNode values = valuableDataLoop.getVals();
 		DataLoopNameListNode names = valuableDataLoop.getNames();

		// find value
		VectorCheckType operands =
		    names.searchByName(operandName);

		if (operands.size() != 1)
		    throw new IllegalArgumentException
			("Found "
			 + operands.size()
			 + " " + operandName + " tags"
			 + ", expect 1.");
		
		int op_index = 
		    names.firstElement().indexOf 
		    ( ((DataNameNode) operands.firstElement()) );

		// find error
// 		operands = names.searchByName(operandName + "_error");
		
// 		if (operands.size() != 1)
// 		    throw new IllegalArgumentException
// 			("Found "
// 			 + operands.size()
// 			 + " " + operandName + "_error tags"
// 			 + ", expect 1.");
		
// 		int op_err_index = 
// 		    names.firstElement().indexOf 
// 		    ( ((DataNameNode) operands.firstElement()) );

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

		    // do the same for error
// 		    if (row_copy.elementAt(op_err_index).getValue()
// 			.equals("."))
// 			row_copy.addElement( new DataValueNode("."));
// 		    else {
// 			origValue = new Double(row_copy.elementAt(op_err_index)
// 					       .getValue() );
			
// 			// take the log, and convert back to a String
// 			row_copy.addElement( new DataValueNode
// 					     ((new Float
// 					       ((Math.log
// 						 (origValue.doubleValue() )
// 						 / Math.log(10.0) ) ))
// 					      .toString()));
// 		    }

		    
		    // now replace the old value with the new one
		    values.setElementAt(row_copy, outer);
		}

		// output the data
		File outFile = new File(outFileName);
		FileOutputStream outStream = 
		    new FileOutputStream(outFile);
		StarUnparser theUnparser = new StarUnparser(outStream);
		
		// speed up computation time a bit
		theUnparser.setFormatting( false );
		
		// writes out a LoopTableNode of values
   		theUnparser.writeOut(values, 0);
		outStream.close();
		return true;
		
	    } else if (saveFrameVec.size() == 0) // none found
		return false;
	    
	    else {		// too many found
		throw new IllegalArgumentException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	    
	} catch (SecurityException e) {
	    System.err.println("Security exception: "
			       + e.getMessage() );
	} catch (IOException e) {
	    System.err.println("IO exception:  "
			       + e.getMessage() );
	}
    
	return false;		// search string not found
    } // end function addLogColumn()


    /* Function calcChemShifts
     * takes no parameters; instead, output based on the accession
     * number concatenated with c.dat, d.dat, or p.dat
     */
    public boolean calcChemShifts (String acc_num) {
	String CHEMSHIFT_FILE = "chemshift.txt";
	String CHEMASSG_FILE = "assignments.txt";
	String ERROR_LOG = "error.log";
	
	FileWriter deltashift_writer, csi_writer, percent_writer;
	String the_number = null;

	if (acc_num == null)
	    the_number = file_name.substring( 3, 7);
	else
	    the_number = acc_num;
	    
	try {
	    //Output error log
	    FileWriter error = new FileWriter(ERROR_LOG);
	    
//  	    System.out.println("Parsing the input file succeeded");
	    
	    //Create tagname and datavalue
	    String tagName = "_Saveframe_category";
	    String dataValue = "assigned_chemical_shifts";
	    String saveFrameName = "N/A";
	    
	    //Find list of all occurences in the star file
	    
	    //Initialize a list of ASTnode;
	    SaveFrameNode currSaveFrame;
	    
	    int chemShiftValueIndex, atomNameIndex, residueLabelIndex,
		residueSeqIndex, atomTypeIndex;
	    
	    // necessary for tagPositionDeep procedure
	    RemoteInt returnIndex = new RemoteInt();
	    RemoteInt nestLevel = new RemoteInt();
	    
	    Enumeration matches = aStarTree.searchForTypeByTagValue
		( Class.forName( StarValidity.pkgName()+".SaveFrameNode"),
		  tagName, 
		  dataValue ).elements();
	
	    //For each match only process if item is in a saveframelist node
	    while (matches.hasMoreElements())
	    {		
		DataLoopNode currentDataLoop;
		currSaveFrame = (SaveFrameNode)matches.nextElement();
		
		if( currSaveFrame != null )
		{
		    saveFrameName 
			= ((SaveFrameNode)currSaveFrame).getLabel();
		    
		    //Strip the save_
		    saveFrameName = saveFrameName.substring(5);
		} 
		else 
		{
		    saveFrameName = "N/A" ;
		}
	    
		//Now to obtain a pointer to the dataloop
		//containing the specific tag
		VectorCheckType loopMatches;
		loopMatches 
		    = currSaveFrame.searchForTypeByName
		    ( Class.forName( StarValidity.pkgName()
				     + ".DataLoopNode"),
		      "_Chem_shift_value" );
		
		//assert that this list has just 1 item in it!
		if( loopMatches.size() != 1 )
		{
		    throw new IllegalArgumentException
			( "Found " + loopMatches.size()
			  + " _Chem_shift_value tags, only expect 1." );
		}
	    
		currentDataLoop = (DataLoopNode)loopMatches.firstElement();
	    
		//And now retrieve chemical shift values
		currentDataLoop.getNames().tagPositionDeep
		    ( "_Chem_shift_value", 
		      nestLevel,
		      returnIndex);
		if( returnIndex.num == -1) 
		    break;
		else
		    chemShiftValueIndex = returnIndex.num;
		
		//Retrieve atom name
		currentDataLoop.getNames().tagPositionDeep
		    ( "_Atom_name", 
		      nestLevel,
		      returnIndex );
		if( returnIndex.num == -1 )
		    break;
		else
		    atomNameIndex = returnIndex.num;
		
		//Retrieve residue label
		currentDataLoop.getNames().tagPositionDeep
		    ( "_Residue_label",
		      nestLevel,
		      returnIndex );
		if( returnIndex.num == -1 )
		    break;
		else
		    residueLabelIndex = returnIndex.num;
		  
		//Retrieve sequence numbers
		currentDataLoop.getNames().tagPositionDeep
		    ( "_Residue_seq_code",
		      nestLevel,
		      returnIndex );
		if( returnIndex.num == -1 )
		    break;
		else
		    residueSeqIndex = returnIndex.num;

		//Retrieve atom type
		currentDataLoop.getNames().tagPositionDeep
		    ( "_Atom_type",
		      nestLevel,
		      returnIndex );
		if( returnIndex.num == -1 )
		    break;
		else
		    atomTypeIndex = returnIndex.num;
		
		//Do the shift computations here
		deltashift_writer = new FileWriter( the_number + "d.dat" );
		
		double currChemShiftValue;
		int currResSeqCode = -1,
		    prevResSeqCode = -1;
		
		//Define the object that manages data of the
		//chemical shift table
		ShiftDataManager refTable 
		    = new ShiftDataManager( CHEMSHIFT_FILE );
	    
		//Processing values in the loop
		int maxRows  = currentDataLoop.getVals().size();
		float ha_deltashift = 0, c_deltashift = 0, ca_deltashift = 0, 
		    cb_deltashift = 0;
		String prevAtomName = "uninitialized value";
		// prevResLabel = "uninitialized value",

		// get deltashift vals out first
		for (int currRowNum = 0; currRowNum < maxRows;
		     currRowNum++)
		{
		    
		    LoopRowNode currRow 
			= currentDataLoop.getVals().elementAt(currRowNum);
		    
		    currChemShiftValue
			= Double.parseDouble(currRow
					     .elementAt(chemShiftValueIndex)
					     .getValue());
		    
		    currResSeqCode
			= Integer.parseInt(currRow.elementAt(residueSeqIndex)
					   .getValue());
		    
		    String currAtomName 
			= currRow.elementAt(atomNameIndex).getValue();
		    
		    if (currResSeqCode != prevResSeqCode &&
			currResSeqCode != 1) 
		    { // assume code starts at 1
			
			deltashift_writer.write
			    (prevResSeqCode + " " + ha_deltashift + " "
			     + c_deltashift + " " + ca_deltashift + " "
			     + cb_deltashift + "\n");
			
			// re-initialize values
			ha_deltashift = 0;
			c_deltashift = 0;
			ca_deltashift = 0;
			cb_deltashift = 0;
			
			prevResSeqCode = currResSeqCode;
			prevAtomName = currAtomName;
			
		    } else if (currResSeqCode == 1) 
		    { // init data
			prevResSeqCode = currResSeqCode;
			prevAtomName
			    = currAtomName;
		    }
		     
		     String currResLabel 
			 = currRow.elementAt(residueLabelIndex).getValue();
		     
		     //Computes delta delta
		     Pair  standardValue = 
			 refTable.returnValues
			 (error, currResLabel, currAtomName);
		     
		     if(standardValue.chemshift > 0.0) {
			 float deltashift 
			     =  (float)(currChemShiftValue 
					- standardValue.chemshift);
			 
			 //The special cases of combining HA with HA2
			 //and HA3 with CB as per algorithm
			 if(currAtomName.compareTo("HA") == 0
			    || (currAtomName.compareTo("HA2") == 0
				&& currResLabel.compareTo("GLY") == 0))
			 {
			     ha_deltashift = deltashift;
			     prevAtomName = "HA";
			 } else if (currAtomName.compareTo("C") == 0)
			     c_deltashift = deltashift;
			 else if (currAtomName.compareTo("CA") == 0)
			     ca_deltashift = deltashift;
			 else if (currAtomName.compareTo("CB") == 0
				  || (currAtomName.compareTo("HA3") == 0
				      && currResLabel.compareTo("GLY") == 0))
			 {
			     cb_deltashift = deltashift;
			     prevAtomName = "CB";
			 }
			
		     } // end if chemshift value > 0.0
		    
		 } // for loop that iterates through this DataLoop
		
		// write the data for the last entry
		deltashift_writer.write
		    (prevResSeqCode + " " 
		     + ha_deltashift + " " + c_deltashift + " "
		     + ca_deltashift + " " + cb_deltashift + "\n");
		deltashift_writer.close();

//  		System.out.println("Finished deltashift vals.");

		// now get csi vals out
		int ha_csi = 0, c_csi = 0, ca_csi = 0, 
		    cb_csi = 0, cons_csi = 0;
		csi_writer = new FileWriter( the_number + "c.dat" );

		LineTokens currentLine 
		    = new LineTokens(new StreamTokenizer
				     (new FileReader
				      ( the_number + "d.dat" )));
		currentLine.readAndTokenLine();
		

		for (int currRowNum = 0; 
		     currRowNum < maxRows;
		     currRowNum++)
		{
		    LoopRowNode currRow
			= currentDataLoop.getVals().elementAt(currRowNum);
		
		    currChemShiftValue
			= Double.parseDouble(currRow
					     .elementAt(chemShiftValueIndex)
					     .getValue());
		      
		    currResSeqCode
			= Integer.parseInt(currRow
					   .elementAt(residueSeqIndex)
					   .getValue());

		    String currAtomName 
 			= currRow.elementAt(atomNameIndex).getValue();

		    if (currResSeqCode != prevResSeqCode &&
			currResSeqCode != 1) 
		    { // assume code starts at 1
			
			if (prevResSeqCode != currentLine.seqNumber)
 			    System.err.println("Consensus csi sequence number"
					       + " doesn't match that of "
 					       + "other sequence csi's!");
			
			csi_writer.write(prevResSeqCode + " " 
					 + ha_csi + " "
					 + c_csi + " "
					 + ca_csi + " "
					 + cb_csi + " "
					 + cons_csi + "\n");
			// re-initialize values
			ha_csi = 0;
			c_csi = 0;
			ca_csi = 0;
			cb_csi = 0;
			cons_csi = 0;

			prevResSeqCode = currResSeqCode;
			prevAtomName
			    = currAtomName;
			 currentLine.readAndTokenLine();
			 cons_csi = currentLine.csi;
			 
		    } else if (currResSeqCode == 1)
		    { // init data
			prevResSeqCode = currResSeqCode;
			prevAtomName
			    = currAtomName;
		    }
		    

 		    String currResLabel 
 			= currRow.elementAt(residueLabelIndex).getValue();

		    //Computes delta delta
		    Pair  standardValue = 
			refTable.returnValues(error,
					      currResLabel, currAtomName);
		    
		    if(standardValue.chemshift > 0.0) {
			float deltashift
			    =  (float)(currChemShiftValue 
				- standardValue.chemshift);
			  			  
			
			//And now to compute the CSI's
			int csi ;
			if(deltashift > standardValue.offset)
			    csi = 1;
			else if(deltashift < -1.0*standardValue.offset)
			    csi = -1;
			else
			    csi = 0;
		    
			//The special cases of combining HA with HA2
			//and HA3 with CB as per algorithm
			if(currAtomName.compareTo("HA") == 0
			   || (currAtomName.compareTo("HA2") == 0
			       && currResLabel.compareTo("GLY") == 0))
			{
			    ha_csi = csi;
			    prevAtomName = "HA";
			} else if (currAtomName.compareTo("C") == 0)
			    c_csi = csi;
			else if (currAtomName.compareTo("CA") == 0)
			    ca_csi = csi;
			else if (currAtomName.compareTo("CB") == 0
				 || (currAtomName.compareTo("HA3") == 0
				     && currResLabel.compareTo("GLY") ==0))
			{
			    cb_csi = csi;
			    prevAtomName = "CB";
			}

		    } // if chemshift value > 0.0
		    
		}

		// write out the last entry		
		csi_writer.write(prevResSeqCode + " " 
				 + ha_csi + " "
				 + c_csi + " "
				 + ca_csi + " "
				 + cb_csi + " "
				 + cons_csi + "\n");
		csi_writer.close();
//  		System.out.println("Finished csi.");

		//Do the atom assignment computation here
		int starnumH, starnumC, starnumN, numH, numC, numN;
		  
		percent_writer = new FileWriter( the_number + "p.dat" );
		
		AssgDataManager assgTable 
		    = new AssgDataManager( CHEMASSG_FILE );
		
		int currRowNum = 0;
		while (currRowNum < maxRows)
		    {
		    LoopRowNode currRow 
			= currentDataLoop.getVals().elementAt(currRowNum);
		    
		    //Process a block with same residue label and
		    //sequence code
		    String currResidueLabel
			= currRow.elementAt(residueLabelIndex).getValue();
		    String currSeqNum
			= currRow.elementAt(residueSeqIndex).getValue();

 		      starnumH = 0;
 		      starnumC = 0;
 		      starnumN = 0;
		      
		      AssgEntry assignments =
			  assgTable.returnAssg(error, currResidueLabel);
		      numH = assignments.numH;
		      numC = assignments.numC;
		      numN = assignments.numN;
		      
 		      if(numH != -1 && numC != -1 && numN != -1)
		      {
 			  //Only if an entry exists in the chemical
 			  //assignment table file
			  while (currRowNum < maxRows &&
				 currResidueLabel.equals(
				    currRow.elementAt(residueLabelIndex)
				    .getValue()) &&
				 currSeqNum.equals(
				      currRow.elementAt(residueSeqIndex)
				      .getValue()))
			  {			      
			      String new_atom =
				  currRow.elementAt(atomTypeIndex).getValue();
			      
			      if (new_atom.equals("H"))
				  starnumH++;
			      else if (new_atom.equals("C"))
				  starnumC++;
			      else if (new_atom.equals("N"))
				  starnumN++;

			      if (++currRowNum < maxRows)
				  currRow
				      = currentDataLoop.getVals()
				      .elementAt(currRowNum);
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
//  		System.out.println("Finished percent assignments.");
	    } // end while matches.hasMoreElements()
	    
	    error.close();
	    return true;
	} catch (IOException e)
	{
	    System.err.println("IO Exception: "
                               + e.getMessage() );
	} catch (ClassNotFoundException e)
	{
	    System.err.println("Class not found exception: "
                               + e.getMessage() );
	}

	return false;
    }


    /* Function sequence
       Determine the amino acid sequence?
     */
//     public boolean sequence( String outFileName) {
// 	// cf processstrdata
// 	return false;
//     }

    /* Function summarize
     */
    public boolean summarize( String outFileName ) {
	try {
	    VectorCheckType saveFrameVec =
		aStarTree.searchByName( "save_entry_information" );

	    // this vector should have size 1 because there should
	    // only be one SaveFrame with this name
	    if ( saveFrameVec.size() != 1 ) {
		throw new IllegalArgumentException
		    ("Found " + saveFrameVec.size()
		     + " save frames for save_entry_information;"
		     + " expect exactly 1.");
	    }
	    
	    // This finds the Entry Title
	    String title =
		((DataItemNode) ((SaveFrameNode) saveFrameVec.firstElement())
				 .searchByName("_Entry_title")
		 .firstElement()).getValue();

	    String system_name =
		((DataItemNode) aStarTree.searchByName("_Mol_system_name")
		 .firstElement()).getValue();

	    VectorCheckType category_vec =
		aStarTree.searchForTypeByName
		( Class.forName( StarValidity.pkgName()+".DataLoopNode"),
		  "_Saveframe_category_type" );

	    if ( category_vec.size() != 1 ) {
		throw new IllegalArgumentException
		    ("Found " + saveFrameVec.size()
		     + " save frames for _Saveframe_category_type,"
		     + " expect exactly 1.");
	    }

	    DataLoopNode category_loop = 
		((DataLoopNode) category_vec.firstElement() );

	    LoopTableNode category_vals =
		((DataLoopNode) category_vec.firstElement() ).getVals();

	    // generate a web page here
	    String the_number = null;
	    the_number = outFileName;

//   	    FileOutputStream outStream 
// 		= new FileOutputStream(the_number + "y.html");
//  	    PrintWriter summary_writer 
//  		= new PrintWriter(new BufferedWriter
//  				  ( new OutputStreamWriter ( outStream ))) ;
// 	    summary_writer.println("<html><head><title>Summary for " +
// 				   file_name + "</title></head>" + 
// 				   "<body bgcolor = white>");
// 	    summary_writer.println("<h3>DEVise plots for:");
// 	    summary_writer.println(system_name + "</h3>");
// 	    summary_writer.println("Title: <tt>" + title + "</tt>");
	    
	    // new display- pages generated my make_view
// 	    StringBuffer display_link =
// 		new StringBuffer
// 		("<br><br><a href=\"" + the_number + "c.html");
	    
	    // DataLoopNode contains DataLoopNameListNode, and LoopTableNode
 	    for ( int i = 0; i < category_vals.size(); i++) {
 		String current_tag =
 		    category_vals.elementAt(i).firstElement().getValue();
// 		if (current_tag.equals("assigned_chemical_shifts")) {
		    
		    // generate the files
// 		    calcChemShifts(null);

		    // and create links
// 		    summary_writer.print(display_link);
// 		    summary_writer.print("\">Chemical Shift Index</a>");

// 		    display_link.setCharAt(21, 'd');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print
// 			("\">Chemical Shift Delta</a>");

// 		    display_link.setCharAt(21, 'p');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print
// 			("\">Percent Assigned Atoms</a>");
 		    
// 		} 
		
		if (current_tag.equals("T1_relaxation")) {

		    // generate the files
		    if (! processStrData("save_T1_750", 
						  the_number + "o.dat") )
			System.err.println
			    ("T1 relaxation save frame not found.");
	
		    if (! processStrData("save_T2_750", 
						  the_number + "t.dat") )
			System.err.println
			    ("T2 750 relaxation save frame not found.");
	
		    if (! processStrData("save_T2_600",
						  the_number + "u.dat") )
			System.err.println
			    ("T2 600 relaxation save frame not found.");

		    if (! processStrData("save_heteronuclear_NOE_750",
						  the_number + "n.dat") )
			System.err.println
			    ("Heteronuclear NOE save frame not found.");

		    if (! addLogColumn("save_H_exch_rates",
				       "_H_exchange_rate_value",
				       the_number + "r.dat") )
			System.err.println
			    ("H-exchange rates save frame not found.");


		    if (! processStrData("save_HNHA_J_values",
					 the_number + "g.dat") )
			System.err.println
			    ("Coupling constants save frame not found.");


		    // we want to plot the log of the next value
		    if (! addLogColumn
			("save_protection_factors_label",
			 "_H_exchange_protection_factor_value",
			 the_number + "f.dat") )
			System.err.println
			    ("H-exchange protection factors save "
			     + "frame not found.");

		    if (! processStrData("save_S2", 
						  the_number + "s.dat") )
			System.err.println("Kinetic order parameters (S2)" +
					   "save frame not found.");

		    // and create the links
// 		    display_link.setCharAt(21, 'h');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print("\">H-Exchange Rates</a>");

// 		    display_link.setCharAt(21, 'o');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print("\">Order Parameters</a>");

// 		    display_link.setCharAt(21, 'r');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print("\">Relaxation Parameters</a>");

// 		    display_link.setCharAt(21, 'g');
// 		    summary_writer.print(display_link);
// 		    summary_writer.print("\">Coupling Constants</a>");
		}
	    }
    
// 	    summary_writer.print("</body></html>");

	    // to print out a DataLoopNode
// 	    DataLoopNode categoryLoop =
// 		(DataLoopNode)
// 		(( (SaveFrameNode) categoryVec.elementAt(0) )
// 		 .lastElement());

//   	    StarUnparser theUnparser = new StarUnparser(outStream);

// 	    // speed up computation time a bit
// 	    theUnparser.setFormatting( false );
	    
// 	    // writes out a LoopTableNode of values
//   	    theUnparser.writeOut((DataLoopNode)categoryVec.firstElement(), 0);
//    	    theUnparser.writeOut(category_vals, 0);

// 	    summary_writer.println("</body></html>");

// 	    summary_writer.close();
//  	    outStream.close();
	    
// 	    VectorCheckType chemshiftVec =
// 		categoryLoop.searchForTypeByName
// 		( Class.forName( StarValidity.pkgName()+".LoopRowNode"),
// 		  "assigned_chemical_shifts" );

// 	    //assert that this list has just 1 item in it!
// 	    if( chemshiftVec.size() == 1 )
// 	    {
// 		int assg_chem_shift_count =
// 		    Integer.parseInt( ((LoopRowNode) chemshiftVec
// 	            .firstElement()).elementAt(1).getValue() );
// 		System.out.println("Chem shift count = " +
// 				   assg_chem_shift_count);
// // 		return true;
// 	    }

	    // Now check for other data...
    
	    return true;

        	} catch (ClassNotFoundException e) {
 	    System.err.println("Class not found exception:  "
 			       + e.getMessage() );
//         	} catch (FileNotFoundException e) {
//  	    System.err.println("File not found exception:  "
//  			       + e.getMessage() );
//         	} catch (IOException e) {
//  	    System.err.println("IO exception:  "
//  			       + e.getMessage() );
 	} finally {}

 	return false;
    } // end summarize


    /* Function sqaure
     *  takes a Float, and squares the number
     */
    public float square(float x) {
	return x*x;
    }


    /* Function countConstraints
     *
     *  Count the number of distance constraints for each residue
     */

    public boolean countConstraints(String the_number) {

	int[] constraint_count = null;
	float[] rms;		// for later

	// find number of residues
	VectorCheckType saveFrameVec;
	saveFrameVec = 
	    aStarTree.searchByName("save_GMG4");
	if (saveFrameVec.size() == 0)
	    return false;
	else if (saveFrameVec.size() != 1)
	    throw new IllegalArgumentException
		("In countConstraints, found " + saveFrameVec.size()
		 + " save frames, expect exactly 1.");

	int num_residues =
	    Integer.parseInt
	    (((DataItemNode)
	       ( ((SaveFrameNode) saveFrameVec.firstElement())
		 .searchByName("_Residue_count") ).firstElement() )
	      .getValue());
	
	if (DEBUG)
	    System.out.println(" " + num_residues + " residues.");

	// count the distance constraints per residue
	saveFrameVec =
	    aStarTree.searchByName("save_distance_constraints");
	 
	if (saveFrameVec.size() == 0) // none found
	    return false;
	
	else if (saveFrameVec.size() != 1) { // search strings found
	    throw new IllegalArgumentException
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
		    .equals("HEME")) {
		    
		    if (! lastLoop.elementAt(id).elementAt(4).equals("HEME")) {
			
			constraint_count[Integer.parseInt(second_residue)]++;
		    }

		} else {
		    constraint_count[Integer.parseInt(first_residue)]++;

		if (! first_residue.equals(second_residue)
		    && ! lastLoop.elementAt(id).elementAt(4).getValue()
		    .equals("HEME"))
		    
		    constraint_count[Integer.parseInt(second_residue)]++;
		}
	    
	    } // end loop through all distance constraints
	    
	} // end if exactly one save frame found

	// calculate root mean squared deviation for CA atom coordinates
	float[][] avg_coords = new float[num_residues + 1][3];
	saveFrameVec =
	    aStarTree.searchByName
	    ("save_conformer_statistical_characteristics");


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
	    ("save_conformer_statistical_characteristics");
	
	if (saveFrameVec.size() == 0) // none found
	    return false;

	else if (saveFrameVec.size() != 1) { // search strings found
	    throw new IllegalArgumentException
		("Found " + saveFrameVec.size()
		 + " save frames,"
		 + " expect exactly 1.");
	}
	
	float[][] total_error = new float[num_residues + 1][3];
	int num_conformers =
	    Integer.parseInt
	    (((DataItemNode)
	      ( ((SaveFrameNode) saveFrameVec.firstElement())
		.searchByName("_Conformer_submitted_total_number") )
	      .firstElement() )
	     .getValue());
	
// 	if (DEBUG)
// 	    System.out.println(" " + num_conformers + " conformers");
	
	saveFrameVec =
	    aStarTree.searchByName("save_GMH4CO_conformer_family");
	
	if (saveFrameVec.size() == 0) // none found
	    return false;

	else if (saveFrameVec.size() != 1) { // search strings found
	    throw new IllegalArgumentException
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
	    if (DEBUG)
		System.out.println(" Read " + num_conformers +
				   " conformers into memory");

// 		    // update avgs
// 		    int res_code = 
// 			Integer.parseInt(curr_row.elementAt(3).getValue());
// 		    for (int dir = 0; dir < 3; dir++) {
// 			total_error[res_code][dir] += 
// 			    square(Float.parseFloat(curr_row
// 						    .elementAt(7 + dir)
// 						    .getValue()));

	    
	    // finish calculating averages
	    // calculate RMS
	    rms = new float[num_residues + 1];

	    for (int rcount = 1; rcount < num_residues + 1; rcount++) {
		for (int dir = 0; dir < 3; dir++) {
		    avg_coords[rcount][dir] /= (float)num_conformers;
		    rms[rcount] += square(avg_coords[rcount][dir]);
// 		    if (DEBUG)
// 			System.out.println(" avg[" + rcount + "][" + dir +
// 					   "] = " + avg_coords[rcount][dir]);
		}
	    }

	    
	    for (int rcount = 1; rcount < num_residues + 1; rcount++) {
		rms[rcount] = 
                  	(float)Math.sqrt(rms[rcount]) / num_conformers;
// 		if (DEBUG)
// 		    System.out.println(" RMS[" + rcount + "] = " +
// 				       rms[rcount]);
	    }
	    
	} // end if exactly one save frame with this name

	try {
	    PrintWriter out_writer = 
		new PrintWriter
		(new BufferedWriter
		 (new FileWriter(the_number + "i.dat")));
		    
	    for (int outcount = 1; 
		 outcount < num_residues + 1; 
		 outcount++) {
		out_writer.println(outcount + " " + 
				   constraint_count[outcount] +
				   rms[outcount]);
	    }
		    
	    out_writer.close();
		
	} catch (IOException e) {
	    System.err.println("IO Exception: "
			       + ": " + e.getMessage() );
	}

	return true;
    }
    
    protected void finalize() throws Throwable {
	super.finalize();
    }

}
