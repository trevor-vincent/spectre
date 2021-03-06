// Distributed under the MIT License.
// See LICENSE.txt for details.

/// \file
/// Defines the class Rotation.

#pragma once

#include <array>
#include <memory>

#include "DataStructures/Tensor/Tensor.hpp"
#include "Parallel/CharmPupable.hpp"

namespace CoordinateMaps {

/// \cond HIDDEN_SYMBOLS
template <size_t Dim>
class Rotation;
/// \endcond

/*!
 * \ingroup CoordinateMapsGroup
 * \brief Spatial rotation in two dimensions.
 *
 * Let \f$(R,\Phi)\f$ be the polar coordinates associated with
 * \f$(\xi,\eta)\f$.
 * Let \f$(r,\phi)\f$ be the polar coordinates associated with \f$(x,y)\f$.
 * Applies the spatial rotation \f$\phi = \Phi + \alpha\f$.
 *
 * The formula for the mapping is:
 *\f{eqnarray*}
  x &=& \xi \cos \alpha - \eta \sin \alpha \\
  y &=& \xi \sin \alpha + \eta \cos \alpha
  \f}.
 */
template <>
class Rotation<2> {
 public:
  static constexpr size_t dim = 2;

  /// Constructor.
  ///
  /// \param rotation_angle the angle \f$\alpha\f$ (in radians).
  explicit Rotation(double rotation_angle);
  Rotation() = default;
  ~Rotation() = default;
  Rotation(const Rotation&) = default;
  Rotation& operator=(const Rotation&) = default;
  Rotation(Rotation&&) noexcept = default;  // NOLINT
  Rotation& operator=(Rotation&&) = default;

  template <typename T>
  std::array<std::decay_t<tt::remove_reference_wrapper_t<T>>, 2> operator()(
      const std::array<T, 2>& xi) const;

  template <typename T>
  std::array<std::decay_t<tt::remove_reference_wrapper_t<T>>, 2> inverse(
      const std::array<T, 2>& x) const;

  template <typename T>
  Tensor<std::decay_t<tt::remove_reference_wrapper_t<T>>,
         tmpl::integral_list<std::int32_t, 2, 1>,
         index_list<SpatialIndex<2, UpLo::Up, Frame::NoFrame>,
                    SpatialIndex<2, UpLo::Lo, Frame::NoFrame>>>
  jacobian(const std::array<T, 2>& /*xi*/) const;

  template <typename T>
  Tensor<std::decay_t<tt::remove_reference_wrapper_t<T>>,
         tmpl::integral_list<std::int32_t, 2, 1>,
         index_list<SpatialIndex<2, UpLo::Up, Frame::NoFrame>,
                    SpatialIndex<2, UpLo::Lo, Frame::NoFrame>>>
  inv_jacobian(const std::array<T, 2>& /*xi*/) const;

  void pup(PUP::er& p);  // NOLINT

 private:
  friend bool operator==(const Rotation<2>& lhs,
                         const Rotation<2>& rhs) noexcept;

  double rotation_angle_{std::numeric_limits<double>::signaling_NaN()};
  tnsr::ij<double, 2, Frame::Grid> rotation_matrix_{
      std::numeric_limits<double>::signaling_NaN()};
};

bool operator!=(const Rotation<2>& lhs, const Rotation<2>& rhs) noexcept;

/*!
 * \ingroup CoordinateMapsGroup
 * \brief Spatial rotation in three dimensions using Euler angles
 *
 * Rotation angles should be specified in degrees.
 * First rotation \f$\alpha\f$ is about z axis.
 * Second rotation \f$\beta\f$ is about rotated y axis.
 * Third rotation \f$\gamma\f$ is about rotated z axis.
 * These rotations are of the \f$(\xi,\eta,\zeta)\f$ coordinate system with
 * respect
 * to the grid coordinates \f$(x,y,z)\f$.
 *
 * The formula for the mapping is:
 * \f{eqnarray*}
 *   x &=& \xi (\cos\gamma \cos\beta \cos\alpha - \sin\gamma \sin\alpha)
 * + \eta (-\sin\gamma \cos\beta \cos\alpha - \cos\gamma \sin\alpha)
 * + \zeta \sin\beta \cos\alpha \\
 * y &=& \xi (\cos\gamma \cos\beta \sin\alpha + \sin\gamma \cos\alpha)
 * + \eta (-\sin\gamma \cos\beta \sin\alpha + \cos\gamma \cos\alpha)
 * + \zeta \sin\beta \sin\alpha \\
 *        z &=& -\xi \cos\gamma \sin\beta + \eta \sin\gamma \sin\beta
 *        +  \zeta \cos\beta
 *  \f}
 */
template <>
class Rotation<3> {
 public:
  static constexpr size_t dim = 3;

  /// Constructor.
  ///
  /// \param rotation_about_z the angle \f$\alpha\f$ (in radians).
  /// \param rotation_about_rotated_y the angle \f$\beta\f$ (in radians).
  /// \param rotation_about_rotated_z the angle \f$\gamma\f$ (in radians).
  Rotation(double rotation_about_z, double rotation_about_rotated_y,
           double rotation_about_rotated_z);
  Rotation() = default;
  ~Rotation() = default;
  Rotation(const Rotation&) = default;
  Rotation& operator=(const Rotation&) = default;
  Rotation(Rotation&&) noexcept = default;  // NOLINT
  Rotation& operator=(Rotation&&) = default;

  template <typename T>
  std::array<std::decay_t<tt::remove_reference_wrapper_t<T>>, 3> operator()(
      const std::array<T, 3>& xi) const;

  template <typename T>
  std::array<std::decay_t<tt::remove_reference_wrapper_t<T>>, 3> inverse(
      const std::array<T, 3>& x) const;

  template <typename T>
  Tensor<std::decay_t<tt::remove_reference_wrapper_t<T>>,
         tmpl::integral_list<std::int32_t, 2, 1>,
         index_list<SpatialIndex<3, UpLo::Up, Frame::NoFrame>,
                    SpatialIndex<3, UpLo::Lo, Frame::NoFrame>>>
  jacobian(const std::array<T, 3>& /*xi*/) const;

  template <typename T>
  Tensor<std::decay_t<tt::remove_reference_wrapper_t<T>>,
         tmpl::integral_list<std::int32_t, 2, 1>,
         index_list<SpatialIndex<3, UpLo::Up, Frame::NoFrame>,
                    SpatialIndex<3, UpLo::Lo, Frame::NoFrame>>>
  inv_jacobian(const std::array<T, 3>& /*xi*/) const;

  void pup(PUP::er& p);  // NOLINT

 private:
  friend bool operator==(const Rotation<3>& lhs,
                         const Rotation<3>& rhs) noexcept;

  double rotation_about_z_{std::numeric_limits<double>::signaling_NaN()};
  double rotation_about_rotated_y_{
      std::numeric_limits<double>::signaling_NaN()};
  double rotation_about_rotated_z_{
      std::numeric_limits<double>::signaling_NaN()};
  tnsr::ij<double, 3, Frame::Grid> rotation_matrix_{
      std::numeric_limits<double>::signaling_NaN()};
};

bool operator!=(const Rotation<3>& lhs, const Rotation<3>& rhs) noexcept;

}  // namespace CoordinateMaps
