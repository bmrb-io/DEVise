/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1995/09/06 17:20:18  jussi
  Downcased name of include file.

  Revision 1.2  1995/09/05 20:31:58  jussi
  Added CVS header.
*/

#include <iostream.h>
#include "tapedrive.h"

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " tape-file" << endl;
    cerr << "  verifies contents of tape file" << endl;
    exit(1);
  }

  TapeDrive tape(argv[1], "r");
  if (!tape) {
    cerr << "Cannot open file " << argv[1] << endl;
    exit(1);
  }

  int numRecs = 0;
  int totSize = 0;
  int size;
  char buffer[4096];

  while((size = tape.gets(buffer, sizeof buffer)) > 0) {
    numRecs++;
    totSize += size;
  }

  cout << endl << "File has " << numRecs << " records, "
       << totSize << " total bytes, "
       << 1.0 * totSize / numRecs << " bytes per record." << endl;

  const int inspectSize = totSize - TAPE_BLOCKSIZE;
  const int numTests = 10;

  for(int i = 0; i < numTests; i++) {
    long pos = (long)(1.0 * i / numTests * inspectSize);
    cout << "Seeking to " << pos << endl;
    if (tape.seek(pos) != pos)
      exit(1);
    if (tape.tell() != pos)
      cout << "Tell says tape is at " << tape.tell() << endl;
    (void)tape.gets(buffer, sizeof buffer);
  }

  for(i = 0; i < numTests; i++) {
    long pos = (long)(1.0 * (numTests - 1 - i) / numTests * inspectSize);
    cout << "Seeking to " << pos << endl;
    if (tape.seek(pos) != pos)
      exit(1);
    if (tape.tell() != pos)
      cout << "Tell says tape is at " << tape.tell() << endl;
    (void)tape.gets(buffer, sizeof buffer);
  }

  return 0;
}
