/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.4  1997/08/21 21:04:51  donjerko
  Implemented view materialization

  Revision 1.3  1997/04/22 15:25:25  wenger
  Conditionaled out lots of debug code; fixed data source visualization
  window so the window for the data again defaults to 'New' if there are
  no windows.

  Revision 1.2  1997/03/28 16:07:35  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <string.h>
#include "CatalogComm.h"
#include "TDataDQLInterp.h"
#include "GroupDir.h"
#include "Control.h"

extern GroupDir* gdir;	// defined in graphics.new/ParseCat.c
extern InsertCatFile(char* tableName);	// defined in graphics.new/ParseCat.c

char* dteImportFileType(char* name){

#if defined(DEBUG)
	cout << "in dteImportFileType(" << name << ")\n";
#endif
	string query = "dummy";

	gdir->add_entry(name);
	TDataDQLInterpClassInfo* DQLclass;
	DQLclass = new TDataDQLInterpClassInfo(name, query.c_str());
	
	ControlPanel::RegisterClass(DQLclass,true);
	InsertCatFile(strdup(name));
	
	/*
	if (Init::PrintTDataAttr()){
#if defined(DEBUG)
		cout << "Should print attributes ?????????????????????" << endl;
		//attrs->Print();
#endif
	}
	*/

	if (gdir->num_topgrp(name) == 0)
	{
		Group *newgrp = new Group("__default", NULL, TOPGRP);
		gdir->add_topgrp(name,newgrp);

		char* attrs = dteListAttributes(name);
		for(char* currAtt = strtok(attrs, " "); currAtt; 
						currAtt = strtok(NULL, " ")){
			newgrp->insert_item(strdup(currAtt));
		}
		delete attrs;
	}
	return name;
}

