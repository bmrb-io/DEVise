/*
 * Class ParseToTree reads and parses an input data file exported from
 * a Paradyn run such as "resources" and "index" into a hirarchical tree structure
 * represented by MutableTreeNodes in Java1.2.
 * Interface in this class is:
 *     getTreeRoot which returns the tree root of parsed source file.
 * If the parsed file is index file, there are two additional interface calls:
 *     getDataFileList which returns all datafiles in the index file
 *     getAllDataFileInfo which returns a string of useful DataFile information
 *
 * - Wei Zhou
 * - June, 1999
 */
package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;

class ParserToTree {

    // Constants
    public static final boolean DEBUG = true;
    private static final String INDEX = "index";
    private static final String RESOURCE = "resource";

    // Data
    protected DefaultMutableTreeNode treeRoot;
    protected DefaultTreeModel treeModel; 
    protected String fileName; // including dirName
    protected String dirName;

    // Elements of this list should be type of "DataFile"
    protected LinkedList dataFiles; 

    protected String dataInfo;

    // Resources file is "0", index file is "1" ...
    protected int fileType; 
    
    // Constructor
    public ParserToTree() {
	this(null, null);
    }

    public ParserToTree(String fileName) {
	this(fileName, null);
    }

    // Constructor
    public ParserToTree(String fileName, String dirName) {
	this.treeRoot = null;
	this.treeModel = null;
	this.fileName = fileName;
	this.dirName = dirName;
	this.dataFiles = null;

	this.dataInfo = "";
	this.fileType = -1;
    }

    // Retrieve the tree rootnode of parsed sourcefile
    public DefaultMutableTreeNode getTreeRoot() {
	parsing();
	return treeRoot;
    }

    protected void parsing() {
	if ((fileName.indexOf(RESOURCE) >= 0) || (fileName.indexOf(INDEX) >= 0))
	    parseSrc();

	else
	    System.out.println("Data file or search file");

}

    protected void parseSrc() {
	
	if (DEBUG) System.out.println("START readFile: (ParserToTree)"+fileName);
	if (fileName == null || fileName.equals(""))
            return;

	BufferedReader in = null;
	try {
	    in = new BufferedReader(new FileReader(fileName));
	}catch (Exception ex) {
	    System.err.println("ERROR: parse: "+ex);
	    return;
	}

	// Set "whole program" as treeRoot node
	treeRoot = new DefaultMutableTreeNode("whole program");
	treeModel = new DefaultTreeModel(treeRoot);

	// Read file and parse it line by line
	String line;
	// index file
	if (fileName.indexOf(INDEX) >= 0) {
	    fileType = 1;
	    try {
		// Ready to get Files information
		dataFiles = new LinkedList();

		while ((line = in.readLine()) != null) 
		    parseIndexLine(line);
	    }catch (IOException io) {
		System.err.println(io);
		treeRoot = null;
		return;
	    }
	}
	// resource file
	else{
	    fileType = 0;
	    try {
		while ((line = in.readLine()) != null) 
		    parseSrcLine(line);
	    }catch (IOException io) {
		System.err.println(io);
		treeRoot = null;
		return;
	    }
	}
	// Close the inputStream
	try {
	    in.close();
	} catch (Exception ex) {
	    System.err.println(ex);
	    return;
	}
    }

    // Lines parsing to get hirarchical nodenames of each line
    protected void parseIndexLine(String line) {

	if ((line == null) || (line.trim().length() == 0))  return;
	if (line.indexOf ("/") < 0) 
	    return;

	// Create DataFile class to memorize info of this file
	line = line.trim();
	DataFile datafile = new DataFile();
	datafile.setAllInfo(line);
	
	// Separate string by the first "/"
	String shortline;
	String phaseTail;
	String head;

	if (line.indexOf("/") < (line.lastIndexOf(" "))) {
	    shortline = line.substring(line.indexOf("/"), line.lastIndexOf(" "));
	    phaseTail = line.substring(line.lastIndexOf(" "));
	    head = line.substring(0, line.indexOf("/"));
	}

	else { // no phase information
	    shortline = line.substring(line.indexOf("/"));
	    head = line.substring(0, line.indexOf("/"));
	    phaseTail = null;
	}
	
	// Parse the head of line which is part of data
	if ((head != null) && (head.trim().length() > 0) && (head.indexOf(" ") >= 0)) {
	    StringTokenizer headtoken = new StringTokenizer(head, " ");
	    int headcount = headtoken.countTokens();
	    String [] infos = new String[headcount];
	    int i = 0;
	    while (headtoken.hasMoreTokens()) {
		infos[i] = headtoken.nextToken().trim();
		if (infos[i] != "")
		    i++;
	    }
	    datafile.setDirName(dirName);
	    datafile.setFileName(infos[0]);
	    datafile.setMeasure(infos[1]);
	}

	if ((phaseTail != null) && (phaseTail.trim().length() > 0))
	    datafile.setPhase(phaseTail.trim());

	// Parse the line into sublines
	if (shortline.indexOf(",") >= 0) {
	    
	    StringTokenizer linetoken = new StringTokenizer (shortline, ", ");
	    int linecount = linetoken.countTokens();
	    String [] lines = new String[linecount];
	    int i = 0;
	    while (linetoken.hasMoreTokens()) {
		lines[i] = linetoken.nextToken();

		if (lines[i].substring(lines[i].indexOf("/")+1).indexOf("/") >= 0) {
		    // not include the "whole program" and "first level"
		    DefaultMutableTreeNode leafNode = parseNodes(lines[i]);
		    if (leafNode != null) {
			datafile.addTreeNode(leafNode);
			
			// Mark the leaf node in index tree if hasn't marked
			Object obj = leafNode.getUserObject();
			if (obj instanceof String)
			    leafNode.setUserObject(new MarkedNode((String)obj, true));
		    }
		}
		
		i++; }
	}
	if (dataFiles != null) 
	    dataFiles.add(datafile);
	else
	    System.out.println("ERROR: dataFiles is NULL");
    }

    protected void parseSrcLine(String line) {
	if ((line == null) || (line.trim().length() == 0))  return;
	if (line.indexOf ("/") < 0) 
	    return;

	String shortline = line.substring(line.indexOf("/"));
	DefaultMutableTreeNode leafNode = parseNodes(shortline);
    }

    protected DefaultMutableTreeNode parseNodes(String line) {

	if ((line == null) || (line.trim().length() == 0))  return null;
	if (line.indexOf ("/") < 0) 
	    return null;

	// Parse the line into nodeNames
	StringTokenizer nodetoken = new StringTokenizer (line, "/ ");
	int nodecount = nodetoken.countTokens();
	String [] nodeNames = new String[nodecount];
	int i = 0;   
	while (nodetoken.hasMoreTokens()) {   
	    nodeNames[i]= nodetoken.nextToken();   
	    i++; }
	
	DefaultMutableTreeNode leafNode = addNodesToTree(nodeNames, treeRoot);
	return leafNode;
    }

    protected DefaultMutableTreeNode addNodesToTree(String [] nodeNames, 
						    DefaultMutableTreeNode parent){

	if (nodeNames == null) return null;

	TreeHandler treeHandler = new TreeHandler(treeRoot, treeModel);

	int len = nodeNames.length;
	DefaultMutableTreeNode subParent = parent;

	for (int i = 0; i < len; i++) {
	    // Traverse tree to add a node in a proper location
	    DefaultMutableTreeNode subTree = 
		treeHandler.hitChildren(nodeNames[i], subParent);

	    // Subtree has existed
	    if (subTree != null)
		subParent = subTree;
	
	    // Add a new node into the subtree
	    else 
		subParent = treeHandler.addObject(subParent, nodeNames[i]);
	}

	treeRoot = treeHandler.getTreeRootNode();
	treeModel = treeHandler.getTreeModel();

	return subParent;
		
    } 	

    public LinkedList getDataFileList() {
	
	sortDataFiles();
	return dataFiles;
    }

    public String getAllDataFileInfo() { 
	if (dataFiles == null) return "";

	int len = dataFiles.size();
	for (int i = 0; i < len; i++) {
	    DataFile datafile = (DataFile)dataFiles.get(i);
	    dataInfo = dataInfo + datafile.getMeasure() +": "+ datafile.getNodeNameList() + "\n";
	}

	return dataInfo; 
    }

    public int getFileType() {
	return fileType;
    }

    protected void sortDataFiles() {
	// sort Datafiles by the measurement/metrics
	if (dataFiles == null) return;
	LinkedList sortList = new LinkedList();
	for (int i= 0; i< dataFiles.size(); i++) {
	    boolean addin = false;
	    DataFile thisfile = (DataFile)dataFiles.get(i);
	    for (int j=0; j< sortList.size(); j++) {
		DataFile earlyfile = (DataFile)sortList.get(j);
		if (thisfile.getMeasure().compareTo(earlyfile.getMeasure())<=0){
		    sortList.add(j, thisfile);
		    addin = true;
		    break;
		}
	    }
	    if (!addin)
		sortList.add(thisfile);
	}
	dataFiles = sortList;
    }

    public void printDataFiles(LinkedList datafiles) {
	if (datafiles == null) return;
	for (int i=0; i< datafiles.size(); i++) {
	    DataFile datafile = (DataFile)datafiles.get(i);
	    System.out.println(datafile.getMeasure());
	}
    }
	    
}





		
	
	
