/* CompositeParser */
#include <stdio.h>
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Exit.h"

CompositeEntry CompositeParser::_entries[MAX_COMPOSITE_ENTRIES];
int CompositeParser::_numEntries = 0;
int CompositeParser::_hintIndex = -1;

void CompositeParser::Register(char *fileType, UserComposite *userComposite){
	if (_numEntries >= MAX_COMPOSITE_ENTRIES){
		fprintf(stderr,"CompositeParser:: too many entries\n");
		Exit::DoExit(2);
	}
	_entries[_numEntries].fileType = fileType;
	_entries[_numEntries].userComposite = userComposite;
	_numEntries++;
}

void CompositeParser::Decode(char *fileType, RecInterp *recInterp){
	int i;
	if (_hintIndex >= 0 && strcmp(fileType,_entries[_hintIndex].fileType) == 0){
		/* found it */
		_entries[_hintIndex].userComposite->
			Decode(recInterp);
		return;
	}
	else {
		/* search for a matching file type */
		for (i=0; i < _numEntries; i++){
			if (strcmp(fileType,_entries[i].fileType) == 0){
				/* found it */
				_hintIndex = i;
				_entries[_hintIndex].userComposite-> Decode(recInterp);
				return;
			}
		}
	}

	/* not found */
	fprintf(stderr,"can't find user composite function for file type %s\n",
		fileType);
	Exit::DoExit(2);
}
