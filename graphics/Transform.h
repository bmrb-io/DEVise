/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.8  1996/06/21 19:28:31  jussi
  Cleaned up 3D transformation code.

  Revision 1.7  1996/06/15 21:06:13  yuc
  Clean up some more stuffs in the "view transformation" matrix.

  Revision 1.6  1996/06/15 14:12:21  jussi
  Added yuc's 3D functions.

  Revision 1.5  1996/03/05 23:34:00  jussi
  Added 3D transformation functions.

  Revision 1.4  1995/11/25 01:17:05  jussi
  Added optimization for Cartesian coordinates (perpendicular, no rotation).

  Revision 1.3  1995/11/24 21:29:58  jussi
  Added copyright notice and cleaned up code. Added Print method
  to help in debugging.

  Revision 1.2  1995/09/05 21:13:11  jussi
  Added/updated CVS header.
*/

/* transformation matrix */

#ifndef Transform_h
#define Transform_h

#include <stdio.h>
#include <math.h>

#include "DeviseTypes.h"
#include "VisualArg.h"
#include "Geom.h"

#if !defined(M_PI) && !defined (PI)
#define PI 3.14159265358979323846
#else
#define PI M_PI 
#endif

#define rad_deg(r) ((r) / PI * 180.0) // change radian -> degree
#define deg_rad(d) ((d) * PI / 180.0) // change degree -> radian
#define SQUARE(a) ((a) * (a))

inline Coord ToRadian(Coord degree)
{
  return degree * 2 * PI / 360.0;
}

inline Coord ToDegree(Coord radian)
{
  return radian * 360.0 / (2 * PI);
};

/* 2D transformation matrix, ala Foley et al, 1990 */
// Chapter 5

class Transform2D {
public:
  /* identity matrix */
  Transform2D() {
    MakeIdentity();
  }

  /* Copy from another matrix */
  Transform2D(Transform2D *t) {
    t->GetCoords(_a00,_a01,_a02,_a10,_a11,_a12);
    _cartesian = t->_cartesian;
  }

  /* Copy from another mattrix */
  void Copy(Transform2D &t) {
    t.GetCoords(_a00,_a01,_a02,_a10,_a11,_a12);
    _cartesian = t._cartesian;
  }

  /* Clear the matrix into an identity matrix */
  void MakeIdentity() {
    _a00 = 1.0; _a01 = 0.0, _a02 = 0.0, _a10 = 0.0; _a11 = 1.0; _a12 = 0.0;
    _cartesian = true;
  }

  /* Translate by dx,dy */
  void Translate(Coord dx, Coord dy) {
    _a02 += dx; _a12 += dy;
  }

  /* Scale by sx, sy */
  void Scale(Coord sx, Coord sy) {
    _a00 *= sx; _a01 *= sx; _a02 *= sx;
    _a10 *= sy; _a11 *= sy; _a12 *= sy;
  }

  /* Rotate by theta. counter-clockwise = positive theta. */
  void Rotate(Coord theta) {
    Coord cos_theta = cos(theta);
    Coord sin_theta = sin(theta);

    Coord temp = _a00;
    _a00 = cos_theta*temp-sin_theta*_a10;
    _a10 = sin_theta*temp+cos_theta*_a10;
    
    temp = _a01;
    _a01 = cos_theta*temp-sin_theta*_a11;
    _a11 = sin_theta*temp+cos_theta*_a11;
    
    temp = _a02;
    _a02 = cos_theta*temp-sin_theta*_a12;
    _a12 = sin_theta*temp+cos_theta*_a12;

    if (_a01 == 0.0 && _a10 == 0.0)
      _cartesian = true;
    else
      _cartesian = false;
  }

  /* Return the transformed X,Y coordinates */
  void Transform(Coord x, Coord y, Coord &newX, Coord &newY) {
    TransformX(x, y, newX);
    TransformY(y, y, newY);
  }

  /* Return the transformed X coordinate, optimized for speed */
  void TransformX(Coord x, Coord y, Coord &newX) {
    if (_cartesian)
      newX = _a00 * x + _a02;
    else
      newX = _a00 * x + _a01 * y + _a02;
  }

  /* Return the transformed Y coordinate, optimized for speed */
  void TransformY(Coord x, Coord y, Coord &newY) {
    if (_cartesian)
      newY = _a11 * y + _a12;
    else
      newY = _a10 * x + _a11 * y + _a12;
  }

  /* Find the original coord given transformed coordinate x,y.
     Inverse of matrix
     | a00 a01 a02|                        | a11  -a01 a01*a12-a02*a11 |
     | a10 a11 a12| = 1/(a00*a11-a01*a10)* | -a10 a00  a10*a02-a12*a00 |
     | 0    0   1 |                        | 0     0   a00*a11-a01*a10 |
  */
  void InverseTransform(Coord x, Coord y, Coord &oldX, Coord &oldY) {
    Coord temp = 1.0/(_a00*_a11-_a01*_a10);
    oldX = (x*_a11- y*_a01 + _a01*_a12-_a02*_a11)*temp;
    oldY = (-x*_a10+y*_a00+_a10*_a02-_a12*_a00)*temp;
  }

  /* Multiply other matrix after this one matrix. Store the
     result in this */
  void PostMultiply(Transform2D *other) {
    Coord o00,o01,o02,o10,o11,o12;
    other->GetCoords(o00,o01,o02,o10,o11,o12);
    
    _a02 = _a00*o02+ _a01*o12 + _a02;
    Coord temp = _a01;
    _a01 = _a00*o01+ _a01*o11;
    _a00 = _a00*o00+ temp*o10;
    
    _a12 = _a10*o02+ _a11*o12 + _a12;
    temp = _a11;
    _a11 = _a10*o01+ _a11*o11;
    _a10 = _a10*o00+ temp*o10;

    if (_a01 == 0.0 && _a10 == 0.0)
      _cartesian = true;
    else
      _cartesian = false;
  }

  /* Multiply other matrix before this. Store the result in this. */
  void PreMultiply(Transform2D *other) {
    Coord o00,o01,o02,o10,o11,o12;
    other->GetCoords(o00,o01,o02,o10,o11,o12);
    
    Coord temp = _a00;
    _a00 = o00*_a00+ o01*_a10;
    _a10 = o10*temp+ o11*_a10;
    
    temp = _a01;
    _a01 = o00*_a01+ o01*_a11;
    _a11 = o10*temp+ o11*_a11;
    
    temp = _a02;
    _a02 = o00*_a02+ o01*_a12 + o02;
    _a12 = o10*temp+ o11*_a12 + o12;

    if (_a01 == 0.0 && _a10 == 0.0)
      _cartesian = true;
    else
      _cartesian = false;
  }
  
  void GetCoords(Coord &a00, Coord &a01, Coord &a02, Coord &a10, Coord &a11,
		 Coord &a12) {
    a00 = _a00; a01 = _a01; a02 = _a02; a10 = _a10; a11 = _a11; a12 = _a12;
  }
  
  void Print() {
    printf("(%.2f,%.2f,%.2f),(%.2f,%.2f,%.2f),%d", _a00, _a01, _a02,
	   _a10, _a11, _a12, (_cartesian ? 1 : 0));
  }

private:
  Coord _a00,_a01,_a02,_a10,_a11,_a12;  // transformation matrix
  Boolean _cartesian;                   // TRUE if cartesian transformation
                                        // (a01 == a10 == 0.0)
};

// ---------------------------------------------------------- 
// transformation matrix = T
//    T = [[a00, a01, a02, a03],
//         [a10, a11, a12, a13],
//         [a20, a21, a22, a23],
//         [a30, a31, a32, a33]]
//    new matrix = old matrix * (x, y, z, 1)^-1
// NOTE: translation, scale, and rotate functions are used to update
//       the transformation matrix, T_new * T_old -> T_stored_by_class

class Transform3D {
public:
  /* identity matrix */
  Transform3D() {
    MakeIdentity();
  }

  /* Copy from another matrix */
  Transform3D(Transform3D *t) {
    t->GetCoords(_a00,_a01,_a02,_a03,
                 _a10,_a11,_a12,_a13,
                 _a20,_a21,_a22,_a23,
                 _a30,_a31,_a32,_a33);
    _cartesian = t->_cartesian;
  }

  /* Copy from another mattrix */
  void Copy(Transform3D &t) {
    t.GetCoords(_a00,_a01,_a02,_a03,
                _a10,_a11,_a12,_a13,
                _a20,_a21,_a22,_a23,
                _a30,_a31,_a32,_a33);
    _cartesian = t._cartesian;
  }

  /* Clear the matrix into an identity matrix */
  void MakeIdentity() {
    _a00 = 1.0; _a01 = _a02 = _a03 = 0.0;
    _a11 = 1.0; _a10 = _a12 = _a13 = 0.0;
    _a22 = 1.0; _a20 = _a21 = _a23 = 0.0;
    _a33 = 1.0; _a30 = _a31 = _a32 = 0.0;
    _cartesian = true;
  }

  /* Translate by dx,dy,dz */
  void Translate(Coord dx, Coord dy, Coord dz) {
    _a03 += dx; _a13 += dy; _a23 += dz;
  }

  /* Scale by sx, sy, sz */
  void Scale(Coord sx, Coord sy, Coord sz) {
    _a00 *= sx; _a01 *= sx; _a02 *= sx; _a03 *= sx;
    _a10 *= sy; _a11 *= sy; _a12 *= sy; _a13 *= sy;
    _a20 *= sz; _a21 *= sz; _a22 *= sz; _a23 *= sz;
  }

  /* Rotate by theta. counter-clockwise = positive theta. */
  void Rotate(Coord theta) {
  }

  /* Return the transformed X,Y,Z coordinates */
  void Transform(Coord x, Coord y, Coord z, 
		 Coord &newX, Coord &newY, Coord &newZ) {
    TransformX(x, y, z, newX);
    TransformY(x, y, z, newY);
    TransformZ(x, y, z, newZ);
  }

  /* Return the transformed X coordinate, optimized for speed */
  void TransformX(Coord x, Coord y, Coord z, Coord &newX) {
    if (_cartesian)
      newX = _a00 * x + _a03;
    else
      newX = _a00 * x + _a01 * y + _a02 * z + _a03;
  }

  /* Return the transformed Y coordinate, optimized for speed */
  void TransformY(Coord x, Coord y, Coord z, Coord &newY) {
    if (_cartesian)
      newY = _a11 * y + _a13;
    else
      newY = _a10 * x + _a11 * y + _a12 * z + _a13;
  }

  /* Return the transformed Z coordinate, optimized for speed */
  void TransformZ(Coord x, Coord y, Coord z, Coord &newZ) {
    if (_cartesian)
      newZ = _a22 * z + _a23;
    else
      newZ = _a20 * x + _a21 * y + _a22 * z + _a23;
  }

  /* Find the original coord given transformed coordinate x,y,z. */
  void InverseTransform(Coord x, Coord y, Coord z,
			Coord &oldX, Coord &oldY, Coord &oldZ) {
    oldX = x; oldY = y; oldZ = z;
  }

  /* Multiply other matrix after this one matrix. Store the
     result in this */
  void PostMultiply(Transform3D *other) {
    Coord o00,o01,o02,o03,o10,o11,o12,o13,
          o20,o21,o22,o23,o30,o31,o32,o33;
    other->GetCoords(o00,o01,o02,o03, o10,o11,o12,o13,
		     o20,o21,o22,o23, o30,o31,o32,o33);
    
    _a03 = _a00*o02+ _a01*o12 + _a02*o23 + _a03;
    Coord temp1 = _a01, temp2 = _a02;
    _a02 = _a00*o02+ _a01*o12 + _a02*o22;
    _a01 = _a00*o01+ _a01*o11 + temp2*o21;
    _a00 = _a00*o00+ temp1*o10 + temp2*o20;
    
    _a13 = _a10*o03+ _a11*o13 + _a12*o23 + _a13;
    temp1 = _a11; temp2 = _a12;
    _a12 = _a10*o02 + _a11*o12 + _a12*o22;
    _a11 = _a10*o01 + _a11*o11 + temp2*o21;
    _a10 = _a10*o00+ temp1*o10 + temp2*o20;

    _a23 = _a20*o03+ _a21*o13 + _a22*o23 + _a23;
    temp1 = _a21; temp2 = _a22;
    _a22 = _a20*o02 + _a21*o12 + _a22*o22;
    _a21 = _a20*o01 + _a21*o11 + temp2*o21;
    _a20 = _a20*o00+ temp1*o10 + temp2*o20;

    if (_a01 == 0.0 && _a02 == 0.0 && _a10 == 0.0 && _a12 == 0.0
	&& _a20 == 0.0 && _a22 == 0.0)
      _cartesian = true;
    else
      _cartesian = false;
  }

  /* Multiply other matrix before this. Store the result in this. */
  void PreMultiply(Transform3D *other) {
    Coord o00,o01,o02,o03,o10,o11,o12,o13,
          o20,o21,o22,o23,o30,o31,o32,o33;
    other->GetCoords(o00,o01,o02,o03, o10,o11,o12,o13,
		     o20,o21,o22,o23, o30,o31,o32,o33);
    
    Coord temp1 = _a00, temp2 = _a10;
    _a00 = o00*_a00+ o01*_a10 + o02*_a20;
    _a10 = o10*temp1+ o11*_a10 + o12*_a20;
    _a20 = o20*temp1+ o21*temp2 + o22*_a20;

    temp1 = _a01; temp2 = _a11;
    _a01 = o00*_a01+ o01*_a11 + o02*_a21;
    _a11 = o10*temp1+ o11*_a11 + o12*_a21;
    _a21 = o20*temp1+ o21*temp2 + o22*_a21;
    
    temp1 = _a02; temp2 = _a12;
    _a02 = o00*_a02+ o01*_a12 + o02*_a22;
    _a12 = o10*temp1+ o11*_a12 + o12*_a22;
    _a22 = o20*temp1+ o21*temp2 + o22*_a22;
    
    temp1 = _a03; temp2 = _a13;
    _a03 = o00*_a03+ o01*_a13 + o02*_a23;
    _a13 = o10*temp1+ o11*_a13 + o12*_a23;
    _a23 = o20*temp1+ o21*temp2 + o22*_a23;

    /*  non-option computation, but a good checking method
    Coord A, B, C, D;
    A = o00 * _a00 + o01 * _a10 + o02 * _a20 + o03 * _a30; 
    B = o10 * _a00 + o11 * _a10 + o12 * _a20 + o13 * _a30;
    C = o20 * _a00 + o21 * _a10 + o22 * _a20 + o23 * _a30;
    D = o30 * _a00 + o31 * _a10 + o32 * _a20 + o33 * _a30;
    _a00 = A; _a10 = B; _a20 = C; _a30 = D;
    
    A = o00 * _a01 + o01 * _a11 + o02 * _a21 + o03 * _a31;
    B = o10 * _a01 + o11 * _a11 + o12 * _a21 + o13 * _a31;
    C = o20 * _a01 + o21 * _a11 + o22 * _a21 + o23 * _a31;
    D = o30 * _a01 + o31 * _a11 + o32 * _a21 + o33 * _a31;
    _a01 = A; _a11 = B; _a21 = C; _a31 = D;

    A = o00 * _a02 + o01 * _a12 + o02 * _a22 + o03 * _a32;
    B = o10 * _a02 + o11 * _a12 + o12 * _a22 + o13 * _a32;
    C = o20 * _a02 + o21 * _a12 + o22 * _a22 + o23 * _a32;
    D = o30 * _a02 + o31 * _a12 + o32 * _a22 + o33 * _a32;
    _a02 = A; _a12 = B; _a22 = C; _a32 = D;

    A = o00 * _a03 + o01 * _a13 + o02 * _a23 + o03 * _a33;
    B = o10 * _a03 + o11 * _a13 + o12 * _a23 + o13 * _a33; 
    C = o20 * _a03 + o21 * _a13 + o22 * _a23 + o23 * _a33;
    D = o30 * _a03 + o31 * _a13 + o32 * _a23 + o33 * _a33;
    _a03 = A; _a13 = B; _a23 = C; _a33 = D;
    */

    if (_a01 == 0.0 && _a02 == 0.0 && _a10 == 0.0 && _a12 == 0.0
		&& _a20 == 0.0 && _a22 == 0.0)
      _cartesian = true;
    else
      _cartesian = false;
  }

  void SetViewMatrix(Camera camera) {
    double st = sin(camera._theta);
    double ct = cos(camera._theta);
    double sp = sin(camera._phi);
    double cp = cos(camera._phi);

    // 1st row
    _a00 = ct; _a01 = -cp * st; _a02 = -sp * st; _a03 = 0.0;

    // 2nd row
    _a10 = 0.0; _a11 = sp; _a12 = -cp; _a13 = 0.0;

    // 3rd row
    _a20 = st; _a21 = cp * ct; _a22 = ct * sp; _a23 = 0.0;

    // 4th row
    _a30 = -(camera.fx * ct) + (-camera.fz * st);
    _a31 = -((camera.fy + camera._rho * cp) * sp) + 
             cp * (-camera.fz * ct + camera._rho * sp +
		   camera.fx * st);
    _a32 = cp * (camera.fy + camera._rho * cp) +
                 sp * (-camera.fz * ct + camera._rho * sp +
		       camera.fx * st);
    _a33 = 1.0;

#ifdef DEBUG
    printf ("....................rho = %f   phi = %f   theta = %f\n", 
	    camera._rho, camera._phi, camera._theta);
    Print();
    printf (" >>>>>>>>>>>>>> END set view matrix <<<<<<<<<\n");
#endif
  }

  void SetVector(POINT3D pt) {
    _a00 = pt.x_;
    _a11 = pt.y_;
    _a22 = pt.z_;
    _a33 = 1.0; 
  }
	
  void GetVector(POINT3D &pt) {
#ifdef DEBUG
    printf("   OLD ------- a00 = %f a01 = %f a02 = %f \n",_a00,_a01,_a02);
#endif
    pt.x_ = _a00 + _a10 + _a20 + _a30;
    pt.y_ = _a01 + _a11 + _a21 + _a31;
    pt.z_ = _a02 + _a12 + _a22 + _a32;
#ifdef DEBUG
    printf("   NEW +++++++ pt -> x = %f y = %f z = %f\n",pt.x_,pt.y_,pt.z_);
#endif
  }
  
  void GetCoords(Coord &a00, Coord &a01, Coord &a02, Coord &a03,
		 Coord &a10, Coord &a11, Coord &a12, Coord &a13,
		 Coord &a20, Coord &a21, Coord &a22, Coord &a23,
		 Coord &a30, Coord &a31, Coord &a32, Coord &a33) {
    a00 = _a00; a01 = _a01; a02 = _a02; a03 = _a03;
    a10 = _a10; a11 = _a11; a12 = _a12; a13 = _a13;
    a20 = _a20; a21 = _a21; a22 = _a22; a23 = _a23;
    a30 = _a30; a31 = _a31; a32 = _a32; a33 = _a33;
  }
  
  void PutCoords(Coord bb00, Coord bb01, Coord bb02, Coord bb03,
		 Coord bb10, Coord bb11, Coord bb12, Coord bb13,
		 Coord bb20, Coord bb21, Coord bb22, Coord bb23,
		 Coord bb30, Coord bb31, Coord bb32, Coord bb33) {
    _a00 = bb00; _a01 = bb01; _a02 = bb02; _a03 = bb03;
    _a10 = bb10; _a11 = bb11; _a12 = bb12; _a13 = bb13;
    _a20 = bb20; _a21 = bb21; _a22 = bb22; _a23 = bb23;
    _a30 = bb30; _a31 = bb31; _a32 = bb32; _a33 = bb33;
  }
  
  void Print() {
    printf("\t(%.3f, %.3f, %.3f, %.3f),\n",   _a00, _a01, _a02, _a03);
    printf("\t(%.3f, %.3f, %.3f, %.3f),\n",   _a10, _a11, _a12, _a13);
    printf("\t(%.3f, %.3f, %.3f, %.3f),\n",   _a20, _a21, _a22, _a23);
    printf("\t(%.3f, %.3f, %.3f, %.3f),%d\n", _a30, _a31, _a32, _a33,
	   (_cartesian ? 1 : 0));
  }

private:
  Coord _a00,_a01,_a02,_a03,
        _a10,_a11,_a12,_a13,
        _a20,_a21,_a22,_a23,
	_a30,_a31,_a32,_a33;      // transformation matrix
  Boolean _cartesian;             // TRUE if cartesian transformation
                                  // (a01 == a10 == 0.0)
};

#endif
