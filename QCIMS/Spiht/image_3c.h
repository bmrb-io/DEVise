
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//                     I M A G E   C L A S S

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//           > > > >    C++ version  1.04 -  06/25/95   < < < <

// Amir Said - amir@densis.fee.unicamp.br
// University of Campinas (UNICAMP)
// Campinas, SP 13081, Brazil

// William A. Pearlman - pearlman@ecse.rpi.edu
// Rensselaer Polytechnic Institute
// Troy, NY 12180, USA

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Copyright (c) 1995 Amir Said & William A. Pearlman

// This program is Copyright (c) by Amir Said & William A. Pearlman.
// It may be freely redistributed in its entirety provided that this
// copyright notice is not removed. It may not be sold for profit or
// incorporated in commercial programs without the written permission
// of the copyright holders. This program is provided as is, without any
// express or implied warranty, without even the warranty of fitness
// for a particular purpose.


// - - External definitions  - - - - - - - - - - - - - - - - - - - - - -

struct Image_Coord { int x, y; };


// - - Class definition  - - - - - - - - - - - - - - - - - - - - - - - -

class Image_3C
{
  // . private data .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    Image_Coord dim, pdim;

    int levels, mean[3];

    float R[3][3];

    float * r_line, * t_line, ** coeff, ** cor[3];

  // . private functions  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    int max_levels(int);

    void assign_mem(Image_Coord);

    void free_mem(void);

    void w_transform(float **);

    void w_recover(float **);

    void extend_image(int);

    void Jacobi(double m[3], double s[3][3]);

  // . constructor and destructor  .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Image_3C(void) { levels = -1; }

    ~Image_3C(void) { free_mem(); }

  // . public functions   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    float & operator[](const Image_Coord & c) {
      return coeff[c.x][c.y]; }

    float & operator()(int i, int j) {
      return coeff[i][j]; }

    float * address(const Image_Coord & c) {
      return coeff[c.x] + c.y; }

    Image_Coord dimension(void) { return dim; }

    Image_Coord pyramid_dim(void) { return pdim; }

    int pyramid_levels(void) { return levels; }

    void set_color(int);

    void averages(int M[3]);

    void color_transform(int T[3][3]);

    float compare(char * file_name);

    void read_pic(Image_Coord, char * file_name);

    void write_pic(char * file_name);

    void reset(Image_Coord);

    void reset(Image_Coord, int m[3], int T[3][3]);

    void transform(void);

    void recover(void);

    void dispose(void) { free_mem(); }

};  // end definition of class  < Image_3C >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file  < Image_3C.H >
