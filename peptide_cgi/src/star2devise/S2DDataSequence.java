// ========================================================================
// (c) Copyright 2003-2004
// By the BioMagResBank
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the BioMagResBank.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ------------------------------------------------------------------------

//package BMRB.chemShift;
package star2devise;

import java.util.*;

/**
* A wrapper around a vector of Strings (one letter).
* A list of the aminoacids, all in One-letter format.
*
* @author Dan Sinclair, BMRB 6/2003
*/

public class S2DDataSequence {
	private Vector data;

  /**
  * Default Class Constructor
  *
  */
	public S2DDataSequence() {
		data = new Vector();
	}

  /**
  * Adds another amino acid (in one-letter format) to the end of the S2DDataSequence.
  *
  * @param  datum the amino acid to be added
  */
	public void add(String datum) {
		if ( datum.length() > 1 ) {
			System.err.println("ERROR: DATASEQUENCE: add(): String " + datum + " too long.");
		} else {
			data.add(datum);
		}
	}

  /**
  * Removes the indexth acid in the sequence.
  *
  * @param  index the index of the acid to be removed
  */
	public String remove(int index) {
		return (String)data.remove(index);
	}
  
  /**
  * Gets the first acid in the Sequence.
  *
  * @return the first acid in the sequence
  */
  public String first() {
    return (String)data.get(0);
  }

  /**
  * Gets the last acid in the Sequence.
  *
  * @return the last acid in the sequence
  */
  public String last() {
    return (String)data.get(data.size() - 1);
  }

  /**
  * Gets the indexth acid in the Sequence.
  *
  * @param  index the index of the acid to get
  * @return       the indexth acid in the sequence
  */
	public String get(int index) {
		return (String)data.get(index);
	}

  /**
  * Gets the length of the sequence
  *
  * @return the size of the underlying Vector
  */
  public int size() {
    return data.size();
  }

  /**
  * Tests for equality between this and another object. If the other object is a
  * Data Sequence, it compares the sequence acid by acid.
  *
  * @param  o the Object to be tested
  * @return   a boolean as to whether or not they are equal
  */
  public boolean equals(Object o) {
    return data.equals( ((S2DDataSequence)o).data );
  }

  /**
  * Tells whether or not the S2DDataSequence is empty.
  *
  * @return   a boolean as to whether or not it is empty
  */
  public boolean isEmpty() {
    return data.isEmpty();
  }

  /**
  * Takes off the first index number of acids from the
  * front of the S2DDataSequence.
  *
  * @param  index the number of acids to take off the front
  */
  public void truncateFront(int index) {
    for (int i = 0; i < index; i++) {
      data.remove(0);
    }
  }

  /**
  * Removes all acids from the end of the sequence, starting at index.
  * Takes off the last index number of acids from the
  * end of the S2DDataSequence.
  *
  * @param  index the number of acids to take off the end
  */
  public void truncateEnd(int index) {
    while (data.size() > index) {
      data.remove(index);
    }
  }

  /**
  * Gets the S2DDataSequence as a String.
  *
  * @return  a String representation of the S2DDataSequence
  */
	public String toString() {
		String result = "";
    for (int i = 0; i < data.size(); i++) {
      result += (String)data.get(i);
    }
    return result;
	}

	/**
  * Gets the S2DDataSequence as a String in a list format. It separates the sequence into
  * chunks of 10 acids
  *
  * @return  a String representation of the DataSeqence in list format
  */
	public String toListFormat() {
		String result = "[";
    for (int i = 0; i < data.size(); i++) {
      if ( ((i % 10) == 0) && (i > 0) ) {
        result += ", ";
      }
      result += (String)data.get(i);
    }
    result += "]";
    return result;
	}

  /**
  * Gets the S2DDataSequence as a String in a TALOS format. Used for output to a
  * TALOS file.
  *
  * @return  a String representation of the DataSeqence in TALOS format
  */
	public String toTALOSFormat() {
		String result = "";
    int resCount = 0;
		for (int i = 0; i < data.size(); i++) {
			if ( (i % 50) == 0) {
				result += "\nDATA SEQUENCE";
			}
      if ( (i % 10) == 0 ) {
        result += " ";
      }
			result += data.get(i);

		}
		return result;
	}


}
