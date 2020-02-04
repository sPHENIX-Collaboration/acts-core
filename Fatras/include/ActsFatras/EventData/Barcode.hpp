// This file is part of the Acts project.
//
// Copyright (C) 2018-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

#include "Acts/Utilities/MultiIndex.hpp"

namespace ActsFatras {

/// Particle identifier that encodes additional event information.
///
/// The barcode has to fulfill two separate requirements: be able to act as
/// unique identifier for particles within an event and to encode details
/// on the event structure for fast lookup. Since we only care about tracking
/// here, we need to support two scenarios:
///
/// *   Identify which primary/secondary vertex particles belong to. No
///     information on intermediate/unstable/invisible particles needs to be
///     retained.
/// *   If visible particles convert, decay, or interact with the detector, we
///     need to be able to identify the initial (primary) particle. Typical
///     examples are pion nuclear interactions or electron/gamma conversions
///     where the leading secondary particle has similar kinematic properties
///     to the primary particle.
///
/// The vertex information is encoded as two 12bit numbers that define the
/// primary and secondary vertex. The primary vertex must be non-zero.
/// Particles with secondary vertex 0 originate directly from the primary
/// vertex.
///
/// Within one vertex (primary+secondary) each particle is identified by two
/// 16bit numbers. The first number stores the parent particle identifier (as
/// defined below) or zero if the particle was created at the vertex.
/// The second numbers stores the particle identifier. Since zero indicates
/// no visible parent particle in the pervious field, the particle identifier
/// must be non-zero as well.
///
/// A 8bit process identifier provides additional information on the process
/// that created the particle.
///
/// A barcode with all elements set to zero (the default value) is an invalid
/// value that can be used e.g. to mark missing or unknown particles.
///
/// ## Example:
///
/// A particle generated in a primary interaction might have the barcode
///
///     2|0|0|14|0 -> vertex=2 (primary), particle=14 (no parent)
///
/// A simulation module might produce an interaction and outputs two new
/// particles. The module can generate the new barcodes directly as
///
///     2|0|14|1|0 -> vertex=2 (primary), parent=14, particle=1
///     2|0|14|2|0 -> vertex=2 (primary), parent=14, particle=2
///
/// The case where a secondary particle generates further tertiaries in
/// subsequent interactions is currently undefined.
class Barcode : public Acts::MultiIndex<uint64_t, 12, 12, 16, 16, 8> {
 public:
  /// Return the primary vertex identifier.
  constexpr Value vertexPrimary() const { return level(0); }
  /// Return the secondary vertex identifier.
  constexpr Value vertexSecondary() const { return level(1); }
  /// Return the parent particle identifier.
  constexpr Value parentParticle() const { return level(2); }
  /// Return the particle identifier.
  constexpr Value particle() const { return level(3); }
  /// Return the process identifier.
  constexpr Value process() const { return level(4); }

  /// Set the primary vertex identifier.
  constexpr Barcode& setVertexPrimary(Value id) { return set(0, id), *this; }
  /// Set the secondary vertex identifier.
  constexpr Barcode& setVertexSecondary(Value id) { return set(1, id), *this; }
  /// Set the parent particle identifier.
  constexpr Barcode& setParentParticle(Value id) { return set(2, id), *this; }
  /// Set the particle identifier.
  constexpr Barcode& setParticle(Value id) { return set(3, id), *this; }
  /// Set the process identifier.
  constexpr Barcode& setProcess(Value id) { return set(4, id), *this; }
};

}  // namespace ActsFatras
