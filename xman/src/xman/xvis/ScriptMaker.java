// $Id$
package xman.xvis;

import java.io.*;
import java.util.*;

// 1. Extract DEVKill, DEVCat & DEVDir and place as part of DEViseServer which will
//    be called DeviseManager (4/18)


// Points to Ponder:
// - Need to evaluate whether using ms timestamp is the best course of action of not.
//   Are there any concurreny issues that need to be resolved? Will this cause any
//   problems?

public class ScriptMaker
{
    public static String DEVDirName = "raptor_" + (new Date ()).getTime ();
    public static String DEVCatName = "catalog.dte";
    
    private String target;
    private boolean del;
    
    private File DEVDir;
    private File DEVStart;
    private File DEVKill;
    private File DEVCat;
    
    public ScriptMaker (String t, boolean b) throws YException
    { 
	// Get target directory
	target = t;
	
	// deleteOnExit ?
	del = b;
	
	makeScripts ();
    }
    
    public File getDEVDir () { return DEVDir; }
    
    public void clearCatFile () throws YException
    {
	try {
	    if (!DEVCat.delete ()) throw new YException ();
	    DEVCat.createNewFile ();
	} catch (IOException e) {throw new YException ("IOException in clearCatFile");}
    }
    
    public void cleanUp () throws YException  
    {
	// Remove DEVCat
	DEVCat.delete ();
	
	// Remove DEVKill
	DEVKill.delete ();
	
	// Remove DEVStart
	DEVStart.delete ();
	
	// Remove DEVDir
	DEVDir.delete ();
    }

    private void makeScripts () throws YException
    {
	File f = new File (target);
	
	if (!f.isDirectory ()) throw new YException (target + " is not a directory", "ScriptMaker.makeScripts");
	
	DEVDir = new File (f, DEVDirName);
	
	if (!DEVDir.exists ()) 
	    if (!DEVDir.mkdir ()) 
		throw new YException ("Could not create " + DEVDir, 
				      "ScriptMaker.makeScripts");
	
	if (del) DEVDir.deleteOnExit ();
	
	DEVCat = new File (DEVDir, DEVCatName);
	
	try {
	    DEVCat.createNewFile ();
	    if (del) DEVCat.deleteOnExit ();
	    
	    String l [] = { "#! /bin/csh -f", "limit coredumpsize 0", "limit descriptors 256",
			    "set path = ( $path /p/devise/public/bin )", "setenv DEVISE_ROOT    /p/devise",
			    "setenv DEVISE_LIB     $DEVISE_ROOT/public/lib",
			    "setenv DEVISE_TMP     /tmp/`whoami`-devise-tmp",
			    "setenv DEVISE_WORK    /tmp/`whoami`-devise-work",
			    "setenv DEVISE_LOG_DIR /tmp",
			    "setenv DEVISE_HOME_TABLE " + DEVCat.getCanonicalPath (),
			    "setenv DEVISE_EXEC   $DEVISE_ROOT/public/bin/devised",
			    "if (! -f $DEVISE_EXEC) then",
			    "echo \"Cannot find Devise executable. Perhaps this platform is not supported?\"",
			    "exit 1", "endif", "$DEVISE_ROOT/run/rmmem", "$DEVISE_EXEC $* -logLevel 5 >>& " + 
			    "/tmp/devised.out", "$DEVISE_ROOT/run/rmmem"};
	
	    DEVStart = new File (DEVDir, "devised");
	    writeFile (DEVStart.toString (), l, del);
	}
	catch (IOException e) { throw new YException ("IOException in makeScripts"); }

	
	Runtime currentRT = Runtime.getRuntime();
        Process proc;
	
	try {
	    proc = currentRT.exec ("chmod ugo+rx " + DEVStart.toString ());
	    proc.waitFor ();
	    
	
	    String m [] = { "#! /s/std/bin/perl", "if ($#ARGV >= 0) {", "$pattern = \"devised -port $ARGV[0]\";",
			    "open PS, \"ps -ax >& 1 |\" or die \"can't fork: $!\";",
			    "} else {", "$pattern = \"devised\";", "open PS, \"ps -axc >& 1 |\" or die \"can't fork: $!\";",
			    "}", "while (<PS>) {", "if ((!($_ =~ /kill_devised2/)) and $_ =~ /$pattern/) {", 
			    "print \"Killing $_\";", "@words = split /  */, $_;", "$pid = @words[1];", 
			    "system(\"kill -INT $pid\");", "system sleep 1;", "system(\"kill -INT $pid\");", 
			    "system sleep 1;", "system(\"kill -INT $pid\");", "}", "}", "close PS;"};
	    
	    
	    DEVKill = new File (DEVDir, "devise.kill");
	    writeFile (DEVKill.toString (), m, del);
	    
	    proc = currentRT.exec ("chmod ugo+rx " + DEVKill.toString ());
	    proc.waitFor ();
        } catch (IOException e) { throw new YException ("IOException occurred", "ScriptMaker.makeScripts"); }
	catch (InterruptedException e) { throw new YException ("InterruptedException occurred", "ScriptMaker.makeScripts"); }
    }
    
    private void writeFile (String name, String [] lines, boolean delete) throws YException
    {
	File f = new File (name);
	
	try {
	    if (!f.exists ()) f.createNewFile ();
	    
	    if (delete) f.deleteOnExit ();
	    
	    Writer out = new BufferedWriter(new FileWriter (f));
	    
	    for (int i = 0; i < lines.length; out.write (lines [i++] + "\n"));
	    
	    out.close ();
	}catch (IOException e) {throw new YException ("IOException occurred", "ScriptMaker.writeFile");}
    }
}


