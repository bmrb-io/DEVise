//----------------------------------------------------------------------
//
// File Name: XManImportParadyn.java
//
// Purpose:   Import Paradyn data
//
// D.O.B.:    3/10/2000
//
//----------------------------------------------------------------------

package xman.importers;

import java.io.File;
import java.io.IOException;
import java.io.*;
import java.util.Vector;

public class XManImportParadyn 
    extends XManImportBase {

    private String text_;

    private String metaName_, destDir_;

    public XManImportParadyn(String outputDir) 
	throws java.io.IOException {
	super(outputDir, (int) 0);
	initialize();
	destDir_ = outputDir;
    }

    //----------------------------------------------------------------------
    // XManCopyData : copy data from source to destination
    //----------------------------------------------------------------------
    protected void XManCopyData(String inputDir, File dest){
	// Copied from sun's developer forum, with some minor modification
	FileInputStream  fin;
	FileOutputStream fout;

	File source = null; 
	File sourceDir;

	try {
	
	sourceDir = new File(inputDir);

	String filelist[] = sourceDir.list();

	dest.delete();   // Remove it first

	dest.mkdirs();   // Create as a directory

	for (int i=0; i < filelist.length; i++) {

		String outputFile = dest.toString() + 
		    File.separator + filelist[i];

		String inputFile = inputDir + File.separator + filelist[i];

		System.out.println(outputFile);
		System.out.println(inputFile);

		fin  = new FileInputStream(inputFile);
		fout = new FileOutputStream(outputFile); 

		int b;

		while ((b = fin.read()) != -1) {
		    fout.write(b);
		}

		fin.close();
		fout.close();

	    } 
	} 

	catch(FileNotFoundException fnfe)   {
	    System.out.println("Inputfile does not exist!");
	} 
	catch(IOException ioe) {
	    System.out.println("Error reading/writing files!");
	} 
	finally { 
	}

    } 

    private void initialize() {
	addAttribute(new XManAttribute(STR_EXPERIMENT_NAME, new String()));
	addAttribute(new XManAttribute(STR_DATE_IMPORTED, new String()));
	addAttribute(new XManAttribute(STR_CPU_COUNT, new String()));
	addAttribute(new XManAttribute(STR_ARCHITECTURE, new String()));
    }
} 


