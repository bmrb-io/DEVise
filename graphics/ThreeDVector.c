#include "ThreeDVector.h"
#include <math.h>

ThreeDVec::ThreeDVec(Coord x1, Coord y1, Coord z1)
  :x(x1),y(y1),z(z1)
{}

ThreeDVec::ThreeDVec(ThreeDVec const & a)
  :x(a.z),y(a.y),z(a.z)
{}

ThreeDVec::ThreeDVec()
  :x(0),y(0),z(0)
{}

ThreeDVec & ThreeDVec::operator =(ThreeDVec const &a)
{
  x=a.x;
  y=a.y;
  z=a.z;
  return *this;
}

ThreeDVec ThreeDVec::crossprod(ThreeDVec const &a, ThreeDVec const &b)
{
  ThreeDVec answer;
  answer.x=a.y*b.z-a.z*b.y;
  answer.y=a.z*b.x-a.x*b.z;
  answer.z=a.x*b.y-a.y*b.x;
  return answer;
}

Coord ThreeDVec::dotprod(ThreeDVec const &a, ThreeDVec const &b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
}

Coord ThreeDVec::len()
{
  return sqrt(x*x+y*y+z*z);
}

int ThreeDVec::normalize()
{
  Coord length=len();
  if (length==0)
    return 0;
  else {
    x/=length;
    y/=length;
    z/=length;
    return 1;
  }
}

ThreeDVec operator *(ThreeDVec const & a, Coord s)
{
  ThreeDVec answer;
  answer.x=a.x*s;
  answer.y=a.y*s;
  answer.z=a.z*s;
  return answer;
}

ThreeDVec operator /(ThreeDVec const & a, Coord s)
{
  ThreeDVec answer;
  answer.x=a.x/s;
  answer.y=a.y/s;
  answer.z=a.z/s;
  return answer;
}

ThreeDVec operator +(ThreeDVec const & a, ThreeDVec const &b)
{
  ThreeDVec answer;
  answer.x=a.x+b.x;
  answer.y=a.y+b.y;
  answer.z=a.z+b.z;
  return answer;
}

ThreeDVec operator -(ThreeDVec const & a, ThreeDVec const & b)
{
  ThreeDVec answer;
  answer.x=a.x-b.x;
  answer.y=a.y-b.y;
  answer.z=a.z-b.z;
  return answer;
}

ThreeDVec operator -(ThreeDVec const & a)
{
  ThreeDVec answer;
  answer.x=-a.x;
  answer.y=-a.y;
  answer.z=-a.z;
  return answer;
}
