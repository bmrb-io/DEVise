
#ifndef CURSORSTORE_H
#define CURSORSTORE_H
#include <GL/gl.h>

class CursorStore {

  public:
    CursorStore();
    ~CursorStore();
    Allocate(int width, int height);
    Dump();
    GLfloat *color_index;
    GLfloat *depth;

  private:
    int _tot;         //total size of CursorStore = width*height
};

#endif
