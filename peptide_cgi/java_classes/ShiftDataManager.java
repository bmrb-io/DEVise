import java.io.*;

//The following class maintains data contained in the shift.txt table
// Converted from dsilva's C++ code to Java by Richard Chang, 06/25/1999
public class ShiftDataManager
{
    private static final int MAX_DATA_ENTRIES = 500;
    private static String filename;

    //Residue label name
    private String [] aminoAcidType = new String[MAX_DATA_ENTRIES];
    //Atom name
    private String [] atomName = new String[MAX_DATA_ENTRIES];
    //Standard value of chemical shift
    private double [] chemicalShiftCorr = new double[MAX_DATA_ENTRIES];
     //The range column vals
    private double [] range = new double[MAX_DATA_ENTRIES];
    //Total number of entries in the table
    private int totalNumEntries; 


    public class Pair
    {
	public double chemshift;
	public double offset;
    }


    //Constructor which initializes the object by reading the data file 
    ShiftDataManager( String filename )
    {
	try
	{
	    StreamTokenizer inFile = 
		new StreamTokenizer(new FileReader(filename));
	    totalNumEntries = 0;
	    this.filename = filename;
	    
	    // This assumes that each entry corresponds to one line
	    while (inFile.nextToken() != inFile.TT_EOF)
	    {
		aminoAcidType[totalNumEntries] = inFile.sval;
		inFile.nextToken();
		atomName[totalNumEntries] = inFile.sval;
		inFile.nextToken();
		chemicalShiftCorr[totalNumEntries] = inFile.nval;
		inFile.nextToken();
		range[totalNumEntries] = inFile.nval;
		totalNumEntries++;
		
	    }

	} catch (FileNotFoundException e)
	{
	    System.err.println("File not found: "
			       + e.getMessage() );
	} catch (IOException e)
	{
	    System.err.println("IO Exception: "
			       + e.getMessage() );
	}
    } // end constructor

    
    //A function that takes as input arguments a residue name and an
    //atom name and returns a pair of values -- standard value of
    //chemical shift and the range. Also the reference parameter. The
    //FileWriter is used to print error messages for all those calls
    //which result in no match in the table.
    public Pair returnValues( FileWriter error, String in_aminoAcidType, 
			      String in_atomName )
    {
	boolean foundAminoAcidType = false;
	boolean foundAtomName      = false;
	Pair retValues = new Pair();
	
	//Initialize with default error values
	retValues.chemshift = -1.0;
	retValues.offset    = -1.0;
	    
	int position = 0;

	while((!foundAminoAcidType) && position < totalNumEntries)
	{
	    if(in_aminoAcidType.compareTo(aminoAcidType[position]) == 0)
		foundAminoAcidType = true;
	    else
		position++;
	}

	try 
	{
	    if(foundAminoAcidType)
	    {
		while(position < totalNumEntries
		      && (!foundAtomName)
		      && in_aminoAcidType.compareTo(aminoAcidType[position])
		               == 0)
		{
		    if(in_atomName.compareTo(atomName[position]) == 0)
			foundAtomName = true;
		    else
			position++;
		}
		
		if(!foundAtomName)
		{
		    error.write("WARNING!!! ....Atom " + in_atomName 
				+ " corresponding to amino acid "
				+ in_aminoAcidType  
				+ " not found in chemical "
				+ "shift reference table file "
				+ filename + "\n");
		    return retValues;
		}
	    
	    } else 
	    {
		error.write("Amino acid type " + in_aminoAcidType 
			    + "  not found..... \n");
		return retValues;
	    }
	    
	} catch (IOException e)
	{
	    System.err.println("IO Exception: "
			       + e.getMessage() );
	}
	
	retValues.chemshift = chemicalShiftCorr[position];
	retValues.offset  = range[position];
	return retValues;
    } // end function returnValues
	
}
