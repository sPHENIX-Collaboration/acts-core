// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// AlgebraHelper.h, ACTS project
///////////////////////////////////////////////////////////////////

#ifndef ACTS_HELPERS_H
#define ACTS_HELPERS_H

// STL include(s)
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

// ACTS include(s)
#include "Definitions.hpp"

#ifndef ACTS_BIT_CODING
#define ACTS_BIT_CODING 1
#define ACTS_BIT_SHIFT(mask) (__builtin_ffsl(mask)-1)
#define ACTS_BIT_DECODE(id, mask) id << ACTS_BIT_SHIFT(mask)
#define ACTS_BIT_ENCODE(id, mask) (id && mask) >> ACTS_BIT_SHIFT(mask)
#endif 

/** Geometry primitives helper functions
 */

namespace Acts {
/** EventPrimitvesToStringConverter

    inline methods for conversion of EventPrimitives (Matrix)
    to std::string.

    This is to enhance formatted screen ouput and for ASCII based
    testing.

    The offset can be used to offset the lines (starting from line 2) wrt to the
    zero position for formatting reasons.


 */

inline double
roundWithPrecision(double val, int precision)
{
  if (val < 0 && std::abs(val) * std::pow(10, precision) < 1.) return -val;
  return val;
}

inline std::string
toString(const ActsMatrixXd& matrix, int precision = 4, std::string offset = "")
{
  std::ostringstream sout;

  sout << std::setiosflags(std::ios::fixed) << std::setprecision(precision);
  if (matrix.cols() == 1) {
    sout << "(";
    for (int i = 0; i < matrix.rows(); ++i) {
      double val = roundWithPrecision(matrix(i, 0), precision);
      sout << val;
      if (i != matrix.rows() - 1) sout << ", ";
    }
    sout << ")";
  } else {
    for (int i = 0; i < matrix.rows(); ++i) {
      for (int j = 0; j < matrix.cols(); ++j) {
        if (j == 0) sout << "(";
        double val = roundWithPrecision(matrix(i, j), precision);
        sout << val;
        if (j == matrix.cols() - 1)
          sout << ")";
        else
          sout << ", ";
      }
      if (i
          != matrix.rows()
              - 1) {  // make the end line and the offset in the next line
        sout << std::endl;
        sout << offset;
      }
    }
  }
  return sout.str();
}

inline std::string
toString(const Acts::Translation3D& translation, int precision = 4)
{
  Acts::Vector3D trans;
  trans[0] = translation.x();
  trans[1] = translation.y();
  trans[2] = translation.z();
  return toString(trans, precision);
}

inline std::string
toString(const Acts::Transform3D& transform,
         int                      precision = 4,
         std::string              offset    = "")
{
  std::ostringstream sout;
  sout << "Translation : " << toString(transform.translation(), precision)
       << std::endl;
  std::string rotationOffset = offset + "              ";
  sout << offset << "Rotation    : "
       << toString(transform.rotation(), precision + 2, rotationOffset);
  return sout.str();
}

/** calculates the opening angle between two vectors */
inline double
angle(const Acts::Vector3D& v1, const Acts::Vector3D& v2)
{
  double dp = v1.dot(v2);
  dp /= v1.mag() * v2.mag();
  if (dp > 1) dp  = 1;
  if (dp < -1) dp = -1;
  return acos(dp);
}

/** calculates the squared distance between two point in 3D space */
inline float
distance2(const Acts::Vector3D& p1, const Acts::Vector3D& p2)
{
  float dx = p2.x() - p1.x(), dy = p2.y() - p1.y(), dz = p2.z() - p1.z();
  return dx * dx + dy * dy + dz * dz;
}

/** calculates the distance between two point in 3D space */
inline float
distance(const Acts::Vector3D& p1, const Acts::Vector3D& p2)
{
  return std::sqrt(distance2(p1, p2));
}

/** sets the phi angle of a vector without changing theta nor the magnitude */
inline void
setPhi(Acts::Vector3D& v, double phi)
{
  double xy = v.perp();
  v[0]      = xy * cos(phi);
  v[1]      = xy * sin(phi);
}

/** sets the theta and phi angle of a vector without changing the magnitude */
inline void
setThetaPhi(Acts::Vector3D& v, double theta, double phi)
{
  double mag = v.mag();
  v[0]       = mag * sin(theta) * cos(phi);
  v[1]       = mag * sin(theta) * sin(phi);
  v[2]       = mag * cos(theta);
}

/** sets radius, the theta and phi angle of a vector. Angles are measured in
 * RADIANS */
inline void
setRThetaPhi(Acts::Vector3D& v, double r, double theta, double phi)
{
  v[0] = r * sin(theta) * cos(phi);
  v[1] = r * sin(theta) * sin(phi);
  v[2] = r * cos(theta);
}

/** sets the theta of a vector without changing phi nor the magnitude */
inline void
setTheta(Acts::Vector3D& v, double theta)
{
  setThetaPhi(v, theta, v.phi());
}

/** scales the vector in the xy plane without changing the z coordinate nor the
 * angles */
inline void
setPerp(Acts::Vector3D& v, double perp)
{
  double p = v.perp();
  if (p != 0.0) {
    double scale = perp / p;
    v[0] *= scale;
    v[1] *= scale;
  }
}

/** scales the vector length without changing the angles */
inline void
setMag(Acts::Vector3D& v, double mag)
{
  double p = v.mag();
  if (p != 0.0) {
    double scale = mag / p;
    v[0] *= scale;
    v[1] *= scale;
    v[2] *= scale;
  }
}
inline double
deltaPhi(const Acts::Vector3D& v1, const Acts::Vector3D& v2)
{
  double dphi = v2.phi() - v1.phi();
  if (dphi > M_PI) {
    dphi -= M_PI * 2;
  } else if (dphi <= -M_PI) {
    dphi += M_PI * 2;
  }
  return dphi;
}
inline double
deltaR(const Acts::Vector3D& v1, const Acts::Vector3D& v2)
{
  double a = v1.eta() - v2.eta();
  double b = deltaPhi(v1, v2);
  return sqrt(a * a + b * b);
}

/*
 * the analogous to CLHEP HepGeom::Transform3D trans (localRot,
 * theSurface.transform().translation());
 */
inline Acts::Transform3D
getTransformFromRotTransl(Acts::RotationMatrix3D rot, Acts::Vector3D transl_vec)
{
  Acts::Transform3D trans = Acts::Transform3D::Identity();
  trans                   = trans * rot;
  trans.translation()     = transl_vec;
  return trans;
}

/*
 * Replacing the CLHEP::HepRotation::getAngleAxis() functionality
 *
 * Note:
 * CLHEP has a 'HepRotation::getAngleAxis()' function, e.g.:
 * ---
 * CLHEP::HepRotation rotation   = transform.getRotation();
 * CLHEP::Hep3Vector  rotationAxis;
 * double      rotationAngle;
 * rotation.getAngleAxis(rotationAngle,rotationAxis);
 * ---
 */
inline void
getAngleAxisFromRotation(Acts::RotationMatrix3D& rotation,
                         double&                 rotationAngle,
                         Acts::Vector3D&         rotationAxis)
{
  rotationAngle = 0.;

  double xx = rotation(0, 0);
  double yy = rotation(1, 1);
  double zz = rotation(2, 2);

  double cosa  = 0.5 * (xx + yy + zz - 1);
  double cosa1 = 1 - cosa;

  if (cosa1 <= 0) {
    rotationAngle = 0;
    rotationAxis  = Acts::Vector3D(0, 0, 1);
  } else {
    double x = 0, y = 0, z = 0;
    if (xx > cosa) x = sqrt((xx - cosa) / cosa1);
    if (yy > cosa) y = sqrt((yy - cosa) / cosa1);
    if (zz > cosa) z = sqrt((zz - cosa) / cosa1);
    if (rotation(2, 1) < rotation(1, 2)) x = -x;
    if (rotation(0, 2) < rotation(2, 0)) y = -y;
    if (rotation(1, 0) < rotation(0, 1)) z = -z;
    rotationAngle = (cosa < -1.) ? acos(-1.) : acos(cosa);
    rotationAxis  = Acts::Vector3D(x, y, z);
  }

  return;
}

/**
 * Get the Translation vector out of a Transformation
 */
inline Acts::Vector3D
getTranslationVectorFromTransform(const Acts::Transform3D& tr)
{
  return Acts::Vector3D(tr(0, 3), tr(1, 3), tr(2, 3));
}
// TODO: check! it's perhaps useless, you acn use the transform.translation()
// method

/**
 * get a AngleAxis from an angle and an axis.
 *
 * to replace the CLHEP constructor:
 * CLHEP::Rotate3D::Rotate3D(double a, cconst Vector3D< double > & v)
 */
inline Acts::Rotation3D
getRotation3DfromAngleAxis(double angle, Acts::Vector3D& axis)
{
  AngleAxis3D t;
  t = Eigen::AngleAxis<double>(angle, axis);

  Acts::Rotation3D rot;
  rot = t;

  return rot;
}

/**
 * get a rotation transformation around X-axis
 */
inline Acts::Transform3D
getRotateX3D(double angle)
{
  Acts::Transform3D transf;
  Acts::AngleAxis3D angleaxis(angle, Acts::Vector3D(1., 0., 0.));
  transf = angleaxis;
  return transf;
}
/**
 * get a rotation transformation around Y-axis
 */
inline Acts::Transform3D
getRotateY3D(double angle)
{
  Acts::Transform3D transf;
  Acts::AngleAxis3D angleaxis(angle, Acts::Vector3D(0., 1., 0.));
  transf = angleaxis;
  return transf;
}
/**
 * get a rotation transformation around Z-axis
 */
inline Acts::Transform3D
getRotateZ3D(double angle)
{
  Acts::Transform3D transf;
  Acts::AngleAxis3D angleaxis(angle, Acts::Vector3D(0., 0., 1.));
  transf = angleaxis;
  return transf;
}

}  // end of Acts namespace

#endif
