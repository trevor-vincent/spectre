// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <catch.hpp>

#include "Domain/CoordinateMaps/AffineMap.hpp"
#include "Domain/CoordinateMaps/ProductMaps.hpp"
#include "Domain/Domain.hpp"
#include "Domain/DomainCreators/Rectangle.hpp"
#include "Domain/DomainCreators/RegisterDerivedWithCharm.hpp"
#include "Utilities/MakeVector.hpp"
#include "tests/Unit/Domain/CoordinateMaps/TestMapHelpers.hpp"
#include "tests/Unit/Domain/DomainTestHelpers.hpp"
#include "tests/Unit/TestFactoryCreation.hpp"
#include "tests/Unit/TestHelpers.hpp"

namespace {
using AffineMap = CoordinateMaps::AffineMap;
using AffineMap2D = CoordinateMaps::ProductOf2Maps<AffineMap, AffineMap>;
void test_rectangle_construction(
    const DomainCreators::Rectangle<Frame::Inertial>& rectangle,
    const std::array<double, 2>& lower_bound,
    const std::array<double, 2>& upper_bound,
    const std::vector<std::array<size_t, 2>>& expected_extents,
    const std::vector<std::array<size_t, 2>>& expected_refinement_level,
    const std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>&
        expected_block_neighbors,
    const std::vector<std::unordered_set<Direction<2>>>&
        expected_external_boundaries) {
  const auto domain = rectangle.create_domain();
  const auto& block = domain.blocks()[0];
  const auto& neighbors = block.neighbors();
  const auto& external_boundaries = block.external_boundaries();

  CHECK(block.id() == 0);
  CHECK(rectangle.initial_extents() == expected_extents);
  CHECK(rectangle.initial_refinement_levels() == expected_refinement_level);

  test_domain_construction(
      domain, expected_block_neighbors, expected_external_boundaries,
      make_vector(make_coordinate_map_base<Frame::Logical, Frame::Inertial>(
          AffineMap2D{AffineMap{-1., 1., lower_bound[0], upper_bound[0]},
                      AffineMap{-1., 1., lower_bound[1], upper_bound[1]}})));
  test_initial_domain(domain, rectangle.initial_refinement_levels());
}
}  // namespace

SPECTRE_TEST_CASE("Unit.Domain.DomainCreators.Rectangle", "[Domain][Unit]") {
  const std::vector<std::array<size_t, 2>> grid_points{{{4, 6}}},
      refinement_level{{{3, 2}}};
  const std::array<double, 2> lower_bound{{-1.2, 3.0}}, upper_bound{{0.8, 5.0}};
  // default OrientationMap is aligned
  const OrientationMap<2> aligned_orientation{};

  const DomainCreators::Rectangle<Frame::Inertial> rectangle{
      lower_bound, upper_bound, std::array<bool, 2>{{false, false}},
      refinement_level[0], grid_points[0]};
  test_rectangle_construction(
      rectangle, lower_bound, upper_bound, grid_points, refinement_level,
      std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>{{}},
      std::vector<std::unordered_set<Direction<2>>>{
          {{Direction<2>::lower_xi()},
           {Direction<2>::upper_xi()},
           {Direction<2>::lower_eta()},
           {Direction<2>::upper_eta()}}});

  const DomainCreators::Rectangle<Frame::Inertial> periodic_x_rectangle{
      lower_bound, upper_bound, std::array<bool, 2>{{true, false}},
      refinement_level[0], grid_points[0]};
  test_rectangle_construction(
      periodic_x_rectangle, lower_bound, upper_bound, grid_points,
      refinement_level,
      std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>{
          {{Direction<2>::lower_xi(), {0, aligned_orientation}},
           {Direction<2>::upper_xi(), {0, aligned_orientation}}}},
      std::vector<std::unordered_set<Direction<2>>>{
          {{Direction<2>::lower_eta()}, {Direction<2>::upper_eta()}}});

  const DomainCreators::Rectangle<Frame::Inertial> periodic_y_rectangle{
      lower_bound, upper_bound, std::array<bool, 2>{{false, true}},
      refinement_level[0], grid_points[0]};
  test_rectangle_construction(
      periodic_y_rectangle, lower_bound, upper_bound, grid_points,
      refinement_level,
      std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>{
          {{Direction<2>::lower_eta(), {0, aligned_orientation}},
           {Direction<2>::upper_eta(), {0, aligned_orientation}}}},
      std::vector<std::unordered_set<Direction<2>>>{
          {{Direction<2>::lower_xi()}, {Direction<2>::upper_xi()}}});

  const DomainCreators::Rectangle<Frame::Inertial> periodic_xy_rectangle{
      lower_bound, upper_bound, std::array<bool, 2>{{true, true}},
      refinement_level[0], grid_points[0]};
  test_rectangle_construction(
      periodic_xy_rectangle, lower_bound, upper_bound, grid_points,
      refinement_level,
      std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>{
          {{Direction<2>::lower_xi(), {0, aligned_orientation}},
           {Direction<2>::upper_xi(), {0, aligned_orientation}},
           {Direction<2>::lower_eta(), {0, aligned_orientation}},
           {Direction<2>::upper_eta(), {0, aligned_orientation}}}},
      std::vector<std::unordered_set<Direction<2>>>{{}});

  // Test serialization of the map
  DomainCreators::register_derived_with_charm();

  const auto base_map =
      make_coordinate_map_base<Frame::Logical, Frame::Inertial>(
          AffineMap2D{AffineMap{-1., 1., lower_bound[0], upper_bound[0]},
                      AffineMap{-1., 1., lower_bound[1], upper_bound[1]}});
  const auto base_map_deserialized = serialize_and_deserialize(base_map);
  using MapType =
      const CoordinateMap<Frame::Logical, Frame::Inertial, AffineMap2D>*;
  REQUIRE(dynamic_cast<MapType>(base_map.get()) != nullptr);
  const auto coord_map = make_coordinate_map<Frame::Logical, Frame::Inertial>(
      AffineMap2D{AffineMap{-1., 1., lower_bound[0], upper_bound[0]},
                  AffineMap{-1., 1., lower_bound[1], upper_bound[1]}});
  CHECK(*dynamic_cast<MapType>(base_map.get()) == coord_map);
}

SPECTRE_TEST_CASE("Unit.Domain.DomainCreators.Rectangle.Factory",
                  "[Domain][Unit]") {
  const auto domain_creator =
      test_factory_creation<DomainCreator<2, Frame::Inertial>>(
          "  Rectangle:\n"
          "    LowerBound: [0,0]\n"
          "    UpperBound: [1,2]\n"
          "    IsPeriodicIn: [True,False]\n"
          "    InitialGridPoints: [3,4]\n"
          "    InitialRefinement: [2,3]\n");
  const auto* rectangle_creator =
      dynamic_cast<const DomainCreators::Rectangle<Frame::Inertial>*>(
          domain_creator.get());
  test_rectangle_construction(
      *rectangle_creator, {{0., 0.}}, {{1., 2.}}, {{{3, 4}}}, {{{2, 3}}},
      std::vector<std::unordered_map<Direction<2>, BlockNeighbor<2>>>{
          {{Direction<2>::lower_xi(), {0, {}}},
           {Direction<2>::upper_xi(), {0, {}}}}},
      std::vector<std::unordered_set<Direction<2>>>{
          {{Direction<2>::lower_eta()}, {Direction<2>::upper_eta()}}});
}
