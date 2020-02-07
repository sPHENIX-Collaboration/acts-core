// This file is part of the Acts project.
//
// Copyright (C) 2018-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Material/MaterialProperties.hpp"
#include "ActsFatras/EventData/Particle.hpp"

namespace ActsFatras {

/// No-op particle selector that selects all particles.
struct EveryParticle {
  constexpr bool operator()(const Particle &) const { return true; }
};

/// A simulation process based on a physics interaction plus selectors.
///
/// @tparam physics_t         is the physics interaction type
/// @tparam input_selector_t  is the input particle selector
/// @tparam output_selector_t is the output particle selector
/// @tparam child_selector_t  is the child particle selector
///
/// The input selector defines whether the interaction is applied while the
/// output selector defines a break condition, i.e. whether to continue
/// simulating the particle propagation.
template <typename physics_t, typename input_selector_t = EveryParticle,
          typename output_selector_t = EveryParticle,
          typename child_selector_t = EveryParticle>
struct Process {
  /// The physics interactions implementation.
  physics_t physics;
  /// Input selection: which particles should the process be applied to.
  input_selector_t selectInput;
  /// Output selection: is the particle still valid after the interaction.
  output_selector_t selectOutput;
  /// Child selection: which generated child particles to keep.
  child_selector_t selectChild;

  /// Execute the physics process considering the configured selectors.
  ///
  /// @param[in]     generator is the random number generator
  /// @param[in]     slab      is the passed material
  /// @param[in,out] particle  is the particle being updated
  /// @param[out]    generated is the container of generated
  /// @return Break condition, i.e. whether this process stops the propagation
  ///
  /// @tparam generator_t must be a RandomNumberEngine
  template <typename generator_t>
  bool operator()(generator_t &generator, const Acts::MaterialProperties &slab,
                  Particle &particle, std::vector<Particle> &generated) const {
    // not selecting this process for a particle is not a break condition
    if (not selectInput(particle)) {
      return false;
    }
    // modify particle according to the physics process and select children
    auto children = physics(generator, slab, particle);
    std::copy_if(children.begin(), children.end(),
                 std::back_inserter(generated), selectChild);
    // break condition is defined by whether the output particle is still valid
    // or not e.g. because it has fallen below a momentum threshold.
    return not selectOutput(particle);
  }
};

}  // namespace ActsFatras
