#include <iostream.h>
#include "CursorStore.h"
#include "Exit.h"

CursorStore::CursorStore(): _inited(0), _need_draw(1), color_index(0), depth(0), _tot(-1) {}

CursorStore::~CursorStore() {
  if (color_index) delete [] color_index;
  if (depth) delete [] depth;
}

void CursorStore::Dump() {

  DOASSERT(_inited, "CursorStore not inited before used");
  
  cout << "CursorStore Dump " << endl;
  for(int i=0; i<_tot; i++) {
     if ( i % 10 == 0) cout << endl;
     cout << color_index[i] <<"\t"; 
  }
  cout << endl;
}


// to calculate the region that cursor will cover
void CursorStore::Clear() {
  _inited=0;
  _need_draw=1;
}

void CursorStore::OneCoord(int x, int y) // one coord in the region
{
  if (!_inited) {
    _min_x=x;
    _min_y=y;
    _max_x=x;
    _max_y=y;
    _inited=1;
  }
  else {
    if (x<_min_x)
      _min_x=x;
    if (y<_min_y)
      _min_y=y;
    if (x>_max_x)
      _max_x=x;
    if (y>_max_y)
      _max_y=y;
  }
}

void CursorStore::Expand(int min_x, int min_y, int max_x, int max_y)
{

  // expand the region to cover a bit more to make sure the edges are covered

  DOASSERT(_inited, "CursorStore not inited before used");
  _min_x-=2;
  _min_y-=2;
  _max_x+=2;
  _max_y+=2;
  if (_min_x > max_x || _max_x < min_x || _min_y > max_y || _max_y < min_y) {
    _need_draw=0;
    return;
  }
  if (_min_x < min_x)
    _min_x=min_x;
  if (_max_x > max_x)
    _max_x=max_x;
  if (_min_y < min_y)
    _min_y=min_y;
  if (_max_y > max_y)
    _max_y=max_y;

  int width=_max_x-_min_x+1;
  int height=_max_y-_min_y+1;

  if (_tot != width*height) {
    if (color_index) delete [] color_index;
    if (depth) delete [] depth;
    color_index = new GLfloat[width*height];
    depth = new GLfloat[width*height];
    _tot  = width*height;
  }
}

int CursorStore::Valid() {
  return _inited && _need_draw;
}
