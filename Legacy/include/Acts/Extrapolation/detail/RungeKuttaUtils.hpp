// This file is part of the Acts project.
//
// Copyright (C) 2016-2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/////////////////////////////////////////////////////////////////////////////////
//  RungeKuttaUtils.h, Acts project
/////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <limits>
#include <map>
#include <vector>
#include "Acts/EventData/NeutralParameters.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Surfaces/BoundaryCheck.hpp"

namespace Acts {

class ConeSurface;
class DiscSurface;
class PlaneSurface;
class CylinderSurface;
class StraightLineSurface;

/// @class RungeKuttaUtils
///
/// Acts::RungeKuttaUtils is set algorithms for track parameters transformation
///
/// from local to global and global to local system coordinate and step
/// estimation to surface.
///
///   AtaPlane       AtaStraw         AtaDisc       AtaCylinder      Perigee
///      |               |               |               |              |
///      |               |               |               |              |
///      V               V               V               V              V
///      -----------------------------------------------------------------
///                                      |              Local->Global
///                                      transformation
///                                      V
///                   Global position (Runge Kutta presentation)
///                                      |
///                                      V              Global->Local
///                                      transformation
///      ----------------------------------------------------------------
///      |               |               |               |              |
///      |               |               |               |              |
///      V               V               V               V              V
///  PlaneSurface    StrawSurface   DiscSurface CylinderSurface PerigeeSurface
///
/// For using Runge Kutta method we use global coordinate, direction,
/// inverse momentum and Jacobian of transformation. All this parameters we save
/// in array P[42].
///                  /dL0    /dL1    /dPhi   /dThe   /dCM
/// X  ->P[0]  dX /   P[ 7]   P[14]   P[21]   P[28]   P[35]
/// Y  ->P[1]  dY /   P[ 8]   P[15]   P[22]   P[29]   P[36]
/// Z  ->P[2]  dZ /   P[ 9]   P[16]   P[23]   P[30]   P[37]
/// Ax ->P[3]  dAx/   P[10]   P[17]   P[24]   P[31]   P[38]
/// Ay ->P[4]  dAy/   P[11]   P[18]   P[25]   P[32]   P[39]
/// Az ->P[5]  dAz/   P[12]   P[19]   P[26]   P[33]   P[40]
/// CM ->P[6]  dCM/   P[13]   P[20]   P[27]   P[34]   P[41]
///
/// where
///      in case local presentation
///
///      L0  - first  local coordinate  (surface dependent)
///      L1  - second local coordinate  (surface dependent)
///      Phi - Azimuthal angle
///      The - Polar     angle
///      CM  - charge/momentum
///
///      in case global presentation
///
///      X   - global x-coordinate        = surface dependent
///      Y   - global y-coordinate        = surface dependent
///      Z   - global z-coordinate        = sutface dependent
///      Ax  - direction cosine to x-axis = Sin(The)*Cos(Phi)
///      Ay  - direction cosine to y-axis = Sin(The)*Sin(Phi)
///      Az  - direction cosine to z-axis = Cos(The)
///      CM  - charge/momentum            = local CM
///

class RungeKuttaUtils
{
  /////////////////////////////////////////////////////////////////////////////////
  // Public methods:
  /////////////////////////////////////////////////////////////////////////////////

public:
  RungeKuttaUtils(){};
  virtual ~RungeKuttaUtils(){};

  /////////////////////////////////////////////////////////////////////////////////
  // Step estimators to surface
  /////////////////////////////////////////////////////////////////////////////////
  ///
  /// @param [in] stype The surface type
  /// @param [in] s The surface orientation
  /// @param [in] pdir The position and direction
  /// @param [in,out] q Quality flag
  /// @param [in] istep Indicator if it's the initial step (direction!)
  double
  stepEstimator(int           stype,
                double*       s,
                const double* pdir,
                bool&         q,
                bool          istep = false,
                double maxStep      = std::numeric_limits<double>::max()) const;
  double
  stepEstimatorToCone(double*, const double*, bool&, bool istep = false) const;
  double
  stepEstimatorToPlane(double*, const double*, bool&, bool istep = false) const;
  double
  stepEstimatorToCylinder(double*,
                          const double*,
                          bool&,
                          bool istep = false) const;
  double
  stepEstimatorToStraw(double*, const double*, bool&, bool istep = false) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from local to global system coordinates
  // for TrackParameters and NeutralParameters
  /////////////////////////////////////////////////////////////////////////////////

  bool
  transformLocalToGlobal(bool, const TrackParameters&, double*) const;

  bool
  transformLocalToGlobal(bool, const NeutralParameters&, double*) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from local to global system coordinates
  // for different surfaces
  /////////////////////////////////////////////////////////////////////////////////

  void
  transformDiscToGlobal(bool, const Surface*, const double*, double*) const;
  void
  transformPlaneToGlobal(bool, const Surface*, const double*, double*) const;
  void
  transformCylinderToGlobal(bool, const Surface*, const double*, double*) const;
  void
  transformLineToGlobal(bool, const Surface*, const double*, double*) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from global to local system coordinates
  /////////////////////////////////////////////////////////////////////////////////

  void
  transformGlobalToLocal(double*, double*) const;
  void
  transformGlobalToLocal(const Surface*, bool, double*, double*, double*) const;
  void
  transformGlobalToCone(const Surface*, bool, double*, double*, double*) const;
  void
  transformGlobalToDisc(const Surface*, bool, double*, double*, double*) const;
  void
  transformGlobalToPlane(const Surface*, bool, double*, double*, double*) const;
  void
  transformGlobalToCylinder(const Surface*, bool, double*, double*, double*)
      const;
  void
  transformGlobalToLine(const Surface*, bool, double*, double*, double*) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Covariance matrix production for TrackParameters
  /////////////////////////////////////////////////////////////////////////////////
  ActsSymMatrixD<5>*
  newCovarianceMatrix(double*, const ActsSymMatrixD<5>&) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from curvilinear to global system coordinates
  // covariance matrix only
  /////////////////////////////////////////////////////////////////////////////////
  void
  transformCurvilinearToGlobal(double*, double*) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Transformations from global to curvilinear system coordinates
  // covariance matrix only
  /////////////////////////////////////////////////////////////////////////////////
  void
  transformGlobalToCurvilinear(bool, double*, double*, double*) const;

  /////////////////////////////////////////////////////////////////////////////////
  // Jacobian of transformations from curvilinear to local system coordinates
  /////////////////////////////////////////////////////////////////////////////////
  void
  jacobianTransformCurvilinearToLocal(const TrackParameters&, double*);
  void
  jacobianTransformCurvilinearToLocal(double*, const Surface*, double*);
  void
  jacobianTransformCurvilinearToDisc(double*, double*) const;
  void
  jacobianTransformCurvilinearToPlane(double*, double*) const;
  void
  jacobianTransformCurvilinearToCylinder(double*, double*) const;
  void
  jacobianTransformCurvilinearToLine(double*, double*) const;

private:
  /////////////////////////////////////////////////////////////////////////////////
  // Private methods:
  /////////////////////////////////////////////////////////////////////////////////

  bool
  transformLocalToGlobal(bool, const Surface*, const double*, double*) const;
};
}  // end of namespace