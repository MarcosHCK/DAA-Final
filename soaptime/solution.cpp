/* Copyright 2024-2025 MarcosHCK
 * This file is part of DAA-Final-Project.
 *
 * DAA-Final-Project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DAA-Final-Project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DAA-Final-Project. If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <random>
#include <vector>

template<typename Tp, int k> class Point
{
private:
  int v [k];
public:
  inline Point () { }
  inline Point (const Tp v [k]) { memcpy (this->v, v, k * sizeof (Tp)); }
  inline Point (const std::array<Tp, k>& v) : Point (v.to_array ()) { }

  inline constexpr const Tp& operator[] (int i) const
    {
      if ((i >= 0) == false) std::__throw_invalid_argument ("should i >= 0");
      if ((i < k) == false) std::__throw_invalid_argument ("should i < k");
      return v [i];
    }

  template<int i>
  inline constexpr const Tp& get () const
    {
      static_assert (i >= 0, "should i >= 0");
      static_assert (i < k, "should i < k");
      return v [i];
    }

  inline constexpr const Tp& get_x () const { return get<0> (); }
  inline constexpr const Tp& get_y () const { return get<1> (); }
};

template<typename Pt, int k> class BallTree
{
  using PointT = Point<Pt, k>;
private:
  PointT centroid;
  std::unique_ptr<BallTree> left = nullptr;
  double radius;
  std::unique_ptr<BallTree> right = nullptr;

  template<typename Ti>
  static inline std::array<Pt, k> bigger_axis (const Ti& begin, const Ti& end)
    {
      return bigger_helper (begin, end, std::make_integer_sequence<int, k> ());
    }

  template<typename Ti, int ... Is>
  static inline std::array<Pt, k> bigger_helper (const Ti& begin, const Ti& end, std::integer_sequence<int, Is...> const&)
    {
      return { bigger_on_index<Ti, Is> (begin, end)... };
    }

  template<typename Ti, int index>
  static inline Pt bigger_on_index (const Ti& begin, const Ti& end)
    {
      Pt t, value = std::numeric_limits<Pt>::min ();

      for (auto iter = begin; iter != end; ++iter)

        if ((t = (*iter) [index]) > value) value = t;
      return value;
    }

  template<typename Ti>
  static inline Ti calc_mean (const Ti& begin, const Ti& end, int axis)
    {
      Pt acc = 0, mean, t;
      Ti best = end;
      int count = 0;

      for (auto iter = begin; iter != end; ++count, ++iter) acc += (*iter) [axis];
      mean = acc / static_cast<Pt> (count);

      for (auto iter = begin; iter != end; ++iter)

        if ((t = (*iter) [axis]) >= mean && (best == end || t < (*best) [axis])) best = iter;
      return best != end ? best : begin;
    }

  template<typename Ti>
  static double calc_radius (const Ti& begin, const Ti& end, const PointT& center)
    {
      double bigger = 0, t;
      for (auto iter = begin; iter != end; ++iter)

        if ((t = distance (center, *iter)) > bigger) bigger = t;
      return bigger;
    }

  static inline double distance (const PointT& a, const PointT& b)
    {
      return std::sqrt (distance_helper (a, b, std::make_integer_sequence<int, k> ()));
    }

  template<int ... Is>
  static inline Pt distance_helper (const PointT& a, const PointT& b, std::integer_sequence<int, Is...> const&)
    {
      Pt acc = 0;
      using unused = int [];
      (void) unused { 0, (acc += ((a [Is] - b [Is]) * (a [Is] - b [Is])))... };
      return acc;
    }

  template<typename Ti>
  static inline int max_spread (const Ti& begin, const Ti& end)
    {
      auto bigger = bigger_axis (begin, end);
      auto smaller = smaller_axis (begin, end);
      return max_spread_helper (bigger, smaller, std::make_integer_sequence<int, k> ());
    }

  template<int ... Is>
  static inline int max_spread_helper (const std::array<Pt, k>& bigger, std::array<Pt, k> smaller, std::integer_sequence<int, Is...> const&)
    {
      int dim = 0;
      Pt max = std::numeric_limits<Pt>::min();

      using unused = int [];
      (void) unused { 0, (max_spread_update<Is> (bigger [Is] - smaller [Is], &max, &dim), 0)... };
      return dim;
    }

  template<int index>
  static inline void max_spread_update (const Pt& value, Pt* max, int* dim)
    {
      if (value > *max) { *max = value; *dim = index; };
    }

  template<typename Ti>
  static inline std::array<Pt, k> smaller_axis (const Ti& begin, const Ti& end)
    {
      return smaller_helper (begin, end, std::make_integer_sequence<int, k> ());
    }

  template<typename Ti, int ... Is>
  static inline std::array<Pt, k> smaller_helper (const Ti& begin, const Ti& end, std::integer_sequence<int, Is...> const&)
    {
      return { smaller_on_index<Ti, Is> (begin, end)... };
    }

  template<typename Ti, int index>
  static inline Pt smaller_on_index (const Ti& begin, const Ti& end)
    {
      Pt t, value = std::numeric_limits<Pt>::max ();

      for (auto iter = begin; iter != end; ++iter)

        if ((t = (*iter) [index]) < value) value = t;
      return value;
    }

public:

  inline BallTree (const std::vector<PointT>& points) : BallTree (points.begin (), points.end ()) { }

  template<typename Ti>
  inline BallTree (const Ti& begin, const Ti& end)
    {
      Ti iter = begin;

      if ((++iter) == end)
        {
          centroid = *begin;
          radius = 0;
        }
      else
        {
          auto axis = max_spread (begin, end);
          auto mean = calc_mean (begin, end, axis);

          std::vector<PointT> leftvec;
          std::vector<PointT> rightvec;
          std::copy_if (begin, end, std::back_inserter (leftvec), [axis, mean] (const PointT& p) { return p [axis] < (*mean) [axis]; });
          std::copy_if (begin, end, std::back_inserter (rightvec), [axis, mean] (const PointT& p) { return p [axis] > (*mean) [axis]; });

          centroid = (*mean);
          left = leftvec.size () == 0 ? nullptr : std::make_unique<BallTree<Pt, k>> (leftvec);
          radius = calc_radius (begin, end, centroid);
          right = rightvec.size () == 0 ? nullptr : std::make_unique<BallTree<Pt, k>> (rightvec);
        }
    }
};

template<typename Pt, int k> std::vector<Point<Pt, k>> collect (int npoints)
{
  static_assert (k == 2, "should i == 2");
  auto points = std::vector<Point<Pt, k>> (npoints);

  for (int i = 0; i < npoints; ++i)
    {
      int v [k];
      scanf ("%i %i", &v[0], &v[1]);

      points [i] = Point<Pt, k> (v);
    }
  return points;
}

int main ()
{
  int ndwarfs, nstations;

  scanf ("%i %i", &ndwarfs, &nstations);
  auto dwarfs = collect<int, 2> (ndwarfs);
  auto stations = collect<int, 2> (nstations);
  auto kdtree = BallTree<int, 2> (stations);
  return 0;
}
