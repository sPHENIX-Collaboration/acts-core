// This file is part of the Acts project.
//
// Copyright (C) 2016-2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// CylinderBounds.cpp, Acts project
///////////////////////////////////////////////////////////////////

#include "Acts/Surfaces/CylinderBounds.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "Acts/Utilities/VariantData.hpp"
#include "Acts/Utilities/detail/periodic.hpp"

Acts::CylinderBounds::CylinderBounds(double radius, double halfZ)
  : CylinderBounds(radius, 0, M_PI, halfZ)
{
}

Acts::CylinderBounds::CylinderBounds(double radius,
                                     double halfPhi,
                                     double halfZ)
  : CylinderBounds(radius, 0, halfPhi, halfZ)
{
}

Acts::CylinderBounds::CylinderBounds(double radius,
                                     double averagePhi,
                                     double halfPhi,
                                     double halfZ)
  : m_radius(std::abs(radius))
  , m_avgPhi(detail::radian_sym(averagePhi))
  , m_halfPhi(std::abs(halfPhi))
  , m_halfZ(std::abs(halfZ))
{
}

Acts::CylinderBounds::CylinderBounds(const variant_data& data_)
  : m_radius(0), m_avgPhi(0), m_halfPhi(0), m_halfZ(0)
{
  throw_assert(data_.which() == 4, "Variant data must be map");
  const variant_map& data = boost::get<variant_map>(data_);
  std::string        type = data.get<std::string>("type");
  throw_assert(type == "CylinderBounds", "Type must be CylinderBounds");

  const variant_map& payload = data.get<variant_map>("payload");

  m_radius  = payload.get<double>("radius");
  m_avgPhi  = payload.get<double>("avgPhi");
  m_halfPhi = payload.get<double>("halfPhi");
  m_halfZ   = payload.get<double>("halfZ");
}

Acts::CylinderBounds::~CylinderBounds()
{
}

Acts::CylinderBounds*
Acts::CylinderBounds::clone() const
{
  return new CylinderBounds(*this);
}

Acts::SurfaceBounds::BoundsType
Acts::CylinderBounds::type() const
{
  return SurfaceBounds::Cylinder;
}

std::vector<TDD_real_t>
Acts::CylinderBounds::valueStore() const
{
  std::vector<TDD_real_t> values(CylinderBounds::bv_length);
  values[CylinderBounds::bv_radius]        = m_radius;
  values[CylinderBounds::bv_averagePhi]    = m_avgPhi;
  values[CylinderBounds::bv_halfPhiSector] = m_halfPhi;
  values[CylinderBounds::bv_halfZ]         = m_halfZ;
  return values;
}

// Convert from (r*phi,z) to (phi,z) centered around phi0
Acts::Vector2D
Acts::CylinderBounds::shifted(const Acts::Vector2D& lpos) const
{
  return {
      Acts::detail::radian_sym((lpos[Acts::eLOC_RPHI] / m_radius) - m_avgPhi),
      lpos[Acts::eLOC_Z]};
}

// Jacobian from (r*phi,z) to (phi,z)
Acts::ActsSymMatrixD<2>
Acts::CylinderBounds::jacobian() const
{
  ActsSymMatrixD<2> j;
  j(0, eLOC_RPHI) = 1 / m_radius;
  j(0, eLOC_Z)    = 0;
  j(1, eLOC_RPHI) = 0;
  j(1, eLOC_Z)    = 1;
  return j;
}

bool
Acts::CylinderBounds::inside(const Acts::Vector2D&      lpos,
                             const Acts::BoundaryCheck& bcheck) const
{
  return bcheck.transformed(jacobian())
      .isInside(shifted(lpos), -m_halfPhi, m_halfPhi, -m_halfZ, m_halfZ);
}

bool
Acts::CylinderBounds::inside3D(const Acts::Vector3D&      pos,
                               const Acts::BoundaryCheck& bcheck) const
{

  if (s_onSurfaceTolerance <= std::abs(pos.perp() - m_radius)) return false;

  Vector2D lpos(detail::radian_sym(pos.phi() - m_avgPhi), pos.z());
  return bcheck.transformed(jacobian())
      .isInside(lpos, -m_halfPhi, m_halfPhi, -m_halfZ, m_halfZ);
}

double
Acts::CylinderBounds::distanceToBoundary(const Acts::Vector2D& lpos) const
{
  return BoundaryCheck(true).distance(
      shifted(lpos), -m_halfPhi, m_halfPhi, -m_halfZ, m_halfZ);
}

// ostream operator overload
std::ostream&
Acts::CylinderBounds::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Acts::CylinderBounds: (radius, averagePhi, halfPhiSector, "
        "halflengthInZ) = ";
  sl << "(" << m_radius << ", " << m_avgPhi << ", ";
  sl << m_halfPhi << ", " << m_halfZ << ")";
  sl << std::setprecision(-1);
  return sl;
}

Acts::variant_data
Acts::CylinderBounds::toVariantData() const
{
  using namespace std::string_literals;

  variant_map payload;
  payload["radius"]  = m_radius;
  payload["avgPhi"]  = m_avgPhi;
  payload["halfPhi"] = m_halfPhi;
  payload["halfZ"]   = m_halfZ;

  variant_map data({{"type", "CylinderBounds"s}, {"payload", payload}});

  return data;
}
