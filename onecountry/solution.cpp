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
#include <cstdio>
#include <vector>

template<typename T, int dims> class Point : public std::array<T, dims>
{
public:

  template<typename... _Args>
  inline Point (_Args&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
};

template<typename T> class Rectangle
{
private:
  Point<T, 2> bottom;
  Point<T, 2> top;

public:

  inline Rectangle (const Point<T, 2>& bottom, const Point<T, 2>& top) : bottom (bottom), top (top) {}
  inline Rectangle (const Point<T, 2>&& bottom, const Point<T, 2>&& top) : bottom (std::move (bottom)), top (std::move (top)) {}

  inline constexpr const Point<T, 2>& get_bottom () const { return bottom; }
  inline constexpr const Point<T, 2>& get_top () const { return top; }
};

template<typename T, typename Ti> class Rec : public Rectangle<T>
{
private:
  Ti deleted = 0;
  Ti index = 0;
public:
  inline Rec (const Point<T, 2>& bottom, const Point<T, 2>& top) : Rectangle<T> (bottom, top) {}
  inline Rec (const Point<T, 2>&& bottom, const Point<T, 2>&& top) : Rectangle<T> (std::move (bottom), std::move (top)) {}

  inline constexpr void dec_deleted () { --deleted; }
  inline constexpr bool get_deleted () const { return deleted; }
  inline constexpr void inc_deleted () { ++deleted; }
  inline constexpr int get_index () const { return index; }
  inline constexpr void set_index (int index) { this->index = index; }
};

template<typename T, typename Ti, int axis> bool is_good (std::array<std::vector<Rec<T, Ti>>, 2>& dims)
{
  Ti has = 0;
  int axis2 = 0;

  if constexpr (axis == 0) axis2 = 1; else axis2 = 0;

  for (Ti i = 0; i < dims [axis].size (); ++i)
    {
      if (dims [axis] [i].get_deleted () == 0) ++has;
    }

  if (has < 2)

    return true;
  else
    {
      Ti first = 0, last, second;

      for (Ti i = 0; i < dims [axis].size (); ++i) if (dims [axis] [i].get_deleted () == 0)
        last = i;

      for (Ti i = 0; i < dims [axis].size (); ++i) if (dims [axis] [i].get_deleted () == 0)
        {
          first = i;
          break;
        }

      for (T leftmost = 0, rightmost = 0; first != last; first = second)
        {

          for (Ti i = 1 + first; i < 1 + last; ++i) if (dims [axis] [i].get_deleted () == 0)
            {
              second = i;
              break;
            }

          leftmost = std::max (leftmost, dims [axis] [first].get_top () [axis]);
          rightmost = dims [axis] [second].get_bottom () [axis];

          if (rightmost >= leftmost)
            {
              for (Ti j = 0; j < second; ++j)
                dims [axis] [j].inc_deleted ();
              for (Ti j = 0; j < dims [axis2].size (); ++j) if (dims [axis2] [j].get_index () < second)
                dims [axis2] [j].inc_deleted ();

              bool rightgood = is_good<T, Ti, 0> (dims);

              for (Ti j = 0; j < second; ++j)
                dims [axis] [j].dec_deleted ();
              for (Ti j = 0; j < dims [axis2].size (); ++j) if (dims [axis2] [j].get_index () < second)
                dims [axis2] [j].dec_deleted ();

              if (! rightgood)

                return rightgood;
              else
                {
                  for (Ti j = second; j < dims [axis].size (); ++j)
                    dims [axis] [j].inc_deleted ();
                  for (Ti j = 0; j < dims [axis2].size (); ++j) if (dims [axis2] [j].get_index () >= second)
                    dims [axis2] [j].inc_deleted ();

                  bool leftgood = is_good<T, Ti, 0> (dims);

                  for (Ti j = second; j < dims [axis].size (); ++j)
                    dims [axis] [j].dec_deleted ();
                  for (Ti j = 0; j < dims [axis2].size (); ++j) if (dims [axis2] [j].get_index () >= second)
                    dims [axis2] [j].dec_deleted ();

                  return leftgood;
                }
            }
        }

      if constexpr (axis > 0)

        return false;
      else
        return is_good<T, Ti, 1> (dims);
    }
}

template<typename T, typename Ti> int program ()
{
  Ti nrecs;
  T x1, y1, x2, y2;

  std::vector<Rec<T, Ti>> sortedx;
  std::vector<Rec<T, Ti>> sortedy;

  scanf ("%i", &nrecs);
  sortedx.reserve (nrecs);
  sortedy.reserve (nrecs);

  for (Ti i = 0; i < nrecs; ++i)
    {
      scanf ("%i %i %i %i", &x1, &y1, &x2, &y2);
      sortedx.push_back (Rec<T, Ti> (Point<T, 2> (x1, y1), Point<T, 2> (x2, y2)));
      sortedy.push_back (Rec<T, Ti> (Point<T, 2> (x1, y1), Point<T, 2> (x2, y2)));
    }

  /* sort sortedx */

  std::sort (sortedx.begin (), sortedx.end (), [] (const Rec<T, Ti>& a, const Rec<T, Ti>& b)
    {
      return a.get_bottom () [1] < b.get_bottom () [1];
    });

  for (Ti i = 0; i < nrecs; ++i) sortedx [i].set_index (i);

  std::sort (sortedx.begin (), sortedx.end (), [] (const Rec<T, Ti>& a, const Rec<T, Ti>& b)
    {
      return a.get_bottom () [0] < b.get_bottom () [0];
    });

  /* sort sortedy */

  std::sort (sortedy.begin (), sortedy.end (), [] (const Rec<T, Ti>& a, const Rec<T, Ti>& b)
    {
      return a.get_bottom () [0] < b.get_bottom () [0];
    });

  for (Ti i = 0; i < nrecs; ++i) sortedy [i].set_index (i);

  std::sort (sortedy.begin (), sortedy.end (), [] (const Rec<T, Ti>& a, const Rec<T, Ti>& b)
    {
      return a.get_bottom () [1] < b.get_bottom () [1];
    });

  std::array<std::vector<Rec<T, Ti>>, 2> ar = { sortedx, sortedy };

  /* answer */

  printf ("%s\n", ! is_good<T, Ti, 0> (ar) ? "NO" : "YES");
  return 0;
}

int main ()
{
  return program<int, int> ();
}
