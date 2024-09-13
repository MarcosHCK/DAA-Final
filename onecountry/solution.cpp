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
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <vector>

template<typename T, int dims> class Point : public std::array<T, dims>
{
public:
  inline Point () = default;
  inline Point (const Point<T, dims>& o) : std::array<T, dims> (o) { }
  template<typename... _Args>
  inline Point (_Args&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
  template<typename... _Args>
  inline Point (_Args&&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }

  inline std::string to_string () const
    {
      return to_string_helper (std::make_integer_sequence<int, dims> ());
    }

  template<int ... Is>
  inline std::string to_string_helper (std::integer_sequence<int, Is ...> const&) const
    {
      auto first = 1;
      auto ss = std::stringstream ();

      using unused = int [];
      (void) unused { (first = (ss << std::string (first == 1 ? "<" : ", ") << (*this) [Is], 0))... };
      return ((ss << ">"), ss.str ());
    }
};

template<typename T> class Rectangle
{
private:
  Point<T, 2> bottom;
  Point<T, 2> top;

public:
  inline Rectangle() = default;
  inline Rectangle (const Rectangle<T>& o) : bottom (o.bottom), top (o.top) { }
  inline Rectangle (const Point<T, 2>& bottom, const Point<T, 2>& top) : bottom (bottom), top (top) {}
  inline Rectangle (const Point<T, 2>&& bottom, const Point<T, 2>&& top) : bottom (std::move (bottom)), top (std::move (top)) {}

  inline constexpr const Point<T, 2>& get_bottom () const { return bottom; }
  inline constexpr const Point<T, 2>& get_top () const { return top; }

  inline constexpr std::string to_string () const
    {
      return (("(" + bottom.to_string ()) + ", " + top.to_string ()) + ")";
    }
};

template<typename T, typename Ti> class Rec : public Rectangle<T>
{
private:
  Ti index = 0;
public:
  inline Rec () = default;
  inline Rec (const Rec<T, Ti>& o) : Rectangle<T> (o), index (o.index) { }
  inline Rec (const Point<T, 2>& bottom, const Point<T, 2>& top) : Rectangle<T> (bottom, top) {}
  inline Rec (const Point<T, 2>&& bottom, const Point<T, 2>&& top) : Rectangle<T> (std::move (bottom), std::move (top)) {}

  inline constexpr int get_index () const { return index; }
  inline constexpr void set_index (int index) { this->index = index; }
};

template<typename T, typename Ti> class Problem
{
public:

  template<int axis>
  static bool is_good (std::array<std::vector<Rec<T, Ti>>, 2>&& dims)
    {
      if constexpr (axis == 1)

        return is_good_<1, 0> (dims);
      else
        {
          if (is_good_<0, 1> (dims))

            return true;
          else
            return is_good_<1, 0> (dims);
        }
    }

  template<int axis, int axis2>
  static inline bool is_good_ (const std::array<std::vector<Rec<T, Ti>>, 2>& dims)
    {
      Ti hint, size;

      if ((size = dims [axis].size ()) < 2)

        return true;
      else
        {
          if ((hint = slice<axis> (dims, size / 2)) < 0)

            return false;
          else
            return split<axis, axis2> (dims, hint);
        }
    }

  template<int axis>
  static inline Ti slice (const std::array<std::vector<Rec<T, Ti>>, 2>& dims, Ti hint)
    {
      auto size = dims [axis].size ();
      auto rightmost = std::vector<T> (size, 0);

      for (Ti i = 0, j = 0; i < size; ++i)
        {
          rightmost [i] = std::max (rightmost [j], dims [axis] [i].get_top () [axis]);
          j = i;
        }

      for (Ti i = hint, j = hint; i > 0 || j < size - 1;)
        {
          if (i > 0)
            {
              if (dims [axis] [i].get_bottom () [axis] < rightmost [i - 1]) --i; else
                return i;
            }

          if (j < size - 1)
            {
              if (dims [axis] [j + 1].get_bottom () [axis] < rightmost [j]) ++j; else
                return j + 1;
            }
        }

      return -1;
    }

  static inline bool solve (std::vector<Rec<T, Ti>>&& recs)
    {
      return is_good<0> (sort (std::move (recs)));
    }

  static inline std::array<std::vector<Rec<T, Ti>>, 2> sort (std::vector<Rec<T, Ti>>&& recs)
    {
      std::vector<std::tuple<Ti, Ti, Rec<T, Ti>>> sorted;
      std::vector<Rec<T, Ti>> sortedx;
      std::vector<Rec<T, Ti>> sortedy;

      (sorted = std::vector<std::tuple<Ti, Ti, Rec<T, Ti>>> ()).reserve (recs.size ());
      (sortedx = std::vector<Rec<T, Ti>> ()).reserve (recs.size ());
      (sortedy = std::vector<Rec<T, Ti>> ()).reserve (recs.size ());

      for (Ti i = 0; i < recs.size (); ++i) sorted.push_back (std::make_tuple (0, 0, recs [i]));

      /* sort by y0 */

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rec<T, Ti>>& a, const std::tuple<Ti, Ti, Rec<T, Ti>>& b)
        {
          return std::get<2> (a).get_bottom () [1] < std::get<2> (b).get_bottom () [1];
        });

      for (Ti i = 0; i < sorted.size (); ++i) sorted [i] = std::make_tuple (0, i, std::move (std::get<2> (sorted [i])));

      /* sort by x0 */

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rec<T, Ti>>& a, const std::tuple<Ti, Ti, Rec<T, Ti>>& b)
        {
          return std::get<2> (a).get_bottom () [0] < std::get<2> (b).get_bottom () [0];
        });

      for (Ti i = 0; i < sorted.size (); ++i) sorted [i] = std::make_tuple (i, std::get<1> (sorted [i]), std::move (std::get<2> (sorted [i])));

      /* transfer sorted rects */

      for (auto iter = sorted.begin (); iter != sorted.end (); ++iter)
        {
          Rec<T, Ti> rec;
          (rec = Rec<T, Ti> (std::get<2> (*iter))).set_index (std::get<1> (*iter));
          sortedx.push_back (std::move (rec));
        }

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rec<T, Ti>>& a, const std::tuple<Ti, Ti, Rec<T, Ti>>& b)
        {
          return std::get<1> (a) < std::get<1> (b);
        });

      for (auto iter = sorted.begin (); iter != sorted.end (); ++iter)
        {
          Rec<T, Ti> rec;
          (rec = Rec<T, Ti> (std::get<2> (*iter))).set_index (std::get<0> (*iter));
          sortedy.push_back (std::move (rec));
        }

      return { std::move (sortedx), std::move (sortedy) };
    }

  template<int axis, int axis2>
  static inline bool split (const std::array<std::vector<Rec<T, Ti>>, 2>& dims, Ti second)
    {
      if (! is_good<0> (split_left<axis, axis2> (dims, second)))

        return false;
      else
        return is_good<0> (split_right<axis, axis2> (dims, second));
    }

  template<int axis, int axis2>
  static inline std::array<std::vector<Rec<T, Ti>>, 2> split_left (const std::array<std::vector<Rec<T, Ti>>, 2>& dims, Ti second)
    {
      std::vector<int> translate (dims [axis].size (), 0);
      std::vector<Rec<T, Ti>> vec1, vec2;

      (vec2 = std::vector<Rec<T, Ti>> ()).reserve (second); for (Ti i = 0, j = 0; i < dims [axis2].size (); ++i) if (dims [axis2] [i].get_index () < second)
        {
          translate [i] = j++;
          vec2.push_back (Rec<T, Ti> (dims [axis2] [i]));
        }

      (vec1 = std::vector<Rec<T, Ti>> ()).reserve (second); for (Ti i = 0; i < second; ++i)
        {
          Rec<T, Ti> rec;
          (rec = Rec<T, Ti> (dims [axis] [i])).set_index (translate [dims [axis] [i].get_index ()]);
          vec1.push_back (std::move (rec));
        }

      if constexpr (axis == 0)
      
        return { std::move (vec1), std::move (vec2) };
      else
        return { std::move (vec2), std::move (vec1) };
    }

  template<int axis, int axis2>
  static inline std::array<std::vector<Rec<T, Ti>>, 2> split_right (const std::array<std::vector<Rec<T, Ti>>, 2>& dims, Ti second)
    {
      Ti others = dims [axis].size () - second;
      std::vector<int> translate (dims [axis].size (), 0);
      std::vector<Rec<T, Ti>> vec1, vec2;

      (vec2 = std::vector<Rec<T, Ti>> ()).reserve (others); for (Ti i = 0, j = 0; i < dims [axis2].size (); ++i) if (dims [axis2] [i].get_index () >= second)
        {
          Rec<T, Ti> rec;
          (rec = Rec<T, Ti> (dims [axis2] [i])).set_index (dims [axis2] [i].get_index () - second);
          translate [i] = j++;
          vec2.push_back (std::move (rec));
        }

      (vec1 = std::vector<Rec<T, Ti>> ()).reserve (others); for (Ti i = second; i < dims [axis].size (); ++i)
        {
          Rec<T, Ti> rec;
          (rec = Rec<T, Ti> (dims [axis] [i])).set_index (translate [dims [axis] [i].get_index ()]);
          vec1.push_back (std::move (rec));
        }

      if constexpr (axis == 0)
      
        return { std::move (vec1), std::move (vec2) };
      else
        return { std::move (vec2), std::move (vec1) };
    }
};

template<typename T, typename Ti> int program ()
{
  Ti nrecs;
  std::vector<Rec<T, Ti>> recs;
  T x1, y1, x2, y2;

  std::cin >> nrecs;

  for (Ti i = 0; i < nrecs; ++i)
    {
      std::cin >> x1 >> y1 >> x2 >> y2;
      recs.push_back (Rec<T, Ti> (Point<T, 2> (x1, y1), Point<T, 2> (x2, y2)));
    }

  std::cout << (! Problem<T, Ti> ().solve (std::move (recs)) ? "NO" : "YES") << std::endl;
  return 0;
}

int main ()
{
  return program<long, int> ();
}
