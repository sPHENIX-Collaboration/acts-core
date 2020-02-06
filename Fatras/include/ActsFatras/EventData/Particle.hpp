// This file is part of the Acts project.
//
// Copyright (C) 2018-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <limits>

#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/PdgParticle.hpp"
#include "ActsFatras/EventData/Barcode.hpp"

namespace ActsFatras {

/// Simulation particle information and kinematic state.
class Particle {
 public:
  using Scalar = double;
  using Vector3 = Acts::ActsVector<Scalar, 3>;
  using Vector4 = Acts::ActsVector<Scalar, 4>;

  /// Construct a default particle with invalid identity.
  Particle() = default;
  /// Construct a particle at rest with a fixed identity.
  ///
  /// @param id     Encoded identifier within an event
  /// @param pdg    PDG particle type number
  /// @param mass   Particle mass in native units
  /// @param charge Particle charge in native units
  Particle(Barcode id, Acts::PdgParticle pdg, Scalar mass, Scalar charge)
      : m_id(id), m_pdg(pdg), m_mass(mass), m_charge(charge) {}
  Particle(const Particle &) = default;
  Particle(Particle &&) = default;
  Particle &operator=(const Particle &) = default;
  Particle &operator=(Particle &&) = default;

  /// Set the space-time position.
  Particle &setPosition(const Vector4 &pos4) {
    m_position4 = pos4;
    return *this;
  }
  /// Set the space-time position from three-position and time.
  Particle &setPosition(const Vector3 &position, Scalar time) {
    m_position4.head<3>() = position;
    m_position4[3] = time;
    return *this;
  }
  /// Set the direction.
  Particle &setDirection(const Vector3 &direction) {
    m_direction = direction;
    m_direction.normalize();
    return *this;
  }
  /// Set the absolute momentum.
  Particle &setMomentum(Scalar momentum) {
    m_momentum = momentum;
    return *this;
  }

  /// Change the energy by the given amount.
  ///
  /// Energy loss corresponds to a negative change. If the updated energy
  /// would result in an unphysical value, the particle is put to rest, i.e.
  /// its absolute momentum is set to zero.
  Particle &correctEnergy(Scalar delta) {
    const auto newEnergy = std::hypot(m_mass, m_momentum) + delta;
    if (newEnergy <= m_mass) {
      m_momentum = Scalar(0);
    } else {
      m_momentum = std::sqrt(newEnergy * newEnergy - m_mass * m_mass);
    }
    return *this;
  }

  /// Encoded particle identifier within an event.
  Barcode id() const { return m_id; }
  /// PDG particle type number.
  Acts::PdgParticle pdg() const { return m_pdg; }
  /// Particle charge.
  Scalar charge() const { return m_charge; }

  /// Space-time four-position.
  const Vector4 &position4() const { return m_position4; }
  /// Three-position, i.e. spatial coordinates without the time.
  auto position() const { return m_position4.head<3>(); }
  /// Time coordinate.
  Scalar time() const { return m_position4[3]; }
  /// Three-direction, i.e. the normalized momentum three-vector.
  const Vector3 &direction() const { return m_direction; }
  /// Absolute momentum.
  Scalar momentum() const { return m_momentum; }
  /// Particle mass.
  Scalar mass() const { return m_mass; }
  /// Total energy, i.e. norm of the four-momentum.
  Scalar energy() const { return std::hypot(m_mass, m_momentum); }
  /// Charge over absolute momentum.
  Scalar chargeOverMomentum() const { return m_charge / m_momentum; }

  /// Relativistic velocity.
  Scalar beta() const { return m_momentum / energy(); }
  /// Relativistic gamma factor.
  Scalar gamma() const { return std::hypot(1, m_momentum / m_mass); }

  /// Check if the particle is still alive, i.e. has non-zero momentum.
  operator bool() const { return (Scalar(0) < m_momentum); }
  bool operator!() const { return (m_momentum <= Scalar(0)); }

 private:
  // identity, i.e. things that do not change over the particle lifetime.
  /// Particle identifier within the event.
  Barcode m_id;
  /// Particle type number
  Acts::PdgParticle m_pdg = Acts::PdgParticle::eInvalid;
  /// Particle mass and charge
  Scalar m_mass = Scalar(0);
  Scalar m_charge = Scalar(0);
  // kinematics, i.e. things that change over the particle lifetime.
  Vector3 m_direction = Vector3::UnitZ();
  Scalar m_momentum = Scalar(0);
  Vector4 m_position4 = Vector4::Zero();
};

}  // namespace ActsFatras
