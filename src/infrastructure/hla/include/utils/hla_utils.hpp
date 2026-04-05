#ifndef HLA_UTILS_HPP
#define HLA_UTILS_HPP

#include <math.h>
#include <cstring> 

//////////////////////////////////////////////////////////////////////////
template <class Type>
inline void RotateAboutAxis(Type d[3], Type const s[3], Type const n[3],
                            Type t) {
  double st = sin(t);
  double ct = cos(t);

  d[0] =
      static_cast<Type>((1.0 - ct) * (n[0] * n[0] * s[0] + n[0] * n[1] * s[1] +
                                      n[0] * n[2] * s[2]) +
                        ct * s[0] + st * (n[1] * s[2] - n[2] * s[1]));
  d[1] =
      static_cast<Type>((1.0 - ct) * (n[0] * n[1] * s[0] + n[1] * n[1] * s[1] +
                                      n[1] * n[2] * s[2]) +
                        ct * s[1] + st * (n[2] * s[0] - n[0] * s[2]));
  d[2] =
      static_cast<Type>((1.0 - ct) * (n[0] * n[2] * s[0] + n[1] * n[2] * s[1] +
                                      n[2] * n[2] * s[2]) +
                        ct * s[2] + st * (n[0] * s[1] - n[1] * s[0]));
}

template <class Type>
inline void Cross(Type d[3], Type const a[3], Type const b[3]) {
  d[0] = a[1] * b[2] - b[1] * a[2];
  d[1] = b[0] * a[2] - a[0] * b[2];
  d[2] = a[0] * b[1] - b[0] * a[1];
}

template <class Type>
inline Type Dot(Type const a[3], Type const b[3]) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

//************************************
// FullName:    KDIS::UTILS<Type>::HeadingPitchRollToEuler
// Description: Converts Heading, Pitch and Roll to Euler for DIS.
// Parameter:   Type H - Heading in radians
// Parameter:   Type P - Pitch in radians
// Parameter:   Type R - Roll in radians
// Parameter:   Type Lat - Geodetic Latitude in radians
// Parameter:   Type Lon - Geodetic Longitude in radians
// Parameter:   Type & Psi - Euler angle out
// Parameter:   Type & Theta - Euler angle out
// Parameter:   Type & Phi - Euler angle out
//************************************

template <class Type>
inline void HeadingPitchRollToEuler(Type H, Type P, Type R, Type Lat, Type Lon,
                                    Type& Psi, Type& Theta, Type& Phi) {
  // local NED
  // Type const  D0[3] = { 1.0 , 0.0 , 0.0 };
  Type const E0[3] = {0.0, 1.0, 0.0};
  Type const N0[3] = {0.0, 0.0, 1.0};
  Type me[3];
  Type N[3];
  Type E[3];
  Type D[3];
  // 'E'
  RotateAboutAxis(E, E0, N0, Lon);
  me[0] = -E[0];
  me[1] = -E[1];
  me[2] = -E[2];
  // 'N'
  RotateAboutAxis(N, N0, me, Lat);
  // 'D'
  Cross(D, N, E);
  /*
   *  Orientation
   */
  // rotate about D by heading
  Type N1[3], E1[3], D1[3];
  RotateAboutAxis(N1, N, D, H);
  RotateAboutAxis(E1, E, D, H);
  memcpy(D1, D, sizeof(Type[3]));
  // rotate about E1 vector by pitch
  Type N2[3], E2[3], D2[3];
  RotateAboutAxis(N2, N1, E1, P);
  memcpy(E2, E1, sizeof(Type[3]));
  RotateAboutAxis(D2, D1, E1, P);
  // rotate about N2 by roll
  Type N3[3], E3[3], D3[3];
  memcpy(N3, N2, sizeof(Type[3]));
  RotateAboutAxis(E3, E2, N2, R);
  RotateAboutAxis(D3, D2, N2, R);

  // calculate angles from vectors
  Type x0[3] = {1.0, 0.0, 0.0};  // == D0
  Type y0[3] = {0.0, 1.0, 0.0};  // == E0
  Type z0[3] = {0.0, 0.0, 1.0};  // == Z0
  Type y2[3];
  Type z2[3];
  Psi = static_cast<Type>(atan2(Dot(N3, y0), Dot(N3, x0)));
  Theta = static_cast<Type>(
      atan2(-Dot(N3, z0), sqrt(pow(Dot(N3, x0), 2) + pow(Dot(N3, y0), 2))));
  RotateAboutAxis(y2, y0, z0, Psi);
  RotateAboutAxis(z2, z0, y2, Theta);
  Phi = static_cast<Type>(atan2(Dot(E3, z2), Dot(E3, y2)));
}

//////////////////////////////////////////////////////////////////////////

//************************************
// FullName:    KDIS::UTILS<Type>::EulerToHeadingPitchRoll
// Description: Converts Euler to Heading, Pitch and Roll.
// Parameter:   Type Lat - Geodetic Latitude in radians
// Parameter:   Type Lon - Geodetic Longitude in radians
// Parameter:   Type Psi - Euler angle
// Parameter:   Type Theta - Euler angle
// Parameter:   Type Phi - Euler angle
// Parameter:   Type & H - Heading in radians out
// Parameter:   Type & P - Pitch in radians out
// Parameter:   Type & R - Roll in radians out
//************************************

template <class Type>
void EulerToHeadingPitchRoll(Type Lat, Type Lon, Type Psi, Type Theta, Type Phi,
                             Type& H, Type& P, Type& R) {
  // local NED vectors in ECEF coordinate frame
  Type N[3];
  Type E[3];
  Type D[3];

  // Calculate NED from lat and lon
  // local NED
  // Type const  D0[3] = { 1. , 0. , 0. };
  Type const E0[3] = {0., 1., 0.};
  Type const N0[3] = {0., 0., 1.};
  Type me[3];
  // 'E'
  RotateAboutAxis(E, E0, N0, Lon);
  me[0] = -E[0];
  me[1] = -E[1];
  me[2] = -E[2];
  // 'N'
  RotateAboutAxis(N, N0, me, Lat);
  // 'D'
  Cross(D, N, E);
  /*
   *  Orientation:
   *    input : (x0,y0,z0)=(N,E,D) and (Psi,Theta,Phi Euler angles)
   *    output: (x3,y3,z3)=body vectors in local frame
   */
  // rotate about Z by Psi
  Type X[] = {1., 0., 0.};
  Type Y[] = {0., 1., 0.};
  Type Z[] = {0., 0., 1.};
  Type X1[3], Y1[3], Z1[3];
  RotateAboutAxis(X1, X, Z, Psi);
  RotateAboutAxis(Y1, Y, Z, Psi);
  memcpy(Z1, Z, sizeof(Type[3]));
  // rotate about Y1 vector by Theta
  Type X2[3], Y2[3], Z2[3];
  RotateAboutAxis(X2, X1, Y1, Theta);
  memcpy(Y2, Y1, sizeof(Type[3]));
  RotateAboutAxis(Z2, Z1, Y1, Theta);
  // rotate about X2 by Phi
  Type X3[3], Y3[3], Z3[3];
  memcpy(X3, X2, sizeof(Type[3]));
  RotateAboutAxis(Y3, Y2, X2, Phi);
  RotateAboutAxis(Z3, Z2, X2, Phi);
  // calculate angles from vectors
  Type x0[3];
  memcpy(x0, N, sizeof(Type[3]));
  Type y0[3];
  memcpy(y0, E, sizeof(Type[3]));
  Type z0[3];
  memcpy(z0, D, sizeof(Type[3]));
  Type y2[3];
  Type z2[3];
  H = static_cast<Type>(atan2(Dot(X3, y0), Dot(X3, x0)));
  P = static_cast<Type>(
      atan2(-Dot(X3, z0), sqrt(pow(Dot(X3, x0), 2) + pow(Dot(X3, y0), 2))));
  RotateAboutAxis(y2, y0, z0, H);
  RotateAboutAxis(z2, z0, y2, P);
  R = static_cast<Type>(atan2(Dot(Y3, z2), Dot(Y3, y2)));
}

#endif 
