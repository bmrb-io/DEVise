/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:31:57  jussi
  Added CVS header.
*/

#include <iostream.h>
#include "tapedrive.h"

int main(int argc, char **argv)
{
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " tape-file numRecs" << endl;
    cerr << "  tests write and read speed of tape" << endl;
    exit(1);
  }

  TapeDrive tape(argv[1], "w");
  if (!tape) {
    cerr << "Cannot open file " << argv[1] << endl;
    exit(1);
  }

  const int numR = atoi(argv[2]);
  const int recSize = 200;
  char buffer[4096];

  if (numR < 1) {
    cerr << "Illegal number of records: " << numR << endl;
    exit(1);
  }

  memset(buffer, 'a', recSize - 1);
  buffer[recSize - 1] = '\n';
  buffer[recSize] = '\0';

  for(int i = 0; i < numR; i++) {
    int bytes = tape.append(buffer, recSize);
    if (bytes != recSize)
      cerr << "Tape says it wrote " << bytes << " bytes, not "
	   << recSize << endl;
  }

  cout << "Seeking to beginning of file" << endl;
  tape.seek(0);

  int numRecs = 0;
  int totSize = 0;
  int size;

  while((size = tape.read(buffer, sizeof buffer)) > 0) {
    numRecs++;
    totSize += size;
  }

  cout << endl;

  if (numRecs != numR)
    cerr << "Number of records read differs from records written: "
         << numRecs << " vs. " << numR << endl;

  cout << "File has " << numRecs << " records, "
       << totSize << " total bytes, "
       << 1.0 * totSize / numRecs << " bytes per record." << endl;

  return 0;
}
