/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.1  1995/11/09 15:30:54  ravim
  Initial revision
*/

// Tests the crsp extraction routines

#include <iostream.h>

#include "sec.h"
#include "tapedrive.h"

main(int argc, char **argv)
{
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <input tape device>" << endl;
    exit(0);
  }

  // Open data file

  TapeDrive tape(argv[1], "r", -1, 32768);
  if (!tape)  {
    cerr << "Error: could not open tape device " << argv[1] << endl;
    exit(0);
  }
  tape.readTarHeader();

  // Try to extract the info for the first security
  
  Security *sec = new Security(tape);

  sec->print_security();
}
