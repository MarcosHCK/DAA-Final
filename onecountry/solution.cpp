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
#include <tuple>
#include <variant>
#include <vector>

template<typename T, int dims> class Point : public std::array<T, dims>
{
public:
  inline Point (const Point<T, dims>& o) : std::array<T, dims> (o) { }
  template<typename... _Args>
  inline Point (_Args&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
  template<typename... _Args>
  inline Point (_Args&&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
};

template<typename T> class Rectangle : public Point<T, 4>
{
public:
  inline Rectangle (const Rectangle& o) : Point<T, 4> (o) {}
  inline Rectangle (const Point<T, 4>& o) : Point<T, 4> (o) {}

  inline constexpr const std::array<T, 2> get_bottom () const
    {
      return { (*this) [0], (*this) [1] };
    }

  inline constexpr const std::array<T, 2> get_top () const
    {
      return { (*this) [2], (*this) [3] };
    }
};

template<typename T, typename Ti> class Rec
{
private:
  const Point<T, 2>* bounds = nullptr;
  const Ti index;
public:
  inline Rec () = delete;
  inline Rec (const Rec<T, Ti>& o) : bounds (o.bounds), index (o.index) {}
  inline Rec (const Rec<T, Ti>& o, const Ti& index) : bounds (o.bounds), index (index) {}
  inline Rec (const Point<T, 2>* bounds, const Ti& index) : bounds (bounds), index (index) {}

  inline constexpr const T& get_bottom () const { return (*bounds) [0]; }
  inline constexpr const Ti& get_index () const { return index; }
  inline constexpr const T& get_top () const { return (*bounds) [1]; }
};

template<typename T, typename Ti> class Recs : public std::array<Rec<T, Ti>*, 2>
{
private:
  const Ti length;
public:
  inline Recs () = delete;
  inline Recs (Rec<T, Ti>* first, Rec<T, Ti>* second, const Ti& length) : std::array<Rec<T, Ti>*, 2> ({ first, second }), length (length) {}
  inline constexpr const Ti& get_length () const { return length; }
};

template<typename T, typename Ti> class Problem
{
public:

  static inline bool is_good (const Recs<T, Ti>& dims)
    {
      Ti size;

      if ((size = dims.get_length ()) < 2)

        return true;
      else
        {
          auto p = slice<0, 1> (dims, size / 2);

          if (std::get<0> (p) == -1)
            {
              return false;
            }
          else switch (std::get<0> (p))
            {
              case 0: return split<0, 1> (dims, std::get<Ti> (std::get<1> (p)));
              case 1: return split<1, 0> (dims, std::get<Ti> (std::get<1> (p)));
              default: return false;
            }
        }
    }

  template<int axis, int axis2>
  static inline std::tuple<int, std::variant<Ti, std::tuple<Ti, Ti>>> slice (const Recs<T, Ti>& dims, const Ti& hint)
    {
      auto size = dims.get_length ();
      auto rightmost = std::vector<Ti> (size, 0);
      auto topmost = std::vector<Ti> (size, 0);

      for (Ti c, i = 0, j = 0; i < size; j = i++)
        {
          if (dims [axis] [c = rightmost [j]].get_top () > dims [axis] [i].get_top ())

            rightmost [i] = c;
          else
            rightmost [i] = i;
        }

      for (Ti c, i = 0, j = 0; i < size; j = i++)
        {
          if (dims [axis2] [c = topmost [j]].get_top () > dims [axis2] [i].get_top ())

            topmost [i] = c;
          else
            topmost [i] = i;
        }

      auto p = slice_axis<axis, axis2> (dims, hint, rightmost, topmost);

      if (std::get<0> (p) != -1)

        return std::move (p);
      else
        {
          if constexpr (axis == 1)

            return std::make_tuple (-1, 0);
          else
            return slice_axis<axis2, axis> (dims, hint, topmost, rightmost);
        }
    }

  template<int axis, int axis2>
  static inline std::tuple<int, std::variant<Ti, std::tuple<Ti, Ti>>> slice_axis (const Recs<T, Ti>& dims, const Ti& hint,
      const std::vector<Ti>& rightmost, const std::vector<Ti>& topmost)
    {
      auto size = dims.get_length ();

      for (Ti i = hint, j = hint; i > 0 || j < size - 1;)
        {
          if (i > 0)
            {
              if (dims [axis] [i].get_bottom () < dims [axis] [rightmost [i - 1]].get_top ()) --i; else
                return std::make_tuple (axis, i);
            }

          if (j < size - 1)
            {
              if (dims [axis] [j + 1].get_bottom () < dims [axis] [rightmost [j]].get_top ()) ++j; else
                return std::make_tuple (axis, j + 1);
            }
        }

      return std::make_tuple (-1, 0);
    }

  static inline bool solve (const std::vector<Point<T, 4>>&& recs)
    {
      Point<T, 2>* points = (Point<T, 2>*) malloc ((recs.size () << 1) * sizeof (Point<T, 2>));
      Point<T, 2>* pointptr [2] = { & points [0], & points [recs.size ()] };

      auto good = is_good (sort (std::move (recs), pointptr));
      return (free (points), good);
    }

  static inline Recs<T, Ti> sort (const std::vector<Point<T, 4>>&& recs, Point<T, 2>* points [2])
    {
      std::vector<std::tuple<Ti, Ti, Rectangle<T>>> sorted;
      (sorted = std::vector<std::tuple<Ti, Ti, Rectangle<T>>> ()).reserve (recs.size ());

      auto sortedx = (Rec<T, Ti>*) malloc (recs.size () * sizeof (Rec<T, Ti>));
      auto sortedy = (Rec<T, Ti>*) malloc (recs.size () * sizeof (Rec<T, Ti>));

      for (Ti i = 0; i < recs.size (); ++i)
        {
          sorted.push_back (std::make_tuple (0, 0, Rectangle<T> (recs [i])));
        }

      /* sort by y0 */

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rectangle<T>>& a, const std::tuple<Ti, Ti, Rectangle<T>>& b)
        {
          return std::get<2> (a).get_bottom () [1] < std::get<2> (b).get_bottom () [1];
        });

      for (Ti i = 0; i < sorted.size (); ++i)
        {
          sorted [i] = std::make_tuple (0, i, std::move (std::get<2> (sorted [i])));
        }

      /* sort by x0 */

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rectangle<T>>& a, const std::tuple<Ti, Ti, Rectangle<T>>& b)
        {
          return std::get<2> (a).get_bottom () [0] < std::get<2> (b).get_bottom () [0];
        });

      for (Ti i = 0; i < sorted.size (); ++i)
        {
          sorted [i] = std::make_tuple (i, std::get<1> (sorted [i]), std::move (std::get<2> (sorted [i])));
        }

      /* transfer sorted rects */
      int i = 0, j = 0;

      for (const std::tuple<Ti, Ti, Rectangle<T>>& t : sorted)
        {
          Point<T, 2>* p;
          *(p = & points [0] [i]) = Point<T, 2> (std::get<2> (t).get_bottom () [0], std::get<2> (t).get_top () [0]);

          new (&sortedx [i]) Rec<T, Ti> (p, std::get<1> (t));
          ++i;
        }

      std::sort (sorted.begin (), sorted.end (), [](const std::tuple<Ti, Ti, Rectangle<T>>& a, const std::tuple<Ti, Ti, Rectangle<T>>& b)
        {
          return std::get<1> (a) < std::get<1> (b);
        });

      for (const std::tuple<Ti, Ti, Rectangle<T>>& t : sorted)
        {
          Point<T, 2>* p;
          *(p = & points [1] [j]) = Point<T, 2> (std::get<2> (t).get_bottom () [1], std::get<2> (t).get_top () [1]);

          new (&sortedy [j]) Rec<T, Ti> (p, std::get<0> (t));
          ++j;
        }

      return Recs<T, Ti> (sortedx, sortedy, sorted.size ());
    }

  template<int axis, int axis2>
  static inline bool split (const Recs<T, Ti>& dims, const Ti& second)
    {
      if (second == 1)
        {
          if (second == dims.get_length () - 1)

            return true;
          else
            return is_good (split_right<axis, axis2> (dims, second));
        }
      else if (second == dims.get_length () - 1)
        {
          if (second == 1)

            return true;
          else
            return is_good (split_left<axis, axis2> (dims, second));
        }
      else
        {
          if (! is_good (split_left<axis, axis2> (dims, second)))

            return false;
          else
            return is_good (split_right<axis, axis2> (dims, second));
        }
    }

  template<int axis, int axis2>
  static inline Recs<T, Ti> split_left (const Recs<T, Ti>& dims, const Ti& second)
    {
      std::vector<int> translate (dims.get_length (), 0);

      auto vec1 = (Rec<T, Ti>*) malloc (second * sizeof (Rec<T, Ti>));
      auto vec2 = (Rec<T, Ti>*) malloc (second * sizeof (Rec<T, Ti>));

      for (Ti i = 0, j = 0; i < dims.get_length (); ++i) if (dims [axis2] [i].get_index () < second)
        {
          translate [i] = j;
          new (&vec2 [j++]) Rec<T, Ti> (dims [axis2] [i]);
        }

      for (Ti i = 0; i < second; ++i)
        {
          new (&vec1 [i]) Rec<T, Ti> (dims [axis] [i], translate [dims [axis] [i].get_index ()]);
        }

      if constexpr (axis == 0) return Recs (vec1, vec2, second); else return Recs (vec2, vec1, second);
    }

  template<int axis, int axis2>
  static inline Recs<T, Ti> split_right (const Recs<T, Ti>& dims, const Ti& second)
    {
      Ti others = dims.get_length () - second;
      std::vector<int> translate (dims.get_length (), 0);

      auto vec1 = (Rec<T, Ti>*) malloc (others * sizeof (Rec<T, Ti>));
      auto vec2 = (Rec<T, Ti>*) malloc (others * sizeof (Rec<T, Ti>));

      for (Ti i = 0, j = 0; i < dims.get_length (); ++i) if (dims [axis2] [i].get_index () >= second)
        {
          translate [i] = j;
          new (&vec2 [j++]) Rec<T, Ti> (dims [axis2] [i], dims [axis2] [i].get_index () - second);
        }

      for (Ti i = second, j = 0; i < dims.get_length (); ++i)
        {
          new (&vec1 [j++]) Rec<T, Ti> (dims [axis] [i], translate [dims [axis] [i].get_index ()]);
        }

      if constexpr (axis == 0) return Recs (vec1, vec2, others); else return Recs (vec2, vec1, others);
    }
};

template<typename T, typename Ti> int program ()
{
  Ti nrecs;
  std::vector<Point<T, 4>> recs;
  T x1, y1, x2, y2;

  std::cin >> nrecs;

  for (Ti i = 0; i < nrecs; ++i)
    {
      std::cin >> x1 >> y1 >> x2 >> y2;
      recs.push_back (Point<T, 4> (x1, y1, x2, y2));
    }

  std::cout << (! Problem<T, Ti> ().solve (std::move (recs)) ? "NO" : "YES") << std::endl;
  return 0;
}

int main ()
{
  return program<long, int> ();
}
