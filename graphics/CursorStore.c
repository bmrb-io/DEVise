
#include <iostream.h>
#include "CursorStore.h"

CursorStore::CursorStore(): color_index(0), depth(0), _tot(-1) {}

CursorStore::~CursorStore() {
  if (color_index) delete [] color_index;
  if (depth) delete [] depth;
}

CursorStore::Allocate(int width, int height) {
  if (_tot != width*height) {
    if (color_index) delete [] color_index;
    if (depth) delete [] depth;
    color_index = new GLfloat[width*height];
    depth = new GLfloat[width*height];
    _tot  = width*height;
  }
}

CursorStore::Dump() {
  
  cout << "CursorStore Dump " << endl;
  for(int i=0; i<_tot; i++) {
     if ( i % 10 == 0) cout << endl;
     cout << color_index[i] <<"\t"; 
  }
  cout << endl;
}

