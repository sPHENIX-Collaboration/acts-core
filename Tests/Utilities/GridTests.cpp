// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#define BOOST_TEST_MODULE grid tests
#include <boost/test/included/unit_test.hpp>
#include <chrono>
#include <random>

#include "Acts/Utilities/concept/AnyGrid.hpp"
#include "Acts/Utilities/detail/Axis.hpp"
#include "Acts/Utilities/detail/Grid.hpp"

namespace Acts {

using namespace detail;

namespace Test {

  BOOST_AUTO_TEST_CASE(grid_test_1d_equidistant)
  {
    typedef std::array<double, 1> Point;
    typedef std::array<size_t, 1> indices;
    EquidistantAxis a(0.0, 4.0, 4u);
    Grid<double, EquidistantAxis> g(std::make_tuple(std::move(a)));

    // test general properties
    BOOST_TEST(g.size() == 6u);
    BOOST_TEST(g.getNBins().at(0) == 4u);

    // global bin index
    BOOST_TEST(g.getGlobalBinIndex(Point({{-0.3}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-0.}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.7}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.2}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2.}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2.7}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3.}})) == 4u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3.9999}})) == 4u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4.}})) == 5u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4.98}})) == 5u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{4}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{5}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{3}}) == 3u);
    BOOST_TEST(g.getGlobalBinIndex({{4}}) == 4u);
    BOOST_TEST(g.getGlobalBinIndex({{5}}) == 5u);

    BOOST_TEST(g.getLocalBinIndices(g.getGlobalBinIndex(Point({{2.7}})))
               == indices({{3}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2.}})));
    BOOST_TEST(g.isInside(Point({{0.}})));
    BOOST_TEST(g.isInside(Point({{2.5}})));
    BOOST_TEST(not g.isInside(Point({{4.}})));
    BOOST_TEST(not g.isInside(Point({{6.}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1}}) == Point({{0.5}}));
    BOOST_TEST(g.getBinCenter({{2}}) == Point({{1.5}}));
    BOOST_TEST(g.getBinCenter({{3}}) == Point({{2.5}}));
    BOOST_TEST(g.getBinCenter({{4}}) == Point({{3.5}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1}}) == Point({{0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2}}) == Point({{1.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{3}}) == Point({{2.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{4}}) == Point({{3.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1}}) == Point({{1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2}}) == Point({{2.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{3}}) == Point({{3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{4}}) == Point({{4.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_2d_equidistant)
  {
    typedef std::array<double, 2> Point;
    typedef std::array<size_t, 2> indices;
    EquidistantAxis a(0.0, 4.0, 4u);
    EquidistantAxis b(0.0, 3.0, 3u);
    Grid<double, EquidistantAxis, EquidistantAxis> g(
        std::make_tuple(std::move(a), std::move(b)));

    // test general properties
    BOOST_TEST(g.size() == 30u);
    BOOST_TEST(g.getNBins().at(0) == 4u);
    BOOST_TEST(g.getNBins().at(1) == 3u);

    // global bin index
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, -1}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, 0}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, 1}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, 2}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, 3}})) == 4u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, -1}})) == 5u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0}})) == 6u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 1}})) == 7u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 2}})) == 8u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3}})) == 9u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, -1}})) == 10u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0}})) == 11u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 1}})) == 12u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 2}})) == 13u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, -1}})) == 15u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 0}})) == 16u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 1}})) == 17u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 2}})) == 18u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 3}})) == 19u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, -1}})) == 20u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 0}})) == 21u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 1}})) == 22u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 2}})) == 23u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 3}})) == 24u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4, -1}})) == 25u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4, 0}})) == 26u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4, 1}})) == 27u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4, 2}})) == 28u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4, 3}})) == 29u);

    // test some arbitrary points
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.2, 0.3}})) == 11u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2.2, 3.3}})) == 19u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.9, 1.8}})) == 7u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3.7, 3.1}})) == 24u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.4, 2.3}})) == 13u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-3, 3}})) == 4u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{8, 1}})) == 27u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, -3}})) == 10u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 11}})) == 24u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-2, -3}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-2, 7}})) == 04u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{12, -1}})) == 25u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{12, 11}})) == 29u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{0, 4}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(6) == indices({{1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(7) == indices({{1, 2}}));
    BOOST_TEST(g.getLocalBinIndices(8) == indices({{1, 3}}));
    BOOST_TEST(g.getLocalBinIndices(9) == indices({{1, 4}}));
    BOOST_TEST(g.getLocalBinIndices(10) == indices({{2, 0}}));
    BOOST_TEST(g.getLocalBinIndices(11) == indices({{2, 1}}));
    BOOST_TEST(g.getLocalBinIndices(12) == indices({{2, 2}}));
    BOOST_TEST(g.getLocalBinIndices(13) == indices({{2, 3}}));
    BOOST_TEST(g.getLocalBinIndices(14) == indices({{2, 4}}));
    BOOST_TEST(g.getLocalBinIndices(15) == indices({{3, 0}}));
    BOOST_TEST(g.getLocalBinIndices(16) == indices({{3, 1}}));
    BOOST_TEST(g.getLocalBinIndices(17) == indices({{3, 2}}));
    BOOST_TEST(g.getLocalBinIndices(18) == indices({{3, 3}}));
    BOOST_TEST(g.getLocalBinIndices(19) == indices({{3, 4}}));
    BOOST_TEST(g.getLocalBinIndices(20) == indices({{4, 0}}));
    BOOST_TEST(g.getLocalBinIndices(21) == indices({{4, 1}}));
    BOOST_TEST(g.getLocalBinIndices(22) == indices({{4, 2}}));
    BOOST_TEST(g.getLocalBinIndices(23) == indices({{4, 3}}));
    BOOST_TEST(g.getLocalBinIndices(24) == indices({{4, 4}}));
    BOOST_TEST(g.getLocalBinIndices(25) == indices({{5, 0}}));
    BOOST_TEST(g.getLocalBinIndices(26) == indices({{5, 1}}));
    BOOST_TEST(g.getLocalBinIndices(27) == indices({{5, 2}}));
    BOOST_TEST(g.getLocalBinIndices(28) == indices({{5, 3}}));
    BOOST_TEST(g.getLocalBinIndices(29) == indices({{5, 4}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0, 0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 3}}) == 3u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 4}}) == 4u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 0}}) == 5u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1}}) == 6u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 2}}) == 7u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 3}}) == 8u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 4}}) == 9u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 0}}) == 10u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 1}}) == 11u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 2}}) == 12u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3}}) == 13u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 4}}) == 14u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0}}) == 15u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 1}}) == 16u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 2}}) == 17u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 3}}) == 18u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 4}}) == 19u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 0}}) == 20u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 1}}) == 21u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 2}}) == 22u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 3}}) == 23u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 4}}) == 24u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 0}}) == 25u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 1}}) == 26u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 2}}) == 27u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 3}}) == 28u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 4}}) == 29u);

    BOOST_TEST(g.getLocalBinIndices(g.getGlobalBinIndex(Point({{1.2, 0.7}})))
               == indices({{2, 1}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2., -1}})));
    BOOST_TEST(not g.isInside(Point({{-2., 1.}})));
    BOOST_TEST(not g.isInside(Point({{-2., 5.}})));
    BOOST_TEST(not g.isInside(Point({{1., -1.}})));
    BOOST_TEST(not g.isInside(Point({{6., -1.}})));
    BOOST_TEST(g.isInside(Point({{0.5, 1.3}})));
    BOOST_TEST(not g.isInside(Point({{4., -1.}})));
    BOOST_TEST(not g.isInside(Point({{4., 0.3}})));
    BOOST_TEST(not g.isInside(Point({{4., 3.}})));
    BOOST_TEST(not g.isInside(Point({{-1., 3.}})));
    BOOST_TEST(not g.isInside(Point({{2., 3.}})));
    BOOST_TEST(not g.isInside(Point({{5., 3.}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1, 1}}) == Point({{0.5, 0.5}}));
    BOOST_TEST(g.getBinCenter({{2, 3}}) == Point({{1.5, 2.5}}));
    BOOST_TEST(g.getBinCenter({{3, 1}}) == Point({{2.5, 0.5}}));
    BOOST_TEST(g.getBinCenter({{4, 2}}) == Point({{3.5, 1.5}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1}}) == Point({{0., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 3}}) == Point({{1., 2.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{3, 1}}) == Point({{2., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{4, 2}}) == Point({{3., 1.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1}}) == Point({{1., 1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 3}}) == Point({{2., 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{3, 1}}) == Point({{3., 1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{4, 2}}) == Point({{4., 2.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7, 1.3}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_3d_equidistant)
  {
    typedef std::array<double, 3> Point;
    typedef std::array<size_t, 3> indices;
    EquidistantAxis a(0.0, 2.0, 2u);
    EquidistantAxis b(0.0, 3.0, 3u);
    EquidistantAxis c(0.0, 2.0, 2u);
    Grid<double, EquidistantAxis, EquidistantAxis, EquidistantAxis> g(
        std::make_tuple(std::move(a), std::move(b), std::move(c)));

    // test general properties
    BOOST_TEST(g.size() == 80u);
    BOOST_TEST(g.getNBins().at(0) == 2u);
    BOOST_TEST(g.getNBins().at(1) == 3u);
    BOOST_TEST(g.getNBins().at(2) == 2u);

    // test grid points
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 0}})) == 25u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 1}})) == 26u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 2}})) == 27u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 1, 0}})) == 29u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 1, 1}})) == 30u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 1, 2}})) == 31u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 2, 0}})) == 33u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 2, 1}})) == 34u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 2, 2}})) == 35u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 0}})) == 37u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 1}})) == 38u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 2}})) == 39u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 0}})) == 45u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 1}})) == 46u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 2}})) == 47u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 1, 0}})) == 49u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 1, 1}})) == 50u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 1, 2}})) == 51u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 2, 0}})) == 53u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 2, 1}})) == 54u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 2, 2}})) == 55u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 0}})) == 57u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 1}})) == 58u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 2}})) == 59u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 0, 0}})) == 65u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 0, 1}})) == 66u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 0, 2}})) == 67u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 1, 0}})) == 69u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 1, 1}})) == 70u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 1, 2}})) == 71u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 2, 0}})) == 73u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 2, 1}})) == 74u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 2, 2}})) == 75u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 3, 0}})) == 77u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 3, 1}})) == 78u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, 3, 2}})) == 79u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0, 0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{0, 0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{0, 0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{0, 0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{0, 1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{0, 1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(6) == indices({{0, 1, 2}}));
    BOOST_TEST(g.getLocalBinIndices(7) == indices({{0, 1, 3}}));
    BOOST_TEST(g.getLocalBinIndices(24) == indices({{1, 1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(25) == indices({{1, 1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(26) == indices({{1, 1, 2}}));
    BOOST_TEST(g.getLocalBinIndices(27) == indices({{1, 1, 3}}));
    BOOST_TEST(g.getLocalBinIndices(52) == indices({{2, 3, 0}}));
    BOOST_TEST(g.getLocalBinIndices(53) == indices({{2, 3, 1}}));
    BOOST_TEST(g.getLocalBinIndices(54) == indices({{2, 3, 2}}));
    BOOST_TEST(g.getLocalBinIndices(55) == indices({{2, 3, 3}}));
    BOOST_TEST(g.getLocalBinIndices(60) == indices({{3, 0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(61) == indices({{3, 0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(62) == indices({{3, 0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(63) == indices({{3, 0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(76) == indices({{3, 4, 0}}));
    BOOST_TEST(g.getLocalBinIndices(77) == indices({{3, 4, 1}}));
    BOOST_TEST(g.getLocalBinIndices(78) == indices({{3, 4, 2}}));
    BOOST_TEST(g.getLocalBinIndices(79) == indices({{3, 4, 3}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 3}}) == 3u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1, 0}}) == 4u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1, 1}}) == 5u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1, 2}}) == 6u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1, 3}}) == 7u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1, 0}}) == 24u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1, 1}}) == 25u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1, 2}}) == 26u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1, 3}}) == 27u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 0}}) == 52u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 1}}) == 53u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 2}}) == 54u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 3}}) == 55u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0, 0}}) == 60u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0, 1}}) == 61u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0, 2}}) == 62u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0, 3}}) == 63u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 4, 0}}) == 76u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 4, 1}}) == 77u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 4, 2}}) == 78u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 4, 3}}) == 79u);

    BOOST_TEST(
        g.getLocalBinIndices(g.getGlobalBinIndex(Point({{1.2, 0.7, 1.4}})))
        == indices({{2, 1, 2}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2., -1, -2}})));
    BOOST_TEST(not g.isInside(Point({{-2., 1., 0.}})));
    BOOST_TEST(not g.isInside(Point({{-2., 5., -1}})));
    BOOST_TEST(not g.isInside(Point({{1., -1., 1.}})));
    BOOST_TEST(not g.isInside(Point({{6., -1., 4.}})));
    BOOST_TEST(g.isInside(Point({{0.5, 1.3, 1.7}})));
    BOOST_TEST(not g.isInside(Point({{2., -1., -0.4}})));
    BOOST_TEST(not g.isInside(Point({{2., 0.3, 3.4}})));
    BOOST_TEST(not g.isInside(Point({{2., 3., 0.8}})));
    BOOST_TEST(not g.isInside(Point({{-1., 3., 5.}})));
    BOOST_TEST(not g.isInside(Point({{2., 3., -1.}})));
    BOOST_TEST(not g.isInside(Point({{5., 3., 0.5}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1, 1, 1}}) == Point({{0.5, 0.5, 0.5}}));
    BOOST_TEST(g.getBinCenter({{2, 3, 2}}) == Point({{1.5, 2.5, 1.5}}));
    BOOST_TEST(g.getBinCenter({{1, 1, 2}}) == Point({{0.5, 0.5, 1.5}}));
    BOOST_TEST(g.getBinCenter({{2, 2, 1}}) == Point({{1.5, 1.5, 0.5}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1, 1}}) == Point({{0., 0., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 3, 2}}) == Point({{1., 2., 1.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1, 2}}) == Point({{0., 0., 1.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 2, 1}}) == Point({{1., 1., 0.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1, 1}}) == Point({{1., 1., 1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 3, 2}}) == Point({{2., 3., 2.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1, 2}}) == Point({{1., 1., 2.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 2, 1}}) == Point({{2., 2., 1.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7, 2.3, 1.3}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_1d_variable)
  {
    typedef std::array<double, 1> Point;
    typedef std::array<size_t, 1> indices;
    VariableAxis a({0.0, 1.0, 4.0});
    Grid<double, VariableAxis> g(std::make_tuple(std::move(a)));

    // test general properties
    BOOST_TEST(g.size() == 4u);
    BOOST_TEST(g.getNBins().at(0) == 2u);

    // global bin index
    BOOST_TEST(g.getGlobalBinIndex(Point({{-0.3}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.7}})) == 1u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.2}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2.7}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4.}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{4.98}})) == 3u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{3}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{3}}) == 3u);

    BOOST_TEST(g.getLocalBinIndices(g.getGlobalBinIndex(Point({{0.8}})))
               == indices({{1}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2.}})));
    BOOST_TEST(g.isInside(Point({{0.}})));
    BOOST_TEST(g.isInside(Point({{2.5}})));
    BOOST_TEST(not g.isInside(Point({{4.}})));
    BOOST_TEST(not g.isInside(Point({{6.}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1}}) == Point({{0.5}}));
    BOOST_TEST(g.getBinCenter({{2}}) == Point({{2.5}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1}}) == Point({{0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2}}) == Point({{1.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1}}) == Point({{1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2}}) == Point({{4.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_2d_variable)
  {
    typedef std::array<double, 2> Point;
    typedef std::array<size_t, 2> indices;
    VariableAxis a({0.0, 0.5, 3.0});
    VariableAxis b({0.0, 1.0, 4.0});
    Grid<double, VariableAxis, VariableAxis> g(
        std::make_tuple(std::move(a), std::move(b)));

    // test general properties
    BOOST_TEST(g.size() == 16u);
    BOOST_TEST(g.getNBins().at(0) == 2u);
    BOOST_TEST(g.getNBins().at(1) == 2u);

    // test grid points
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0}})) == 5u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 1}})) == 6u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 4}})) == 7u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 0}})) == 9u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 1}})) == 10u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 4}})) == 11u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 0}})) == 13u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 1}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3, 4}})) == 15u);

    // test some arbitrary points
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.3, 1.2}})) == 6u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3.3, 2.2}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.8, 0.9}})) == 9u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{3.1, 0.7}})) == 13u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2.3, 1.4}})) == 10u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{2, -3}})) == 8u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 8}})) == 11u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-3, 1}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{11, 3}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-3, -2}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{7, -2}})) == 12u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-1, 12}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{11, 12}})) == 15u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(6) == indices({{1, 2}}));
    BOOST_TEST(g.getLocalBinIndices(7) == indices({{1, 3}}));
    BOOST_TEST(g.getLocalBinIndices(8) == indices({{2, 0}}));
    BOOST_TEST(g.getLocalBinIndices(9) == indices({{2, 1}}));
    BOOST_TEST(g.getLocalBinIndices(10) == indices({{2, 2}}));
    BOOST_TEST(g.getLocalBinIndices(11) == indices({{2, 3}}));
    BOOST_TEST(g.getLocalBinIndices(12) == indices({{3, 0}}));
    BOOST_TEST(g.getLocalBinIndices(13) == indices({{3, 1}}));
    BOOST_TEST(g.getLocalBinIndices(14) == indices({{3, 2}}));
    BOOST_TEST(g.getLocalBinIndices(15) == indices({{3, 3}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0, 0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 3}}) == 3u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 0}}) == 4u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1}}) == 5u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 2}}) == 6u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 3}}) == 7u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 0}}) == 8u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 1}}) == 9u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 2}}) == 10u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3}}) == 11u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0}}) == 12u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 1}}) == 13u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 2}}) == 14u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 3}}) == 15u);

    BOOST_TEST(g.getLocalBinIndices(g.getGlobalBinIndex(Point({{3.2, 1.8}})))
               == indices({{3, 2}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2., -1}})));
    BOOST_TEST(not g.isInside(Point({{-2., 1.}})));
    BOOST_TEST(not g.isInside(Point({{-2., 5.}})));
    BOOST_TEST(not g.isInside(Point({{1., -1.}})));
    BOOST_TEST(not g.isInside(Point({{6., -1.}})));
    BOOST_TEST(g.isInside(Point({{0.5, 1.3}})));
    BOOST_TEST(not g.isInside(Point({{3., -1.}})));
    BOOST_TEST(not g.isInside(Point({{3., 0.3}})));
    BOOST_TEST(not g.isInside(Point({{3., 4.}})));
    BOOST_TEST(not g.isInside(Point({{-1., 4.}})));
    BOOST_TEST(not g.isInside(Point({{2., 4.}})));
    BOOST_TEST(not g.isInside(Point({{5., 4.}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1, 1}}) == Point({{0.25, 0.5}}));
    BOOST_TEST(g.getBinCenter({{2, 1}}) == Point({{1.75, 0.5}}));
    BOOST_TEST(g.getBinCenter({{1, 2}}) == Point({{0.25, 2.5}}));
    BOOST_TEST(g.getBinCenter({{2, 2}}) == Point({{1.75, 2.5}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1}}) == Point({{0., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 1}}) == Point({{0.5, 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 2}}) == Point({{0., 1.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 2}}) == Point({{0.5, 1.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1}}) == Point({{0.5, 1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 1}}) == Point({{3., 1.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 2}}) == Point({{0.5, 4.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 2}}) == Point({{3., 4.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7, 1.3}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_3d_variable)
  {
    typedef std::array<double, 3> Point;
    typedef std::array<size_t, 3> indices;
    VariableAxis a({0.0, 1.0});
    VariableAxis b({0.0, 0.5, 3.0});
    VariableAxis c({0.0, 0.5, 3.0, 3.3});
    Grid<double, VariableAxis, VariableAxis, VariableAxis> g(
        std::make_tuple(std::move(a), std::move(b), std::move(c)));

    // test general properties
    BOOST_TEST(g.size() == 60u);
    BOOST_TEST(g.getNBins().at(0) == 1u);
    BOOST_TEST(g.getNBins().at(1) == 2u);
    BOOST_TEST(g.getNBins().at(2) == 3u);

    // test grid points
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 0}})) == 26u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 0}})) == 46u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0.5, 0}})) == 31u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0.5, 0}})) == 51u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 0}})) == 36u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 0}})) == 56u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 0.5}})) == 27u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 0.5}})) == 47u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0.5, 0.5}})) == 32u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0.5, 0.5}})) == 52u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 0.5}})) == 37u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 0.5}})) == 57u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 3}})) == 28u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 3}})) == 48u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0.5, 3}})) == 33u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0.5, 3}})) == 53u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 3}})) == 38u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 3}})) == 58u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0, 3.3}})) == 29u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0, 3.3}})) == 49u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0.5, 3.3}})) == 34u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0.5, 3.3}})) == 54u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3, 3.3}})) == 39u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3, 3.3}})) == 59u);

    // global bin index -> local bin indices
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0, 0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{0, 0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{0, 0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{0, 0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{0, 0, 4}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{0, 1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(21) == indices({{1, 0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(22) == indices({{1, 0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(23) == indices({{1, 0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(24) == indices({{1, 0, 4}}));
    BOOST_TEST(g.getLocalBinIndices(25) == indices({{1, 1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(26) == indices({{1, 1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(57) == indices({{2, 3, 2}}));
    BOOST_TEST(g.getLocalBinIndices(58) == indices({{2, 3, 3}}));
    BOOST_TEST(g.getLocalBinIndices(59) == indices({{2, 3, 4}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 0, 0}}) == 20u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 0, 0}}) == 40u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1, 0}}) == 5u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1, 0}}) == 25u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 1, 0}}) == 45u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 3, 1}}) == 16u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 3, 1}}) == 36u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 1}}) == 56u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 0, 2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 0, 2}}) == 22u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 0, 2}}) == 42u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 3, 4}}) == 19u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 3, 4}}) == 39u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3, 4}}) == 59u);

    BOOST_TEST(
        g.getLocalBinIndices(g.getGlobalBinIndex(Point({{1.8, 0.7, 3.2}})))
        == indices({{2, 2, 3}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2., -1, -2}})));
    BOOST_TEST(not g.isInside(Point({{-2., 1., 0.}})));
    BOOST_TEST(not g.isInside(Point({{-2., 5., -1}})));
    BOOST_TEST(not g.isInside(Point({{1., -1., 1.}})));
    BOOST_TEST(not g.isInside(Point({{6., -1., 4.}})));
    BOOST_TEST(g.isInside(Point({{0.5, 1.3, 1.7}})));
    BOOST_TEST(not g.isInside(Point({{1., -1., -0.4}})));
    BOOST_TEST(not g.isInside(Point({{1., 0.3, 3.4}})));
    BOOST_TEST(not g.isInside(Point({{1., 3., 0.8}})));
    BOOST_TEST(not g.isInside(Point({{-1., 3., 5.}})));
    BOOST_TEST(not g.isInside(Point({{2., 3., -1.}})));
    BOOST_TEST(not g.isInside(Point({{5., 3., 0.5}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1, 1, 1}}) == Point({{0.5, 0.25, 0.25}}));
    BOOST_TEST(g.getBinCenter({{1, 1, 2}}) == Point({{0.5, 0.25, 1.75}}));
    BOOST_TEST(g.getBinCenter({{1, 1, 3}}) == Point({{0.5, 0.25, 3.15}}));
    BOOST_TEST(g.getBinCenter({{1, 2, 1}}) == Point({{0.5, 1.75, 0.25}}));
    BOOST_TEST(g.getBinCenter({{1, 2, 2}}) == Point({{0.5, 1.75, 1.75}}));
    BOOST_TEST(g.getBinCenter({{1, 2, 3}}) == Point({{0.5, 1.75, 3.15}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1, 1}}) == Point({{0., 0., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1, 2}}) == Point({{0., 0., 0.5}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1, 3}}) == Point({{0., 0., 3.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 2, 1}}) == Point({{0., 0.5, 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 2, 2}}) == Point({{0., 0.5, 0.5}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 2, 3}}) == Point({{0., 0.5, 3.}}));

    // test some upper right-bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1, 1}}) == Point({{1., 0.5, 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1, 2}}) == Point({{1., 0.5, 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1, 3}}) == Point({{1., 0.5, 3.3}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 2, 1}}) == Point({{1., 3., 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 2, 2}}) == Point({{1., 3., 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 2, 3}}) == Point({{1., 3., 3.3}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{0.7, 1.3, 3.7}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_2d_mixed)
  {
    typedef std::array<double, 2> Point;
    typedef std::array<size_t, 2> indices;
    EquidistantAxis a(0.0, 1.0, 4u);
    VariableAxis    b({0.0, 0.5, 3.0});
    Grid<double, EquidistantAxis, VariableAxis> g(
        std::make_tuple(std::move(a), std::move(b)));

    // test general properties
    BOOST_TEST(g.size() == 24u);
    BOOST_TEST(g.getNBins().at(0) == 4u);
    BOOST_TEST(g.getNBins().at(1) == 2u);

    // test grid points
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0}})) == 5u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.25, 0}})) == 9u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 0}})) == 13u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.75, 0}})) == 17u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0}})) == 21u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 0.5}})) == 6u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.25, 0.5}})) == 10u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 0.5}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.75, 0.5}})) == 18u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 0.5}})) == 22u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0, 3}})) == 7u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.25, 3}})) == 11u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.5, 3}})) == 15u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.75, 3}})) == 19u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{1, 3}})) == 23u);

    // test some arbitrary points
    BOOST_TEST(g.getGlobalBinIndex(Point({{1.2, 0.3}})) == 21u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.2, 1.3}})) == 6u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.9, 1.8}})) == 18u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.7, 2.1}})) == 14u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.4, 0.3}})) == 9u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-3, 2}})) == 2u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{8, 1}})) == 22u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.1, -3}})) == 4u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{0.8, 11}})) == 19u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-2, -3}})) == 0u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{-2, 7}})) == 3u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{12, -1}})) == 20u);
    BOOST_TEST(g.getGlobalBinIndex(Point({{12, 11}})) == 23u);

    // global bin index -> local bin indices
    typedef std::array<size_t, 2> indices;
    BOOST_TEST(g.getLocalBinIndices(0) == indices({{0, 0}}));
    BOOST_TEST(g.getLocalBinIndices(1) == indices({{0, 1}}));
    BOOST_TEST(g.getLocalBinIndices(2) == indices({{0, 2}}));
    BOOST_TEST(g.getLocalBinIndices(3) == indices({{0, 3}}));
    BOOST_TEST(g.getLocalBinIndices(4) == indices({{1, 0}}));
    BOOST_TEST(g.getLocalBinIndices(5) == indices({{1, 1}}));
    BOOST_TEST(g.getLocalBinIndices(6) == indices({{1, 2}}));
    BOOST_TEST(g.getLocalBinIndices(7) == indices({{1, 3}}));
    BOOST_TEST(g.getLocalBinIndices(8) == indices({{2, 0}}));
    BOOST_TEST(g.getLocalBinIndices(9) == indices({{2, 1}}));
    BOOST_TEST(g.getLocalBinIndices(10) == indices({{2, 2}}));
    BOOST_TEST(g.getLocalBinIndices(11) == indices({{2, 3}}));
    BOOST_TEST(g.getLocalBinIndices(12) == indices({{3, 0}}));
    BOOST_TEST(g.getLocalBinIndices(13) == indices({{3, 1}}));
    BOOST_TEST(g.getLocalBinIndices(14) == indices({{3, 2}}));
    BOOST_TEST(g.getLocalBinIndices(15) == indices({{3, 3}}));
    BOOST_TEST(g.getLocalBinIndices(16) == indices({{4, 0}}));
    BOOST_TEST(g.getLocalBinIndices(17) == indices({{4, 1}}));
    BOOST_TEST(g.getLocalBinIndices(18) == indices({{4, 2}}));
    BOOST_TEST(g.getLocalBinIndices(19) == indices({{4, 3}}));
    BOOST_TEST(g.getLocalBinIndices(20) == indices({{5, 0}}));
    BOOST_TEST(g.getLocalBinIndices(21) == indices({{5, 1}}));
    BOOST_TEST(g.getLocalBinIndices(22) == indices({{5, 2}}));
    BOOST_TEST(g.getLocalBinIndices(23) == indices({{5, 3}}));

    // local bin indices -> global bin index
    BOOST_TEST(g.getGlobalBinIndex({{0, 0}}) == 0u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 1}}) == 1u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 2}}) == 2u);
    BOOST_TEST(g.getGlobalBinIndex({{0, 3}}) == 3u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 0}}) == 4u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 1}}) == 5u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 2}}) == 6u);
    BOOST_TEST(g.getGlobalBinIndex({{1, 3}}) == 7u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 0}}) == 8u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 1}}) == 9u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 2}}) == 10u);
    BOOST_TEST(g.getGlobalBinIndex({{2, 3}}) == 11u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 0}}) == 12u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 1}}) == 13u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 2}}) == 14u);
    BOOST_TEST(g.getGlobalBinIndex({{3, 3}}) == 15u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 0}}) == 16u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 1}}) == 17u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 2}}) == 18u);
    BOOST_TEST(g.getGlobalBinIndex({{4, 3}}) == 19u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 0}}) == 20u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 1}}) == 21u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 2}}) == 22u);
    BOOST_TEST(g.getGlobalBinIndex({{5, 3}}) == 23u);

    BOOST_TEST(g.getLocalBinIndices(g.getGlobalBinIndex(Point({{1.1, 1.7}})))
               == indices({{5, 2}}));

    // inside checks
    BOOST_TEST(not g.isInside(Point({{-2., -1}})));
    BOOST_TEST(not g.isInside(Point({{-2., 1.}})));
    BOOST_TEST(not g.isInside(Point({{-2., 5.}})));
    BOOST_TEST(not g.isInside(Point({{0.1, -1.}})));
    BOOST_TEST(not g.isInside(Point({{6., -1.}})));
    BOOST_TEST(g.isInside(Point({{0.5, 1.3}})));
    BOOST_TEST(not g.isInside(Point({{1., -1.}})));
    BOOST_TEST(not g.isInside(Point({{1., 0.3}})));
    BOOST_TEST(not g.isInside(Point({{1., 3.}})));
    BOOST_TEST(not g.isInside(Point({{-1., 3.}})));
    BOOST_TEST(not g.isInside(Point({{0.2, 3.}})));
    BOOST_TEST(not g.isInside(Point({{5., 3.}})));

    // test some bin centers
    BOOST_TEST(g.getBinCenter({{1, 1}}) == Point({{0.125, 0.25}}));
    BOOST_TEST(g.getBinCenter({{1, 2}}) == Point({{0.125, 1.75}}));
    BOOST_TEST(g.getBinCenter({{2, 1}}) == Point({{0.375, 0.25}}));
    BOOST_TEST(g.getBinCenter({{2, 2}}) == Point({{0.375, 1.75}}));
    BOOST_TEST(g.getBinCenter({{3, 1}}) == Point({{0.625, 0.25}}));
    BOOST_TEST(g.getBinCenter({{3, 2}}) == Point({{0.625, 1.75}}));
    BOOST_TEST(g.getBinCenter({{4, 1}}) == Point({{0.875, 0.25}}));
    BOOST_TEST(g.getBinCenter({{4, 2}}) == Point({{0.875, 1.75}}));

    // test some lower-left bin edges
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 1}}) == Point({{0., 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{1, 2}}) == Point({{0., 0.5}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 1}}) == Point({{0.25, 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{2, 2}}) == Point({{0.25, 0.5}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{3, 1}}) == Point({{0.5, 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{3, 2}}) == Point({{0.5, 0.5}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{4, 1}}) == Point({{0.75, 0.}}));
    BOOST_TEST(g.getLowerLeftBinEdge({{4, 2}}) == Point({{0.75, 0.5}}));

    // test some upper-right bin edges
    BOOST_TEST(g.getUpperRightBinEdge({{1, 1}}) == Point({{0.25, 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{1, 2}}) == Point({{0.25, 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 1}}) == Point({{0.5, 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{2, 2}}) == Point({{0.5, 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{3, 1}}) == Point({{0.75, 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{3, 2}}) == Point({{0.75, 3.}}));
    BOOST_TEST(g.getUpperRightBinEdge({{4, 1}}) == Point({{1., 0.5}}));
    BOOST_TEST(g.getUpperRightBinEdge({{4, 2}}) == Point({{1., 3.}}));

    // initialize grid
    for (size_t bin = 0; bin < g.size(); ++bin) g.at(bin) = bin;

    // consistency of access
    const auto& point     = Point({{1.3, 3.7}});
    size_t      globalBin = g.getGlobalBinIndex(point);
    indices     localBins = g.getLocalBinIndices(globalBin);

    BOOST_TEST(g.at(point) == g.at(globalBin));
    BOOST_TEST(g.at(point) == g.at(localBins));
  }

  BOOST_AUTO_TEST_CASE(grid_test_2d_mixed_at)
  {
    EquidistantAxis a(0.0, 6.0, 4u);
    VariableAxis    b({0.0, 1.5, 3.0});
    Grid<double, EquidistantAxis, VariableAxis> g(
        std::make_tuple(std::move(a), std::move(b)));

    // initialize the grid
    typedef std::array<double, 2> Point;
    g.at(Point({{0, 0}}))     = 0.;
    g.at(Point({{1.5, 0}}))   = 1.;
    g.at(Point({{3, 0}}))     = 2.;
    g.at(Point({{4.5, 0}}))   = 3.;
    g.at(Point({{6, 0}}))     = 4.;
    g.at(Point({{0, 1.5}}))   = 5.;
    g.at(Point({{1.5, 1.5}})) = 6.;
    g.at(Point({{3, 1.5}}))   = 7.;
    g.at(Point({{4.5, 1.5}})) = 8.;
    g.at(Point({{6, 1.5}}))   = 9.;
    g.at(Point({{0, 3}}))     = 10.;
    g.at(Point({{1.5, 3}}))   = 11.;
    g.at(Point({{3, 3}}))     = 12.;
    g.at(Point({{4.5, 3}}))   = 13.;
    g.at(Point({{6, 3}}))     = 14.;

    // test general properties
    BOOST_TEST(g.size() == 24u);

    // test some arbitrary points
    BOOST_TEST(g.at(Point({{1.2, 0.3}})) == 0.);
    BOOST_TEST(g.at(Point({{2.2, 1.3}})) == 1.);
    BOOST_TEST(g.at(Point({{4.9, 1.8}})) == 8.);
    BOOST_TEST(g.at(Point({{3.7, 2.1}})) == 7.);
    BOOST_TEST(g.at(Point({{0.4, 2.3}})) == 5.);
  }

  BOOST_AUTO_TEST_CASE(grid_interpolation)
  {
    typedef std::array<double, 3> Point;
    EquidistantAxis a(1.0, 3.0, 2u);
    EquidistantAxis b(1.0, 5.0, 2u);
    EquidistantAxis c(1.0, 7.0, 2u);
    Grid<double, EquidistantAxis, EquidistantAxis, EquidistantAxis> g(
        std::make_tuple(std::move(a), std::move(b), std::move(c)));

    g.at(Point({{1., 1., 1.}})) = 10.;
    g.at(Point({{2., 1., 1.}})) = 20.;
    g.at(Point({{1., 3., 1.}})) = 30.;
    g.at(Point({{2., 3., 1.}})) = 40.;
    g.at(Point({{1., 1., 4.}})) = 50.;
    g.at(Point({{2., 1., 4.}})) = 60.;
    g.at(Point({{1., 3., 4.}})) = 70.;
    g.at(Point({{2., 3., 4.}})) = 80.;

    BOOST_TEST(g.interpolate(Point({{1., 1., 1.}})) == 10.);
    BOOST_TEST(g.interpolate(Point({{2., 1., 1.}})) == 20.);
    BOOST_TEST(g.interpolate(Point({{1., 3., 1.}})) == 30.);
    BOOST_TEST(g.interpolate(Point({{2., 3., 1.}})) == 40.);
    BOOST_TEST(g.interpolate(Point({{1., 1., 4.}})) == 50.);
    BOOST_TEST(g.interpolate(Point({{2., 1., 4.}})) == 60.);
    BOOST_TEST(g.interpolate(Point({{1., 3., 4.}})) == 70.);
    BOOST_TEST(g.interpolate(Point({{2., 3., 4.}})) == 80.);
    BOOST_TEST(g.interpolate(Point({{1.5, 1., 1.}})) == 15.);
    BOOST_TEST(g.interpolate(Point({{1.5, 3., 1.}})) == 35.);
    BOOST_TEST(g.interpolate(Point({{1., 2., 1.}})) == 20.);
    BOOST_TEST(g.interpolate(Point({{2., 2., 1.}})) == 30.);
    BOOST_TEST(g.interpolate(Point({{1.5, 1., 4.}})) == 55.);
    BOOST_TEST(g.interpolate(Point({{1.5, 3., 4.}})) == 75.);
    BOOST_TEST(g.interpolate(Point({{1., 2., 4.}})) == 60.);
    BOOST_TEST(g.interpolate(Point({{2., 2., 4.}})) == 70.);
    BOOST_TEST(g.interpolate(Point({{1., 1., 2.5}})) == 30.);
    BOOST_TEST(g.interpolate(Point({{1., 3., 2.5}})) == 50.);
    BOOST_TEST(g.interpolate(Point({{2., 1., 2.5}})) == 40.);
    BOOST_TEST(g.interpolate(Point({{2., 3., 2.5}})) == 60.);
    BOOST_TEST(g.interpolate(Point({{1.5, 2., 2.5}})) == 360. / 8);
    BOOST_TEST(g.interpolate(Point({{1.3, 2.1, 1.6}})) == 32.);
    BOOST_TEST(g.interpolate(Point({{2., 3., 4.}})) == 80.);
  }

  BOOST_AUTO_TEST_CASE(neighborhood)
  {
    typedef std::set<size_t> bins_t;
    typedef std::array<double, 1> point_t;
    typedef EquidistantAxis EAxis;
    typedef Grid<double, EAxis> Grid1_t;
    typedef Grid<double, EAxis, EAxis> Grid2_t;
    typedef Grid<double, EAxis, EAxis, EAxis> Grid3_t;

    EAxis   a(0.0, 1.0, 10u);
    EAxis   b(0.0, 1.0, 10u);
    EAxis   c(0.0, 1.0, 10u);
    Grid1_t g1(std::make_tuple(std::move(a)));
    Grid2_t g2(std::make_tuple(std::move(a), std::move(b)));
    Grid3_t g3(std::make_tuple(std::move(a), std::move(b), std::move(c)));

    // clang-format off
    // 1D case
    BOOST_TEST((g1.neighborHoodIndices({{0}}, 1) == bins_t({0, 1})));
    BOOST_TEST((g1.neighborHoodIndices({{0}}, 2) == bins_t({0, 1, 2})));
    BOOST_TEST((g1.neighborHoodIndices({{1}}, 1) == bins_t({0, 1, 2})));
    BOOST_TEST((g1.neighborHoodIndices({{1}}, 3) == bins_t({0, 1, 2, 3, 4})));
    BOOST_TEST((g1.neighborHoodIndices({{4}}, 2) == bins_t({2, 3, 4, 5, 6})));
    BOOST_TEST((g1.neighborHoodIndices({{9}}, 2) == bins_t({7, 8, 9, 10, 11})));
    BOOST_TEST((g1.neighborHoodIndices({{10}}, 2) == bins_t({8, 9, 10, 11})));
    BOOST_TEST((g1.neighborHoodIndices({{11}}, 2) == bins_t({9, 10, 11})));

    
    BOOST_TEST((g1.neighborHoodIndices(point_t({{-0.05}}), 1) == bins_t({0, 1})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{-0.05}}), 2) == bins_t({0, 1, 2})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{0.05}}), 1) == bins_t({0, 1, 2})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{0.05}}), 3) == bins_t({0, 1, 2, 3, 4})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{0.35}}), 2) == bins_t({2, 3, 4, 5, 6})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{0.85}}), 2) == bins_t({7, 8, 9, 10, 11})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{0.95}}), 2) == bins_t({8, 9, 10, 11})));
    BOOST_TEST((g1.neighborHoodIndices(point_t({{10.5}}), 2) == bins_t({9, 10, 11})));

    // 2D case
    BOOST_TEST((g2.neighborHoodIndices({{0, 0}}, 1) == bins_t({0, 1, 12, 13})));
    BOOST_TEST((g2.neighborHoodIndices({{0, 1}}, 1) == bins_t({0, 1, 2, 12, 13, 14})));
    BOOST_TEST((g2.neighborHoodIndices({{1, 0}}, 1) == bins_t({0, 1, 12, 13, 24, 25})));
    BOOST_TEST((g2.neighborHoodIndices({{1, 1}}, 1) == bins_t({0, 1, 2, 12, 13, 14, 24, 25, 26})));
    BOOST_TEST((g2.neighborHoodIndices({{5, 5}}, 1) == bins_t({52, 53, 54, 64, 65, 66, 76, 77, 78})));
    BOOST_TEST((g2.neighborHoodIndices({{9, 10}}, 2) == bins_t({92, 93, 94, 95, 104, 105, 106, 107, 116, 117, 118, 119, 128, 129, 130, 131, 140, 141, 142, 143})));

    // 3D case
    BOOST_TEST((g3.neighborHoodIndices({{0, 0, 0}}, 1) == bins_t({0, 1, 12, 13, 144, 145, 156, 157})));
    BOOST_TEST((g3.neighborHoodIndices({{0, 0, 1}}, 1) == bins_t({0, 1, 2, 12, 13, 14, 144, 145, 146, 156, 157, 158})));
    BOOST_TEST((g3.neighborHoodIndices({{0, 1, 0}}, 1) == bins_t({0, 1, 12, 13, 24, 25, 144, 145, 156, 157, 168, 169})));
    BOOST_TEST((g3.neighborHoodIndices({{1, 0, 0}}, 1) == bins_t({0, 1, 12, 13, 144, 145, 156, 157, 288, 289, 300, 301})));
    BOOST_TEST((g3.neighborHoodIndices({{0, 1, 1}}, 1) == bins_t({0, 1, 2, 12, 13, 14, 24, 25, 26, 144, 145, 146, 156, 157, 158, 168, 169, 170})));
    BOOST_TEST((g3.neighborHoodIndices({{1, 1, 1}}, 1) == bins_t({0, 1, 2, 12, 13, 14, 24, 25, 26, 144, 145, 146, 156, 157, 158, 168, 169, 170, 288, 289, 290, 300, 301, 302, 312, 313, 314})));
    BOOST_TEST((g3.neighborHoodIndices({{11, 10, 9}}, 1) == bins_t({1556, 1557, 1558, 1568, 1569, 1570, 1580, 1581, 1582, 1700, 1701, 1702, 1712, 1713, 1714, 1724, 1725, 1726})));
    // clang-format on

    typedef Axis<AxisType::Equidistant, AxisBoundaryType::Closed> EAxisClosed;
    typedef Grid<double, EAxisClosed>                             Grid1Closed_t;
    EAxisClosed d(0.0, 1.0, 10u);

    Grid1Closed_t g1Cl(std::make_tuple(std::move(d)));
    BOOST_TEST((g1Cl.neighborHoodIndices({{0}}, 1)
                == bins_t({})));  // underflow, makes no sense
    BOOST_TEST((g1Cl.neighborHoodIndices({{11}}, 1)
                == bins_t({})));  // overflow, makes no sense
    BOOST_TEST((g1Cl.neighborHoodIndices({{1}}, 1)
                == bins_t({10, 1, 2})));  // overflow, makes no sense
    BOOST_TEST((g1Cl.neighborHoodIndices({{5}}, 1)
                == bins_t({4, 5, 6})));  // overflow, makes no sense

    typedef Grid<double, EAxisClosed, EAxisClosed> Grid2Closed_t;
    // typedef Grid<double, EAxisClosed, EAxisClosed, EAxisClosed>
    // Grid3Closed_t;
    EAxisClosed   e(0.0, 1.0, 5u);
    EAxisClosed   f(0.0, 1.0, 5u);
    EAxisClosed   g(0.0, 1.0, 5u);
    Grid2Closed_t g2Cl(std::make_tuple(std::move(e), std::move(f)));
    BOOST_TEST(g2Cl.neighborHoodIndices({{3, 3}}, 1)
               == bins_t({16, 17, 18, 23, 24, 25, 30, 31, 32}));
    BOOST_TEST(g2Cl.neighborHoodIndices({{1, 1}}, 1)
               == bins_t({8, 9, 15, 16, 12, 19, 36, 37, 40}));
    BOOST_TEST(g2Cl.neighborHoodIndices({{1, 5}}, 1)
               == bins_t({11, 12, 18, 19, 39, 40, 8, 15, 36}));
    BOOST_TEST(g2Cl.neighborHoodIndices({{5, 1}}, 1)
               == bins_t({36, 37, 29, 30, 33, 40, 8, 9, 12}));
    BOOST_TEST(g2Cl.neighborHoodIndices({{5, 5}}, 1)
               == bins_t({39, 40, 32, 33, 11, 12, 29, 36, 8}));

    bins_t all({8,  9,  10, 11, 12, 15, 16, 17, 18, 19, 22, 23, 24,
                25, 26, 29, 30, 31, 32, 33, 36, 37, 38, 39, 40});
    BOOST_TEST(g2Cl.neighborHoodIndices({{3, 3}}, 2) == all);
    BOOST_TEST(g2Cl.neighborHoodIndices({{1, 1}}, 2) == all);
    BOOST_TEST(g2Cl.neighborHoodIndices({{1, 5}}, 2) == all);
    BOOST_TEST(g2Cl.neighborHoodIndices({{5, 1}}, 2) == all);
    BOOST_TEST(g2Cl.neighborHoodIndices({{5, 5}}, 2) == all);

    // @TODO 3D test would be nice, but should essentially not be a problem if
    // 2D works.

    // clang-format off
    /*
     *       1   2    3    4    5
     *   |------------------------|
     * 1 |  8 |  9 | 10 | 11 | 12 |
     *   |----|----|----|----|----|
     * 2 | 15 | 16 | 17 | 18 | 19 |
     *   |----|----|----|----|----|
     * 3 | 22 | 23 | 24 | 25 | 26 |
     *   |----|----|----|----|----|
     * 4 | 29 | 30 | 31 | 32 | 33 |
     *   |----|----|----|----|----|
     * 5 | 36 | 37 | 38 | 39 | 40 |
     *   |------------------------|
     */
    // clang-format on
  }

  BOOST_AUTO_TEST_CASE(closestPoints)
  {
    typedef std::array<double, 3> Point;
    typedef std::set<size_t> bins_t;
    typedef EquidistantAxis  EAxis;
    typedef Grid<double, EAxis> Grid1_t;
    typedef Grid<double, EAxis, EAxis> Grid2_t;
    typedef Grid<double, EAxis, EAxis, EAxis> Grid3_t;

    EAxis   a(0.0, 1.0, 10u);
    EAxis   b(0.0, 1.0, 5u);
    EAxis   c(0.0, 1.0, 3u);
    Grid1_t g1(std::make_tuple(std::move(a)));
    Grid2_t g2(std::make_tuple(std::move(a), std::move(b)));
    Grid3_t g3(std::make_tuple(std::move(a), std::move(b), std::move(c)));

    // clang-format off
    // 1D case
    BOOST_TEST((g1.closestPointsIndices(Point({{0.52}})) == bins_t({6, 7})));
    BOOST_TEST((g1.closestPointsIndices(Point({{0.98}})) == bins_t({10, 11})));

    // 2D case
    BOOST_TEST((g2.closestPointsIndices(Point({{0.52, 0.08}})) == bins_t({43, 44, 50, 51})));
    BOOST_TEST((g2.closestPointsIndices(Point({{0.05, 0.08}})) == bins_t({8, 9, 15, 16})));

    // 3D case
    BOOST_TEST((g3.closestPointsIndices(Point({{0.23, 0.13, 0.61}})) == bins_t({112, 113, 117, 118, 147, 148, 152, 153})));
    BOOST_TEST((g3.closestPointsIndices(Point({{0.52, 0.35, 0.71}})) == bins_t({223, 224, 228, 229, 258, 259, 263, 264})));

    typedef Axis<AxisType::Equidistant, AxisBoundaryType::Closed>  EAxisClosed;
    typedef Grid<double, EAxisClosed> Grid1Cl_t;
    typedef Grid<double, EAxisClosed, EAxisClosed> Grid2Cl_t;
    //typedef Grid<double, EAxisClosed, EAxisClosed, EAxisClosed> Grid3Cl_t;
    EAxisClosed   aCl(0.0, 1.0, 10u);
    EAxisClosed   bCl(0.0, 1.0, 5u);
    EAxisClosed   cCl(0.0, 1.0, 3u);
    Grid1Cl_t g1Cl(std::make_tuple(std::move(aCl)));
    Grid2Cl_t g2Cl(std::make_tuple(std::move(aCl), std::move(bCl)));

    // 1D case
    BOOST_TEST((g1Cl.closestPointsIndices(Point({{0.52}})) == bins_t({6, 7})));
    BOOST_TEST((g1Cl.closestPointsIndices(Point({{0.98}})) == bins_t({10, 1})));

    // 2D case
    BOOST_TEST((g2Cl.closestPointsIndices(Point({{0.52, 0.08}})) == bins_t({43, 44, 50, 51})));
    BOOST_TEST((g2Cl.closestPointsIndices(Point({{0.52, 0.68}})) == bins_t({46, 47, 53, 54})));
    BOOST_TEST((g2Cl.closestPointsIndices(Point({{0.52, 0.88}})) == bins_t({47, 43, 54, 50})));
    BOOST_TEST((g2Cl.closestPointsIndices(Point({{0.05, 0.08}})) == bins_t({8, 9, 15, 16})));
    BOOST_TEST((g2Cl.closestPointsIndices(Point({{0.9, 0.95}})) == bins_t({75, 71, 12, 8})));

    // @TODO: 3D checks would also be nice

    typedef Axis<AxisType::Equidistant, AxisBoundaryType::Bound>  EAxisOpen;
    typedef Grid<double, EAxisOpen> Grid1Op_t;
    typedef Grid<double, EAxisOpen, EAxisOpen> Grid2Op_t;
    //typedef Grid<double, EAxisOpen, EAxisOpen, EAxisOpen> Grid3Op_t;

    EAxisOpen  aOp(0.0, 1.0, 10u);
    EAxisOpen  bOp(0.0, 1.0, 5u);
    EAxisOpen  cOp(0.0, 1.0, 3u);
    Grid1Op_t g1Op(std::make_tuple(std::move(aOp)));
    Grid2Op_t g2Op(std::make_tuple(std::move(aOp), std::move(bOp)));

    // 1D case
    BOOST_TEST((g1Op.closestPointsIndices(Point({{0.52}})) == bins_t({6, 7})));
    BOOST_TEST((g1Op.closestPointsIndices(Point({{0.98}})) == bins_t({10})));
    BOOST_TEST((g1Op.closestPointsIndices(Point({{0.88}})) == bins_t({9, 10})));

    // 2D case
    BOOST_TEST((g2Op.closestPointsIndices(Point({{0.52, 0.08}})) == bins_t({43, 44, 50, 51})));
    BOOST_TEST((g2Op.closestPointsIndices(Point({{0.52, 0.68}})) == bins_t({46, 47, 53, 54})));
    BOOST_TEST((g2Op.closestPointsIndices(Point({{0.52, 0.88}})) == bins_t({47, 54})));
    BOOST_TEST((g2Op.closestPointsIndices(Point({{0.05, 0.1}})) == bins_t({8, 9, 15, 16})));
    BOOST_TEST((g2Op.closestPointsIndices(Point({{0.95, 0.95}})) == bins_t({75})));
    
    // @TODO: 3D checks would also be nice

    /*
     *       1    2    3    4    5
     *    |------------------------|
     *  1 |  8 |  9 | 10 | 11 | 12 |
     *    |----|----|----|----|----|
     *  2 | 15 | 16 | 17 | 18 | 19 |
     *    |----|----|----|----|----|
     *  3 | 22 | 23 | 24 | 25 | 26 |
     *    |----|----|----|----|----|
     *  4 | 29 | 30 | 31 | 32 | 33 |
     *    |----|----|----|----|----|
     *  5 | 36 | 37 | 38 | 39 | 40 |
     *    |------------------------|
     *  6 | 43 | 44 | 45 | 46 | 47 |
     *    |------------------------|
     *  7 | 50 | 51 | 52 | 53 | 54 |
     *    |------------------------|
     *  8 | 57 | 58 | 59 | 60 | 61 |
     *    |------------------------|
     *  9 | 64 | 65 | 66 | 67 | 68 |
     *    |------------------------|
     * 10 | 71 | 72 | 73 | 74 | 75 |
     *    |------------------------|
     * 77   78   79   80   81   82   83
     */

    // clang-format on
  }

}  // namespace Test

}  // namespace Acts
