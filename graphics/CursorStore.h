
#ifndef CURSORSTORE_H
#define CURSORSTORE_H
#include <GL/gl.h>

class CursorStore {

  public:
    CursorStore();
    ~CursorStore();

    int Valid();
    // return 1 if the cursor store needs to be drawn
    // return 0 otherwise

    void Dump();

    // to calculate the region that cursor will cover
    void Clear();
    void OneCoord(int x, int y); // one coord in the region
    void Expand(int min_x, int min_y, int max_x, int max_y);
    // expand the region to cover a bit more to make sure the edges are covered


    int _inited;
    int _need_draw;
    // whether the cursor is inside the window and needs to be drawn

    // the coordinates for lower left corner and upper right corner the
    // 2D region that the cursor covers on the screen
    int _min_x, _min_y, _max_x, _max_y;

    GLfloat *color_index;
    GLfloat *depth;

    int _tot;         //total size of CursorStore = width*height
};

#endif
