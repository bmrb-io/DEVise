// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002
// By the DEVise Development Group 
// Madison, Wisconsin
// All Rights Reserved. 
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise 
// Development Group.

// ------------------------------------------------------------------------

// This class contains methods for converting an NMR-Star file to 
// a ClassAds File. It receives a StarNode as input.

// ------------------------------------------------------------------------

// $Id$

// 2002/09/10 Patrícia Kayser - kayser@cos.ufrj.br
// Third version: only small changes to allow multiple columns in
// save frame constraints and "-nl" and "-al" options

// 2002/08/28  Patrícia Kayser - kayser@cos.ufrj.br
// First version: translates all input file without allowing constraints
// about loops.

// ------------------------------------------------------------------------

package bmrb.star2ad;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;

public class Star2AdTranslator{
    //===================================================================
    // VARIABLES
    private StarNode aStarTree;
    private BufferedWriter  outputFile;
    private final boolean DEBUGING   = true;
    private final boolean DEBUGING_1 = false;
    private String debugingOutStream = "";
    //private final String TAB = "\t";
    private final String TAB = "   ";

    // Variable ALL is used in searchForType method as its second
    //  parameter (negative value means we want all the DataValueNodes
    //  regardless of their delimiter type)
    private final short ALL = -1;

    private Star2AdConstraintManager constraintMng = null;
    private boolean checkConstraints = false;
    
    
    //===================================================================
    // CONSTRUCTOR METHOD
    public Star2AdTranslator(StarNode aStarTree, String fileName){
	this.aStarTree = aStarTree;

        try {
	    this.outputFile = new BufferedWriter(
                                  new FileWriter(fileName));
	} catch (IOException e) {
	    System.out.println("<NMR-STAR2ClassAd>\tError open output"+
                               " file "+ fileName);
	}    
    }

    //===================================================================
    // PUBLIC METHODS
    
    //-------------------------------------------------------------------
    public void setConstraintMng(Star2AdConstraintManager constraintMng) {
        this.constraintMng = constraintMng;
    }

    //-------------------------------------------------------------------
    public void start(){
	System.out.println("<NMR-STAR2ClassAd>\tTranslation."); 
	print2Output("[\n");
	getSaveFrameNodes();
	print2Output("]");
	this.stop();
    }


    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    private void print2Output(String s){
	if (DEBUGING){
	    debugingOutStream = debugingOutStream+s;
	}     
	try {
	    outputFile.write(s);
	} catch (IOException e) {
	    System.out.println("<NMR-STAR2ClassAd>\tError writing"+
			       " to output file");
	}    
    }

    //-------------------------------------------------------------------
    private String getSaveFrameName(SaveFrameNode saveFrame){
	//
	// A SaveFrameNode contains the contents of a STAR file 
	// saveframe. It has a name, available through "getLabel()" 
	// and "setLabel()". This name must have a prefix of "save_". 
	// It behaves like a vector (it implements StarVectorLike) 
	// that can contain DataItemNodes and DataLoopNodes.
	if (DEBUGING_1) {
	    System.out.println("<NMR-STAR2ClassAd>\t"+
                               saveFrame.getLabel());
	}
        
        String saveFrameName = saveFrame.getLabel();
	// print2Output(TAB+TAB+"Name = \""+saveFrameName+"\";\n");
        
        checkConstraints = constraintMng.hasConstraints(saveFrameName);
        if (checkConstraints) {
            System.out.println("CONSTRAINTS on "+saveFrameName);
        } else {
            System.out.println("NONE CONSTRAINT on "+saveFrameName);
        }
        
        return saveFrameName;
    }


    //-------------------------------------------------------------------
    private boolean isDataItem(StarNode node){
	return ((node.getClass()).getName()).equals("EDU.bmrb.starlibj.DataItemNode");
    }

    //-------------------------------------------------------------------
    private boolean isNumber(String s){
	try {
	    float f = Float.parseFloat(s);
	    return true;
	} catch (NumberFormatException e) {
	    return false;
	}

    }

    //-------------------------------------------------------------------
    private void getNameValuePairs(DataItemNode dataItem, String space){
	print2Output(space+dataItem.getLabel()+" = ");

	// removes CR/LF from string values
	String value = dataItem.getValue();

	if (isNumber(value)) {
	    print2Output(value+";\n");
	} else {
	    value = value.replace('\n',' ');

	    print2Output("\""+value+"\";\n");
	}
    }



    //-------------------------------------------------------------------
    private void getRowOutput(LoopRowNode loopRowNode, 
			      DataLoopNameListNode dlNameList,
			      int row) {
	//// ughs!!! the following does not exist:
	///  Enumeration eValueList = loopRowNode.elements(); 

	int eValueIndex = 0;

	Enumeration eNameListNode   = dlNameList.elements();
	while (eNameListNode.hasMoreElements()) {
	    LoopNameListNode nameList = 
		(LoopNameListNode)eNameListNode.nextElement();
	    Enumeration eNameList = nameList.elements();
	
	    while (eNameList.hasMoreElements()) {
		DataNameNode rowName = 
		    (DataNameNode) eNameList.nextElement();
		DataValueNode rowValue = (DataValueNode) 
		    loopRowNode.elementAt(eValueIndex);
		eValueIndex++;
		
		print2Output(TAB+TAB+TAB+rowName.getLabel());   
		print2Output("_"+row+" = ");   
		if (isNumber(rowValue.getLabel())) {
		    print2Output(rowValue.getLabel()+";\n");
		} else {
		    print2Output("\""+rowValue.getLabel()+"\";\n");   
		}   
	    }
	}
    }



    //-------------------------------------------------------------------
    private void getDataLoop(DataLoopNode dataloop, int loopNumber){
        
        if ((!checkConstraints) ||
            ((checkConstraints) && 
             (constraintMng.isSelectedLoop(dataloop.getNames())))) {

            print2Output(TAB+TAB+"loop_"+loopNumber+" = [\n");

            //  
            // DataLoopNameListNode contains the list of lists of names
            // that represents all the tag names for a loop. 
            DataLoopNameListNode dlNameList = dataloop.getNames();

            //
            // LoopTableNode is a 'table' of values in a loop. 
            // It is a vector of LoopRowNodes.
            LoopTableNode loopTable = dataloop.getVals();

            Enumeration eValueTable = loopTable.elements();
            int row = 1;

            while (eValueTable.hasMoreElements()){
                if (row>1) {
                    print2Output("\n");   
                }
                LoopRowNode loopRowNode = 
                    (LoopRowNode) eValueTable.nextElement();

                getRowOutput(loopRowNode, dlNameList, row);
                row++;
            }

            print2Output(TAB+TAB+TAB+"];\n");   
        }
    }

    //-------------------------------------------------------------------
    private void translateSaveFrameNode(SaveFrameNode saveFrame,
					int saveFrameNumber) {
	if (saveFrameNumber > 1) {
	    print2Output("\n");
	}

	//print2Output(TAB+"save_frame_"+saveFrameNumber+" = [\n");
	//print2Output(TAB+saveFrame.getLabel()+" = [\n");
	print2Output(TAB+getSaveFrameName(saveFrame)+" = [\n");
        
        
	// 
	// maybe this type is not necessary... 
	print2Output(TAB+TAB+"Type = \"save_frame\";\n");

	int loopNumber = 1;
	Enumeration e = saveFrame.elements(); 
        while (e.hasMoreElements()) {
	    StarNode node = (StarNode) e.nextElement();
	    if (DEBUGING_1){
		System.out.println("<NMR-STAR2ClassAd>\t"+node);
	    }
	    if (isDataItem(node)) {
		getNameValuePairs((DataItemNode)node, TAB+TAB);
	    } else { // "DataLoopNode"
		getDataLoop((DataLoopNode)node, loopNumber);
		loopNumber++;
	    }
	}

	print2Output(TAB+TAB+"];\n");
    }

    //-------------------------------------------------------------------
    /**
     * The top level is always save frames, so the first step 
     * is getting all SaveFrameNode parsed.  There are no save
     * frames within save frames. Within the save frames there 
     * are loops and name/value pairs that are translated in 
     * translateSaveFrameNode() method
     */
    private void getSaveFrameNodes(){
	// VectorCheckType is essentially the exact same thing 
	// as the standard Java class java.util.vector,
	// but with the additional provisions to ensure that 
	// only objects of a specific type will be allowed 
	// to be put into the vector. Anything else is deemed 
	// an error and generates an exception. 
	VectorCheckType allSaveFrameNodes=null;

	try{
	    allSaveFrameNodes = aStarTree.searchForType(
	      Class.forName("EDU.bmrb.starlibj.SaveFrameNode"), ALL);
	} catch (ClassNotFoundException e) {
	    System.out.println("<NMR-STAR2ClassAd>\tError getting"+
                               " SaveFrameNodes");
	    System.exit(1);
	}
	
	//
	// translate each SaveFrameNode retrieved on VectorCheckType
	int saveFrameNumber = 1;
	Enumeration e = allSaveFrameNodes.elements(); 
        while (e.hasMoreElements()) {
	    SaveFrameNode saveFrame = (SaveFrameNode) e.nextElement();
	    if (DEBUGING_1){
		System.out.println("<NMR-STAR2ClassAd>\t"+saveFrame);
	    }
	    translateSaveFrameNode(saveFrame, saveFrameNumber);
	    saveFrameNumber++;
	}

    }

    //-------------------------------------------------------------------
    private void stop() {
	if (DEBUGING){
	    System.out.println("<NMR-STAR2ClassAd>\tOUTPUT:");
	    System.out.println(debugingOutStream);
	}
        try {
	    outputFile.close();
	} catch (IOException e) {
	    System.out.println("<NMR-STAR2ClassAd>\tError closing"+
                               " output file");
	}    
    }
}
