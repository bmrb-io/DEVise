//----------------------------------------------------------------------
//
// File Name: XManImportBase.java
//
// Purpose:   Importers' base class
//
// D.O.B.:    3/10/2000
//
//----------------------------------------------------------------------
package xman.importers;

import java.util.Observable;
import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;
import java.util.StringTokenizer;
import java.io.CharConversionException;
import java.util.Iterator;

public abstract class XManImportBase 
    extends Observable {

    //------------------------------------------
    // Private data
    //------------------------------------------
    private boolean DEBUG = true;
    private boolean hasNewAttribute_;

    private File                metaFile_;       // Meta file fd
    private BufferedReader      freader_;
    private BufferedWriter      fwriter_;  // File writer

    private String              workDir_;
    private int                 totalExperiments_;
    private Integer             type_;

    //------------------------------------------
    // Data for map, set, etc.
    //------------------------------------------
    private Vector  attributes_;     // Vector of XManAttribute
    private Vector  defaultValue_;   // Default attributes
    private TreeMap attributeMap_;   // Map of attributes using attribute name
    private Vector  visualAttribute_;// Subset of attributes_ -- displabable

    private Vector  data_;           // Vector of XManExperimentInfo
    private Vector  partial_;        // Partial set of the data
    private TreeMap searchMap_;      // For column based search

    final static String wseperator = new String(" |");
    final static String rseperator = new String("|");
    final static String STR_XMAN_META_FILE = ".xmanMeta";
    public final static String STR_SOURCE          = "Directory";
    public final static String STR_EXPERIMENT_NAME = "Experiment Name";
    public final static String STR_CPU_COUNT       = "CPU Count";
    public final static String STR_ARCHITECTURE    = "Architecture";
    public final static String STR_DATE_IMPORTED   = "Date Imported";

    //------------------------------------------------------
    // Methods
    //------------------------------------------------------
    public XManImportBase(String workDir) 
	throws CharConversionException, java.io.IOException {
	//------------------------------------------
	// Some initialization
	//------------------------------------------
	initialize();

	workDir_          = workDir;     // Working directory
	metaFile_         = new File(workDir+File.separator+
				     STR_XMAN_META_FILE);
	if (metaFile_.exists()) {

	    freader_= new BufferedReader(new FileReader(metaFile_.toString()));
	    
	    // Make sure the type matches
	    StringTokenizer tokens = new StringTokenizer(freader_.readLine());
	    if (tokens.hasMoreTokens()) {
		type_ = new Integer(tokens.nextToken().trim());

		buildAttributeMap();
		buildExperimentMap();
	    }

	    freader_.close();
	    // Everything is okay now. 
	    fwriter_=new BufferedWriter(new FileWriter(metaFile_.toString()));
	}
    }

    public XManImportBase(String workDir, int type) 
	throws CharConversionException, java.io.IOException {

	//------------------------------------------
	// Some initialization
	//------------------------------------------
	initialize();

	setupDefaultAttributes();

	type_             = new Integer(type);
	workDir_          = workDir;     // Working directory
	metaFile_         = new File(workDir+File.separator+
				     STR_XMAN_META_FILE);

	if (metaFile_.exists()) {

	    freader_= new BufferedReader(new FileReader(metaFile_.toString()));
	    
	    // Make sure the type matches
	    StringTokenizer tokens = new StringTokenizer(freader_.readLine());
	    if (tokens.hasMoreTokens()) {
		Integer value = new Integer(tokens.nextToken().trim());
		if (value.compareTo(new Integer(type)) != 0) {
		    throw new CharConversionException("Incorrect type!");
		}
		else {
		    // We have a good match! Now, let's build a map for the
		    // attributes
		    buildAttributeMap();
		    buildExperimentMap();
		    setChanged();
		    notifyObservers();
		}
	    }
	    freader_.close();
	    // Everything is okay now. 
	    fwriter_ = new BufferedWriter(new FileWriter(metaFile_.toString()));
	}
	// Now we should have a meta file
    }

    abstract protected void XManCopyData(String source, File destDir); 
    // For copying data

    //----------------------------------------------------------------------
    // Method Name : numberOfExperiments -- returns number of experiments
    //----------------------------------------------------------------------
    public int numberOfExperiments() {
	return (totalExperiments_ + 1); // or values_.size()
    }

    //----------------------------------------------------------------------
    // Method Name : numberOfAttributes -- returns number of attributes for
    // the collection
    //----------------------------------------------------------------------
    public int numberOfAttributes() {
	return attributes_.size();
    }

    //----------------------------------------------------------------------
    // Method Name: flushMetaFile() -- write meta file to disk
    //----------------------------------------------------------------------
    public void flushMetaFile() 
	throws java.io.IOException {
	// Remove the old meta file regardless of what
	System.out.println("Flushing!");
	metaFile_.delete();
	metaFile_.createNewFile();

	fwriter_ = new BufferedWriter(new FileWriter(metaFile_));
	fwriter_.write(type_.toString(), 0, (type_.toString()).length());
	fwriter_.newLine();

	// Next, the attributes
	Integer size = new Integer(attributes_.size());
	fwriter_.write(size.toString(), 0, (size.toString()).length());
	fwriter_.newLine();
	for (int i=0; i< attributes_.size(); i++) {
	    String attribute = 
		((XManAttribute) attributes_.get(i)).attributeName();
	    String defaultVal = 
		((XManAttribute) attributes_.get(i)).defaultValue();
	    fwriter_.write(attribute ,0, attribute.length());
	    fwriter_.write(wseperator, 0, wseperator.length());
	    fwriter_.write(defaultVal,0, defaultVal.length());
	    fwriter_.write(wseperator, 0, wseperator.length());
	    fwriter_.newLine();
	}

	// Now the data
	for (int j=0; j < data_.size(); j++) {
	    XManExperimentInfo data = (XManExperimentInfo) data_.get(j);

	    // Write out rest of the data
	    for (int k=0; k < attributes_.size(); k++) {
		String value = data.getData(k);
		fwriter_.write(value, 0, value.length());
		fwriter_.write(wseperator, 0, wseperator.length());
	    }
	    fwriter_.newLine();
	}
	fwriter_.flush();
	fwriter_.close();
	System.out.println("Done!");
    }
    //----------------------------------------------------------------------
    // Method Name : attributeName -- returns name of attributes for
    // the collection
    //----------------------------------------------------------------------
    public String attributeName(int attributeColumn) {
	return ((XManAttribute) attributes_.get(attributeColumn)).attributeName();
    }

    //----------------------------------------------------------------------
    // attributes() will return the user (i.e. displayable attributes
    //----------------------------------------------------------------------
    public Vector attributes() {
	return attributes_;
    }

    public Vector visualIndex() {
	return visualAttribute_;
    }

    public Vector data() {
	return data_;
    }

    protected int nextIndex() {
	return totalExperiments_;
    }

    public void setDefaultValue(String attributeName, String defVal) {
	int index = indexOf(attributeName);
	if (index != -1) {
	    defaultValue_.set(index, defVal);
	}
    }
    public String defaultValue(int index) {
	return (String) defaultValue_.get(index);
    }

    public String defaultValue(String attributeName) {
	int index = indexOf(attributeName);
	return defaultValue(index);
    }
    //----------------------------------------------------------------------
    // experimentInfo
    //----------------------------------------------------------------------
    public String experimentInfo(int attributeColumn, int index) {
	return (String) 
	    ((XManExperimentInfo)(data_.get(index))).getData(attributeColumn);
    }
    
    public boolean exist(String attributeName) {
	return (attributeMap_.get(attributeName) != null);
    }
    
    public int indexOf(String attributeName) {
	return ((Integer) attributeMap_.get(attributeName)).intValue();
    }

    public void addNewExperiment(XManExperimentInfo info,
				 String             inputDir)
	throws java.io.IOException {
	//String inputDir = info.getData(STR_SOURCE);
	File fileDir = File.createTempFile("xman", "exp", new File(workDir_));

	info.setData(STR_SOURCE, fileDir.toString());
	XManCopyData(inputDir, fileDir);

	// Do some assertions here
	int position = data_.size(); 

	data_.add(position, info);

	for (int i=0; i<attributes_.size(); i++) {
	    // Now, add to the search engine
	    String attribute = ((XManAttribute) attributes_.get(i)).attributeName();
	    String dataString = info.getData(attribute);
	    TreeMap val = (TreeMap)searchMap_.get(attribute);
	    TreeSet dataSet = (TreeSet) val.get(dataString);
	
	    if (dataSet == null) {
		dataSet = new TreeSet();
		dataSet.add(new Integer(position));
		val.put(dataString, dataSet);
	    }
	    else {
		dataSet.add(new Integer(position));
	    } 
	}
	
	totalExperiments_++;
	setChanged();
	System.out.println("Notifying observers!");
	notifyObservers();
    }
    //------------------------------------------------
    // private void buildAttributeMap
    // 
    // This method will read the meta file and build
    // a map for the collection
    //------------------------------------------------
    private void buildAttributeMap() 
	throws java.io.IOException {
	String newLine;
	StringTokenizer tokens;

	// First, read the attributes size
	tokens = new StringTokenizer(freader_.readLine());

	Integer size = new Integer(tokens.nextToken().trim());
	String name, def;

	for (int i=0; i<size.intValue(); i++) {
	    newLine = freader_.readLine();
	    tokens = new StringTokenizer(newLine, rseperator);
	    if (tokens.hasMoreTokens()) {
		name = tokens.nextToken().trim();
	    }
	    else {
		name = new String();
	    }
	    if (tokens.hasMoreTokens()) {
		def  = tokens.nextToken().trim();
	    }
	    else {
		def = new String();
	    }

	    addAttribute(new XManAttribute(name, def));
	    searchMap_.put(name, new TreeMap());
	}
    }

    //-----------------------------------------------
    // This method will build data structures for
    // the experiemen data
    //-----------------------------------------------
    private void buildExperimentMap() 
	throws java.io.IOException {

	String newLine;
	StringTokenizer tokens;

	int size = attributes_.size();
	int j    = 0;
	while((newLine = freader_.readLine()) != null) {

	    tokens = new StringTokenizer(newLine, rseperator);

	    // Unique index
	    Integer index = new Integer(j++);

	    // Build the list
	    XManExperimentInfo info = new XManExperimentInfo(this);

	    int i = 0;
	    boolean done = (i >= size);

	    while (!done) {
		String dataString = null;
		if (tokens.hasMoreTokens() && (i < size)) {
		    dataString = tokens.nextToken().trim();
		}
		else {
		    dataString = new String();
		}

		// First, put in the vector for accessing
		String attribute = ((XManAttribute) attributes_.get(i)).attributeName();
		info.setData(attribute, dataString);

		// Now, add to the search engine
		TreeMap val = (TreeMap)searchMap_.get(attribute);

		TreeSet dataSet = (TreeSet) val.get(dataString);

		if (dataSet == null) {
		    dataSet = new TreeSet();
		    dataSet.add(index);
		    val.put(dataString, dataSet);
		}
		else {
		    dataSet.add(index);
		} 
		done = (++i >= size);
	    }

	    // Add data to the list
	    data_.add(index.intValue(), info);

	    totalExperiments_++;
	}
    }

    //------------------------------------------
    // Initialize the importer
    //------------------------------------------
    private void initialize() {

	// Initialize some common data
	attributes_       = new Vector(20, 5);
	defaultValue_     = new Vector(20, 5);
	data_             = new Vector(100, 10);
	partial_          = new Vector(100, 10);
	attributeMap_     = new TreeMap();
	visualAttribute_  = new Vector(20, 5);
	searchMap_        = new TreeMap();
	totalExperiments_ = 0;
	hasNewAttribute_  = false;
	for (int i =0 ; i < attributes_.size(); i++ ) {
	    defaultValue_.add(i, new String());
	}
    }

    //------------------------------------------
    // setupDefaultAttributes -- if we are 
    // creating a new collection, then create 
    // some default attributes.
    //------------------------------------------
    private void setupDefaultAttributes() {
	// Now, set up the fixed attribute set
	XManAttribute directory = new XManAttribute(STR_SOURCE,
						    new String(),
						    false,
						    false,
						    false);

	addAttribute(directory);

	/*
	XManAttribute isSelected = new XManAttribute(STR_SELECTED,
						     true, 
						     true, 
						     false);

	addAttribute(isSelected);
	*/
    }

    //------------------------------------------
    // addAttribute -- will add an attribute to
    // the attribute vector and map
    //------------------------------------------
    public void addAttribute(XManAttribute attribute) {
	if (!exist(attribute.attributeName())) {
	    Integer index = new Integer(attributes_.size());
	    attributes_.add(index.intValue(), attribute);
	    defaultValue_.add(index.intValue(), attribute.defaultValue());
	    attributeMap_.put(attribute.attributeName(), index);
	    // Add to the search map
	    searchMap_.put(attribute.attributeName(), new TreeMap());

	    for (int i=0; i < data_.size(); i++) {
		((XManExperimentInfo)data_.get(i)).setData(attribute.attributeName(), new String());
	    }

	    System.out.println("New attribute!");
	    if (attribute.isDisplayable().booleanValue()) {
		visualAttribute_.add(index);
		setChanged();
		setNewAttribute(true);
		notifyObservers();
		System.out.println("Notifying observers");
		setNewAttribute(false);
	    }
	}
    }

    private void setNewAttribute(boolean newAtt) {
	hasNewAttribute_ = newAtt;
    }

    public boolean hasNewAttribute() {
	return hasNewAttribute_;
    }

    public void forceRefresh() {
	setChanged();
	notifyObservers();
    }

    public boolean search(XManExperimentInfo info) {
	
	boolean done = false;
	int i = 0;
	// First, clean up
	partial_.clear();   // Not really necessary

	XManAttribute attr = (XManAttribute) attributes_.get(i);
	String name        = attr.attributeName();
	String value       = info.getData(name);
	TreeMap val        = (TreeMap) searchMap_.get(name);
	TreeSet resultSet = null, subset;

	if (value.length() > 0) {
	    subset = (TreeSet) val.get(value);
	    if (subset != null) {
		resultSet = new TreeSet(subset);
	    }
	    else {
		resultSet = null;
	    }
	}
	else {
	    resultSet = null;
	}

	done = ((resultSet == null) || (attributes_.size() == 1));

	while (!done) {
	    i++;
	    attr  = (XManAttribute) attributes_.get(i);
	    name  = attr.attributeName();
	    value = info.getData(name);

	    if (value.length() > 0) {
		subset = (TreeSet) ((TreeMap) searchMap_.get(name)).get(value);
		if (subset == null) {
		    done = true;
		    resultSet.clear();
		}
		else {
		    resultSet.retainAll(subset);
		}
	    }
	    done = ((resultSet == null) || (i == attributes_.size()));
	}

	if (resultSet != null) {
	    Iterator it = resultSet.iterator();
	    while (it.hasNext()) {
		partial_.add(i, data_.get(((Integer)it.next()).intValue()));
	    }
	}
	return (resultSet != null);
    }
}


