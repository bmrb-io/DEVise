//----------------------------------------------------------------------
// 
// File Name:    XManDataManager.java
// 
// Description:  Experiment Management Data Manager
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import java.io.File;
import java.io.*;
import xman.shared.*;
import java.util.Vector;

public class XManDataManager
{
    private String rootDir = "/u/h/b/hbwang/xman/test/Paradyn/";
    private File root;
    private static XManFileFilter filter = new XManFileFilter("index");
    private Vector experiments_;

    public XManDataManager()
    {
	root = new File(rootDir);
	String[] list = root.list(filter);

	experiments_ = new Vector(list.length);
	for (int i=0; i < list.length; i++)
	    {
		String name = new String(rootDir);
		name += list[i];
		File dir = new File(name);

		experiments_.add(i, new XManExperiment(list[i], 
						       dir.lastModified(),
						       dir.list(filter).length,
						       false));
	    }
    }

    public void select(int row, Boolean value)
    {
	//experiments_.elementAt(row).setSelection(value);
    }

    public String rootName()
    {
	return root.getName();
    }

    public Vector getAllExperiments()
    {
	return experiments_;
    }

    public String[] getExperimentData(String exp)
    {
	String name = new String(rootDir);
	name += exp;
	File dir = new File(name);
	return dir.list(filter);
    }

    public XManExperiment getExperiment(int index)
    {
	return (XManExperiment) experiments_.elementAt(index);
    }

    public int getNumberOfExperiments()
    {
	return experiments_.size();
    }

}

class XManFileFilter implements FilenameFilter
{
    private static String index;

    public XManFileFilter(String name)
    {
	index = new String(name);
    }

    public boolean accept(File dir, String name)
    {
	return !(index.equals(name));
    }  
}





