// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <functional>
#include <limits>

#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Propagator/DefaultExtension.hpp"
#include "Acts/Propagator/DenseEnvironmentExtension.hpp"
#include "Acts/Propagator/EigenStepperError.hpp"
#include "Acts/Propagator/StepperExtensionList.hpp"
#include "Acts/Propagator/StepperState.hpp"
#include "Acts/Propagator/detail/Auctioneer.hpp"
#include "Acts/Propagator/detail/SteppingHelper.hpp"
#include "Acts/Utilities/Intersection.hpp"
#include "Acts/Utilities/Result.hpp"
#include "Acts/Utilities/Units.hpp"

namespace Acts {

using namespace Acts::UnitLiterals;

/// @brief Runge-Kutta-Nystroem stepper based on Eigen implementation
/// for the following ODE:
///
/// r = (x,y,z)    ... global position
/// T = (Ax,Ay,Az) ... momentum direction (normalized)
///
/// dr/ds = T
/// dT/ds = q/p * (T x B)
///
/// with s being the arc length of the track, q the charge of the particle,
/// p its momentum and B the magnetic field
///
template <typename bfield_t,
          typename extensionlist_t = StepperExtensionList<DefaultExtension>,
          typename auctioneer_t = detail::VoidAuctioneer>
class EigenStepper {
 public:
  /// Jacobian, Covariance and State defintions
  using Jacobian = BoundMatrix;
  using Covariance = BoundSymMatrix;
  using BoundState = std::tuple<BoundParameters, Jacobian, double>;
  using CurvilinearState = std::tuple<CurvilinearParameters, Jacobian, double>;
  using BField = bfield_t;

  /// @brief The state object. This object extends the general container @c
  /// StepperState by some parameters for RKN4
  struct State : public StepperState {
    /// Constructor from the initial track parameters
    ///
    /// @tparam parameters_t the Type of the track parameters
    ///
    /// @param [in] gctx is the context object for the geometery
    /// @param [in] mctx is the context object for the magnetic field
    /// @param [in] par The track parameters at start
    /// @param [in] ndir The navigation direciton w.r.t momentum
    /// @param [in] ssize is the maximum step size
    /// @param [in] stolerance is the stepping tolerance
    ///
    /// @note the covariance matrix is copied when needed
    template <typename parameters_t>
    explicit State(std::reference_wrapper<const GeometryContext> gctx,
                   std::reference_wrapper<const MagneticFieldContext> mctx,
                   const parameters_t& par, NavigationDirection ndir = forward,
                   double ssize = std::numeric_limits<double>::max(),
                   double stolerance = s_onSurfaceTolerance)
        : StepperState(gctx, mctx, par, ndir, ssize, stolerance),
          fieldCache(mctx) {

    /// This caches the current magnetic field cell and stays
    /// (and interpolates) within it as long as this is valid.
    /// See step() code for details.
    typename BField::Cache fieldCache;

    /// List of algorithmic extensions
    extensionlist_t extension;

    /// Auctioneer for choosing the extension
    auctioneer_t auctioneer;

    /// @brief Storage of magnetic field and the sub steps during a RKN4 step
    struct {
      /// Magnetic field evaulations
      Vector3D B_first, B_middle, B_last;
      /// k_i of the RKN4 algorithm
      Vector3D k1, k2, k3, k4;
    } stepData;
  };

  /// Constructor requires knowledge of the detector's magnetic field
  EigenStepper(BField bField = BField());

  /// Get the field for the stepping, it checks first if the access is still
  /// within the Cell, and updates the cell if necessary.
  ///
  /// @param [in,out] state is the propagation state associated with the track
  ///                 the magnetic field cell is used (and potentially updated)
  /// @param [in] pos is the field position
  Vector3D getField(State& state, const Vector3D& pos) const {
    // get the field from the cell
    return m_bField.getField(pos, state.fieldCache);
  }

  /// Global particle position accessor
  ///
  /// @param state [in] The stepping state (thread-local cache)
  Vector3D position(const State& state) const { return state.pos; }

  /// Momentum direction accessor
  ///
  /// @param state [in] The stepping state (thread-local cache)
  Vector3D direction(const State& state) const { return state.dir; }

  /// Actual momentum accessor
  ///
  /// @param state [in] The stepping state (thread-local cache)
  double momentum(const State& state) const { return state.p; }

  /// Charge access
  ///
  /// @param state [in] The stepping state (thread-local cache)
  double charge(const State& state) const { return state.q; }

  /// Time access
  ///
  /// @param state [in] The stepping state (thread-local cache)
  double time(const State& state) const { return state.t; }

  /// Update surface status
  ///
  /// It checks the status to the reference surface & updates
  /// the step size accordingly
  ///
  /// @param state [in,out] The stepping state (thread-local cache)
  /// @param surface [in] The surface provided
  /// @param bcheck [in] The boundary check for this status update
  Intersection::Status updateSurfaceStatus(State& state, const Surface& surface,
                                           const BoundaryCheck& bcheck) const {
    return detail::updateSingleSurfaceStatus<EigenStepper>(*this, state,
                                                           surface, bcheck);
  }

  /// Update step size
  ///
  /// This method intersect the provided surface and update the navigation
  /// step estimation accordingly (hence it changes the state). It also
  /// returns the status of the intersection to trigger onSurface in case
  /// the surface is reached.
  ///
  /// @param state [in,out] The stepping state (thread-local cache)
  /// @param oIntersection [in] The ObjectIntersection to layer, boundary, etc
  /// @param release [in] boolean to trigger step size release
  template <typename object_intersection_t>
  void updateStepSize(State& state, const object_intersection_t& oIntersection,
                      bool release = true) const {
    detail::updateSingleStepSize<EigenStepper>(state, oIntersection, release);
  }

  /// Set Step size - explicitely with a double
  ///
  /// @param state [in,out] The stepping state (thread-local cache)
  /// @param stepSize [in] The step size value
  /// @param stype [in] The step size type to be set
  void setStepSize(State& state, double stepSize,
                   ConstrainedStep::Type stype = ConstrainedStep::actor) const {
    state.previousStepSize = state.stepSize;
    state.stepSize.update(stepSize, stype, true);
  }

  /// Release the Step size
  ///
  /// @param state [in,out] The stepping state (thread-local cache)
  void releaseStepSize(State& state) const {
    state.stepSize.release(ConstrainedStep::actor);
  }

  /// Output the Step Size - single component
  ///
  /// @param state [in,out] The stepping state (thread-local cache)
  std::string outputStepSize(const State& state) const {
    return state.stepSize.toString();
  }

  /// Overstep limit
  ///
  /// @param state [in] The stepping state (thread-local cache)
  double overstepLimit(const State& /*state*/) const {
    // A dynamic overstep limit could sit here
    return -m_overstepLimit;
  }

  /// Create and return the bound state at the current position
  ///
  /// @brief This transports (if necessary) the covariance
  /// to the surface and creates a bound state. It does not check
  /// if the transported state is at the surface, this needs to
  /// be guaranteed by the propagator
  ///
  /// @param [in] state State that will be presented as @c BoundState
  /// @param [in] surface The surface to which we bind the state
  /// @param [in] reinitialize Boolean flag whether reinitialization is needed,
  /// i.e. if this is an intermediate state of a larger propagation
  ///
  /// @return A bound state:
  ///   - the parameters at the surface
  ///   - the stepwise jacobian towards it (from last bound)
  ///   - and the path length (from start - for ordering)
  BoundState boundState(State& state, const Surface& surface,
                        bool reinitialize = true) const;

  /// Create and return a curvilinear state at the current position
  ///
  /// @brief This transports (if necessary) the covariance
  /// to the current position and creates a curvilinear state.
  ///
  /// @param [in] state State that will be presented as @c CurvilinearState
  /// @param [in] reinitialize Boolean flag whether reinitialization is needed
  /// i.e. if this is an intermediate state of a larger propagation
  ///
  /// @return A curvilinear state:
  ///   - the curvilinear parameters at given position
  ///   - the stepweise jacobian towards it (from last bound)
  ///   - and the path length (from start - for ordering)
  CurvilinearState curvilinearState(State& state,
                                    bool reinitialize = true) const;

  /// Method to update a stepper state to the some parameters
  ///
  /// @param [in,out] state State object that will be updated
  /// @param [in] pars Parameters that will be written into @p state
  void update(State& state, const BoundParameters& pars) const;

  /// Method to update momentum, direction and p
  ///
  /// @param [in,out] state State object that will be updated
  /// @param [in] uposition the updated position
  /// @param [in] udirection the updated direction
  /// @param [in] up the updated momentum value
  void update(State& state, const Vector3D& uposition,
              const Vector3D& udirection, double up, double time) const;

  /// Method for on-demand transport of the covariance
  /// to a new curvilinear frame at current  position,
  /// or direction of the state
  ///
  /// @param [in,out] state State of the stepper
  /// @param [in] reinitialize is a flag to steer whether the state should be
  /// reinitialized at the new position
  ///
  /// @return the full transport jacobian
  void covarianceTransport(State& state, bool reinitialize = false) const;

  /// Method for on-demand transport of the covariance
  /// to a new curvilinear frame at current position,
  /// or direction of the state
  ///
  /// @tparam surface_t the Surface type
  ///
  /// @param [in,out] state State of the stepper
  /// @param [in] surface is the surface to which the covariance is forwarded to
  /// @param [in] reinitialize is a flag to steer whether the state should be
  /// reinitialized at the new position
  /// @note no check is done if the position is actually on the surface
  void covarianceTransport(State& state, const Surface& surface,
                           bool reinitialize = true) const;

  /// Perform a Runge-Kutta track parameter propagation step
  ///
  /// @param [in,out] state is the propagation state associated with the track
  /// parameters that are being propagated.
  ///
  ///                      the state contains the desired step size.
  ///                      It can be negative during backwards track
  ///                      propagation,
  ///                      and since we're using an adaptive algorithm, it can
  ///                      be modified by the stepper class during propagation.
  template <typename propagator_state_t>
  Result<double> step(propagator_state_t& state) const;

 private:
  /// Magnetic field inside of the detector
  BField m_bField;

  /// Overstep limit: could/should be dynamic
  double m_overstepLimit = 100_um;
};
}  // namespace Acts

#include "Acts/Propagator/EigenStepper.ipp"
