/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ViewDir (view direction?).
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/03/18 19:27:22  wenger
  Zhenhai forgot to commit this.

 */


#ifndef ViewDir_h_
#define ViewDir_h_

#include <string.h>

// viewing direction
// need modification later
enum ViewDir {PosX, PosY, PosZ, NegX, NegY, NegZ};
inline char * ViewDir2Char(ViewDir const dir) {
  switch(dir) {
	case PosX: return "PosX";
	case PosY: return "PosY";
	case PosZ: return "PosZ";
	case NegX: return "NegX";
	case NegY: return "NegY";
	case NegZ: return "NegZ";
  	default:
		return 0;
  }
}

inline int Char2ViewDir(ViewDir &dir, char const * const x) {
  if (strcasecmp(x,"PosX")==0) {
	dir=PosX;
  }
  else if (strcasecmp(x,"PosY")==0) {
	dir=PosY;
  }
  else if (strcasecmp(x,"PosZ")==0) {
	dir=PosZ;
  }
  else if (strcasecmp(x,"NegX")==0) {
	dir=NegX;
  }
  else if (strcasecmp(x,"NegY")==0) {
	dir=NegY;
  }
  else if (strcasecmp(x,"NegZ")==0) {
	dir=NegZ;
  }
  else
      return 0;
  return 1;
}

#endif // ViewDir_h_
