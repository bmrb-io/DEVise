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
    private static final String RESIDUE_CODE = "_Residue_seq_code";

    private StarNode aStarTree;
    private String file_name;
    private Vector value_vector;
    
    // Location of RESIDUE_CODE tag in average_refined_structure,
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
	      new java.net.URL("http://www.bmrb.wisc.edu/data_library/files/" +
	      file_name);
	    s2d = new Star2Devise(file_name, starfile.openStream());
	} catch(java.io.IOException ex) {
	    System.err.println("Unable to open or read " + ex.getMessage());
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
	    throw new S2DException("Unable to parse " + file_name);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
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
	    throw new S2DException("Unable to output data for " + file_name);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    throw new S2DException("Unable to output data for " + file_name);
	}
    }


    // ----------------------------------------------------------------------
    /* Function processStrData
     * Parameters:
     *   saveFrameName: the name of the save frame to output
     *   outFileName: the name of the output file, as ASCII text for DEVise
     * Return value: true iff data is successfully processed; false otherwise.
     */
    public boolean processStrData( String saveFrameName, String outFileName )
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("Star2Devise.processStrData(" +
	      saveFrameName + ", " + outFileName + ")");
	}

	try {
	    
	    VectorCheckType saveFrameVec =
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
// 		    throw new S2DException
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
		
	    } else if (saveFrameVec.size() == 0) { // none found
		return false;

	    } else {		// multiple search strings found
		throw new S2DException("Found " +
		  saveFrameVec.size() + " " + saveFrameName +
		  " save frames," + " expect exactly 1.");
	    }
	    
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    throw new S2DException("Unable to process save frame " +
	      saveFrameName);
	} catch (Exception e) {
	    System.err.println("Exception: " + e.getMessage() );
	    throw new S2DException("Unable to process save frame " +
	      saveFrameName);
	}
    } // end function processStrData()
    

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
		    throw new S2DException
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
		throw new S2DException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	    
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    throw new S2DException("Unable to find backbone for save frame " +
	      saveFrameName);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    throw new S2DException("Unable to find backbone for save frame " +
	      saveFrameName);
	}
    } // end findBackbone


    // ----------------------------------------------------------------------
    /* Function addLogColumn
     * Parameters:
     *   inFileName: the name of the input file, in NMR-Star format
     *   operandName: the name of the field for which you wish to take the log.
     *   outFileName: the name of the output file, as ASCII text for DEVise
     * Return value: true iff column is added; false otherwise.
     */
    public boolean addLogColumn(String saveFrameName, String operandName,
      String outFileName) throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("Star2Devise.addLogColumn(" +
	      saveFrameName + ", " + operandName + ", " + outFileName + ")");
	}

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
		    throw new S2DException
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
		    throw new S2DException
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
// 		    throw new S2DException
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
		throw new S2DException
		    ("Found " + saveFrameVec.size()
		     + " save frames,"
		     + " expect exactly 1.");
	    }
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    throw new S2DException(
	      "Unable to find add log column for save frame " +
	      saveFrameName);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    throw new S2DException(
	      "Unable to find add log column for save frame " +
	      saveFrameName);
	}
    } // end function addLogColumn()


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
	    final String tagName = "_Saveframe_category";
	    final String dataValue = "assigned_chemical_shifts";
	    
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
		    saveFrameName = saveFrameName.substring("save_".length());
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
		      "_Chem_shift_value" );
		
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
	    throw new S2DException(
	      "Unable to find calculate chem shifts " + acc_num);
	} catch (IOException e) {
	    System.err.println("IOException: " + e.getMessage() );
	    throw new S2DException(
	      "Unable to find calculate chem shifts " + acc_num);
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.getMessage() );
	    throw new S2DException(
	      "Unable to find calculate chem shifts " + acc_num);
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
	_currentDataLoop.getNames().tagPositionDeep("_Chem_shift_value",
	  nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for chemical shift values");
	} else {
	    _chemShiftValueIndex = returnIndex.num;
	}
		
	//Retrieve atom name
	_currentDataLoop.getNames().tagPositionDeep("_Atom_name", nestLevel,
	  returnIndex );

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for atom names");
	} else {
	    _atomNameIndex = returnIndex.num;
	}
		
	//Retrieve residue label
	_currentDataLoop.getNames().tagPositionDeep("_Residue_label",
	  nestLevel, returnIndex );

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for residue labels");
	} else {
	    _residueLabelIndex = returnIndex.num;
	}
		  
	//Retrieve sequence numbers
	_currentDataLoop.getNames().tagPositionDeep("_Residue_seq_code",
	  nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DException("Can't get index for sequence numbers");
	} else {
	    _residueSeqIndex = returnIndex.num;
        }

	//Retrieve atom type
	_currentDataLoop.getNames().tagPositionDeep("_Atom_type", nestLevel,
	  returnIndex);

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
	FileWriter deltashift_writer = new FileWriter( _accNum + "d.dat" );
		
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
	    
	    _currChemShiftValue
		= Double.parseDouble(currRow
				     .elementAt(_chemShiftValueIndex)
				     .getValue());
	    
	    _currResSeqCode
		= Integer.parseInt(currRow.elementAt(_residueSeqIndex)
				   .getValue());
	    
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
	// now get csi vals out
	int ha_csi = 0, c_csi = 0, ca_csi = 0, 
	    cb_csi = 0, cons_csi = 0;

	FileWriter csi_writer = new FileWriter( _accNum + "c.dat" );

	LineTokens currentLine 
	    = new LineTokens(new StreamTokenizer
			     (new FileReader
			      ( _accNum + "d.dat" )));
	currentLine.readAndTokenLine();
	
	int maxRows  = _currentDataLoop.getVals().size();
	for (int currRowNum = 0; currRowNum < maxRows; currRowNum++)
	{
	    LoopRowNode currRow
		= _currentDataLoop.getVals().elementAt(currRowNum);
	
	    _currChemShiftValue
		= Double.parseDouble(currRow
				     .elementAt(_chemShiftValueIndex)
				     .getValue());
	      
	    _currResSeqCode
		= Integer.parseInt(currRow
				   .elementAt(_residueSeqIndex)
				   .getValue());

	    String currAtomName 
 		= currRow.elementAt(_atomNameIndex).getValue();

	    if (_currResSeqCode != _prevResSeqCode &&
		_currResSeqCode != 1) 
	    { // assume code starts at 1
		
		if (_prevResSeqCode != currentLine.seqNumber) {
 		    System.err.println("Consensus csi sequence number"
				       + " doesn't match that of "
 				       + "other sequence csi's!");
		}
		
		csi_writer.write(_prevResSeqCode + " " 
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

		_prevResSeqCode = _currResSeqCode;
		_prevAtomName = currAtomName;
		currentLine.readAndTokenLine();
		cons_csi = currentLine.csi;
			 
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
		_refTable.returnValues(_error,
				      currResLabel, currAtomName);
	    
	    if(standardValue.chemshift > 0.0) {
		float deltashift
		    =  (float)(_currChemShiftValue 
			- standardValue.chemshift);
			  			  
			
		//And now to compute the CSI's
		int csi ;
		if (deltashift > standardValue.offset) {
		    csi = 1;
		} else if(deltashift < -1.0*standardValue.offset) {
		    csi = -1;
		} else {
		    csi = 0;
		}
		    
		//The special cases of combining HA with HA2
		//and HA3 with CB as per algorithm
		if(currAtomName.compareTo("HA") == 0
		   || (currAtomName.compareTo("HA2") == 0
		       && currResLabel.compareTo("GLY") == 0))
		{
		    ha_csi = csi;
		    _prevAtomName = "HA";
		} else if (currAtomName.compareTo("C") == 0) {
		    c_csi = csi;
		} else if (currAtomName.compareTo("CA") == 0) {
		    ca_csi = csi;
		} else if (currAtomName.compareTo("CB") == 0
			 || (currAtomName.compareTo("HA3") == 0
			     && currResLabel.compareTo("GLY") ==0))
		{
		    cb_csi = csi;
		    _prevAtomName = "CB";
		}

	    } // if chemshift value > 0.0
    
	}

	// write out the last entry		
	csi_writer.write(_prevResSeqCode + " " 
			 + ha_csi + " "
			 + c_csi + " "
			 + ca_csi + " "
			 + cb_csi + " "
			 + cons_csi + "\n");
	csi_writer.close();
//  	System.out.println("Finished csi.");
    }

    // ----------------------------------------------------------------------
    // Write out the percent assignment info.
    private void writeAssignments() throws IOException, S2DException
    {
	//Do the atom assignment computation here
	int starnumH, starnumC, starnumN, numH, numC, numN;
	  
	FileWriter percent_writer = new FileWriter( _accNum + "p.dat" );
	
        final String CHEMASSG_FILE = "assignments.txt";
	AssgDataManager assgTable 
	    = new AssgDataManager( CHEMASSG_FILE );
	
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
    /* Function sequence
       Determine the amino acid sequence?
     */
//     public boolean sequence( String outFileName) {
// 	// cf processstrdata
// 	return false;
//     }

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
		aStarTree.searchByName( "save_entry_information" );
	    if (DEBUG >= 2) {
	        System.out.println("  summarizing save_entry_information");
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
		  .searchByName("_Entry_title") .firstElement()).getValue();
            } catch(Exception ex) {
	        System.err.println("Exception getting title: " +
		  ex.getMessage());
	    }

            String system_name = "unknown";
	    try {
	        system_name = ((DataItemNode)
		  aStarTree.searchByName("_Mol_system_name")
		  .firstElement()).getValue();
            } catch(Exception ex) {
	        System.err.println("Exception getting system name: " +
		  ex.getMessage());
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

	    LoopTableNode category_vals =
		((DataLoopNode) category_vec.firstElement() ).getVals();

	    // generate a web page here
	    String the_number = null;
	    if (outFileName == null) {
	        the_number = S2DUtils.starName2Num(file_name);
	    } else {
	        the_number = outFileName;
	    }
  	    FileOutputStream outStream 
		= new FileOutputStream(the_number + "y.html");
 	    PrintWriter summary_writer 
 		= new PrintWriter(new BufferedWriter
 				  ( new OutputStreamWriter ( outStream ))) ;
	    summary_writer.println("<html><head><title>Summary for " +
				   file_name + "</title></head>" + 
				   "<body bgcolor = white>");
	    summary_writer.println("<h3>DEVise plots for:");
	    summary_writer.println(system_name + "</h3>");
	    summary_writer.println("Title: <tt>" + title + "</tt>");
	    
	    // new display- pages generated my make_view
	    String display_link_base = "<br><br><a href=\"" + the_number;
	    String display_link = display_link_base + "c.html";
	    
	    // DataLoopNode contains DataLoopNameListNode, and LoopTableNode
	    for ( int i = 0; i < category_vals.size(); i++) {
		String current_tag =
		    category_vals.elementAt(i).firstElement().getValue();
		if (current_tag.equals("assigned_chemical_shifts")) {
	            if (DEBUG >= 2) {
	                System.out.println(
			  "  summarizing assigned_chemical_shifts");
	            }
		    
		    // generate the files
		    calcChemShifts(null);

		    // and create links
		    summary_writer.print(display_link);
		    summary_writer.print("\">Chemical Shift Index</a>");

	            display_link = display_link_base + "d.html";
		    summary_writer.print(display_link);
		    summary_writer.print
			("\">Chemical Shift Delta</a>");

	            display_link = display_link_base + "p.html";
		    summary_writer.print(display_link);
		    summary_writer.print
			("\">Percent Assigned Atoms</a>");
		    
		} 
		
		if (current_tag.equals("T1_relaxation")) {
	            if (DEBUG >= 2) {
	                System.out.println(
			  "  summarizing T1_relaxation");
	            }

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
	            display_link = display_link_base + "h.html";
		    summary_writer.print(display_link);
		    summary_writer.print("\">H-Exchange Rates</a>");

	            display_link = display_link_base + "o.html";
		    summary_writer.print(display_link);
		    summary_writer.print("\">Order Parameters</a>");

	            display_link = display_link_base + "r.html";
		    summary_writer.print(display_link);
		    summary_writer.print("\">Relaxation Parameters</a>");

	            display_link = display_link_base + "g.html";
		    summary_writer.print(display_link);
		    summary_writer.print("\">Coupling Constants</a>");
		}
	    }

	    summary_writer.print("</body></html>");

// loop_
// _Saveframe_category_type 
// _Saveframe_category_type_count 
// coupling_constants 1 
// T1_relaxation 1 
// T2_relaxation 2 
// heteronuclear_NOE 1 
// S2_parameters 1 
// H_exchange_rate 1 
// H_exchange_protection_factors 1 
// representative_structure 1 
// structure_coordinate_set 1 
// stop_


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

	    summary_writer.println("</body></html>");



	    summary_writer.close();
 	    outStream.close();



	    
// 	    VectorCheckType chemshiftVec =
// 		categoryLoop.searchForTypeByName
// 		( Class.forName( StarValidity.pkgName()+".LoopRowNode"),
// 		  "assigned_chemical_shifts" );

// 	    //assert that this list has just 1 item in it!
// 	    if( chemshiftVec.size() == 1 )
// 	    {
// 		int assg_chem_shift_count =
// 		    new Integer( ((LoopRowNode) chemshiftVec.firstElement())
// 				 .elementAt(1).getValue() ).intValue();
// 		System.out.println("Chem shift count = " +
// 				   assg_chem_shift_count);
// // 		return true;
// 	    }

	    // Now check for other data...
	    
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
	    aStarTree.searchByName("save_GMG4");

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
		 .searchByName("_Residue_count") ).firstElement() )
	      .getValue());
	
	if (DEBUG >= 2) {
	    System.out.println(" " + num_residues + " residues.");
        }

	// count the distance constraints per residue
	saveFrameVec =
	    aStarTree.searchByName("save_distance_constraints");
	 
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
		.searchByName("_Conformer_submitted_total_number") )
	      .firstElement() )
	     .getValue());
	
 	if (DEBUG >= 3) {
 	    System.out.println(" " + num_conformers + " conformers");
	}
	
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

}
