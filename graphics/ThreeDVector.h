#ifndef THREEDVECTOR_H
#define THREEDVECTOR_H

#include "DeviseTypes.h"

struct ThreeDVec {
  ThreeDVec(Coord x, Coord y, Coord z);
  ThreeDVec(ThreeDVec const &a);
  ThreeDVec();

  static ThreeDVec crossprod(ThreeDVec const &a, ThreeDVec const &b);
  static Coord dotprod(ThreeDVec const &a, ThreeDVec const &b);

  Coord len();
  int normalize();
  // return 1 if succeeded
  // return 0 if failed

  ThreeDVec & operator = (ThreeDVec const &a);

  friend ThreeDVec operator *(ThreeDVec const & a, Coord s);
  friend ThreeDVec operator /(ThreeDVec const & a, Coord s);
  friend ThreeDVec operator +(ThreeDVec const & a, ThreeDVec const &b);
  friend ThreeDVec operator -(ThreeDVec const & a);

  Coord x, y, z;
};

#endif
