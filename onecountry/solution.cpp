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
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <span>
#include <tuple>
#include <vector>

template<typename T, int dims> class Point : public std::array<T, dims>
{
public:
  template<typename... _Args>
  inline Point (_Args&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
  template<typename... _Args>
  inline Point (_Args&&... __args) : std::array<T, dims> ({ std::forward<_Args> (__args)... }) { }
};

template<typename T> class Rectangle : public Point<T, 4>
{
public:
  template<typename... _Args>
  inline Rectangle (_Args&... __args) : Point<T, 4> ({ std::forward<_Args> (__args)... }) { }
  template<typename... _Args>
  inline Rectangle (_Args&&... __args) : Point<T, 4> ({ std::forward<_Args> (__args)... }) { }

  inline constexpr const T& get_x0 () const noexcept { return (*this) [0]; }
  inline constexpr const T& get_x1 () const noexcept { return (*this) [2]; }
  inline constexpr const T& get_y0 () const noexcept { return (*this) [1]; }
  inline constexpr const T& get_y1 () const noexcept { return (*this) [3]; }
};

namespace std
{
  template<typename T, int axis>
  inline const std::array<T, 2> get (const Rectangle<T>& r) noexcept
    {
      if constexpr (axis == 0)

        return { r.get_x0 (), r.get_x1 () };
      else
        return { r.get_y0 (), r.get_y1 () };
    }

  template<typename T, int axis>
  inline const std::array<T, 2> get (const Rectangle<T>* r) noexcept
    {
      return std::get<T, axis> (*r);
    }

  template<typename T, int axis>
  inline const std::array<T, 2> get (Rectangle<T>* r) noexcept
    {
      return std::get<T, axis> (*r);
    }
}

template<typename T, typename N, int axis, typename M = T::value_type> class SegmentTree
{
public:
  static const int A0_MOST = 0;
  static const int A1_MOST = 1;
  using size_type = std::vector<T>::size_type;
  static inline size_type left (size_type node) { return 2 * node + 1; }
  static inline size_type mid (size_type l, size_type r) { return l + (r - l) / 2; }
  static inline size_type right (size_type node) { return 2 * node + 2; }

  size_type size;
  std::map<T, N*> stage;
  std::vector<T> store;
  std::vector<N> tree;

  inline const N* build (size_type node, size_type start, size_type end)
    {
      if (start == end)
        {
          stage.insert (std::make_pair (store [start], & tree [node]));
          return (tree [node] = N (store [start]), & tree [node]);
        }
      else
        {
          auto m = mid (start, end);
          auto l = build (left (node), start, m);
          auto r = build (right (node), m + 1, end);

          return (tree [node] = N (l, r), & tree [node]);
        }
    }

public:

  inline const N query (size_type node, size_type start, size_type end, size_type l, size_type r)
    {
      if (r < start || end < l)

        return N ();

      if (l <= start && end <= r)

        return tree [node];
      else
        {
          auto m = mid (start, end);
          auto l_ = query (left (node), start, m, l, r);
          auto r_ = query (right (node), m + 1, end, l, r);
          return N (&l_, &r_);
        }
    }

  inline const N* update (size_type node, size_type start, size_type end, N* hint = nullptr)
    {
      if (start == end)
        {
          return & tree [node];
        }
      else if (hint == nullptr)
        {
          auto m = mid (start, end);
          auto l = update (left (node), start, m, hint);
          auto r = update (right (node), m + 1, end, hint);

          return (tree [node] = N (l, r), & tree [node]);
        }
      else
        {
          size_type m;

          if (& tree [m = mid (start, end)] > hint)
            {
              auto l = update (left (node), start, m, hint);
              auto r = & tree [right (node)];
              return (tree [node] = N (l, r), & tree [node]);
            }
          else
            {
              auto l = & tree [left (node)];
              auto r = update (right (node), m + 1, end, hint);
              return (tree [node] = N (l, r), & tree [node]);
            }
        }
    }

public:

  inline SegmentTree (const std::vector<T>&& vec) : size (vec.size ()), store (std::move (vec)), tree (4 * vec.size ())
    {
      build (0, 0, size - 1);
    }

  inline constexpr N* operator[] (const T& v)
    {
      typename std::map<T, N*>::iterator at;

      if ((at = stage.find (v)) != stage.end ())

        return std::get<1> (*at);
      else
        std::__throw_out_of_range ("unexisting value");
    }

  inline constexpr std::span<T> get_values () { return std::span (store); }
};

template<typename T, int axis, typename M> class Node
{
  static const int A0_MOST = SegmentTree<T, Node<T, axis, M>, axis, M>::A0_MOST;
  static const int A1_MOST = SegmentTree<T, Node<T, axis, M>, axis, M>::A1_MOST;
public:

  M a0most = std::numeric_limits<M>::max ();
  M a1most = std::numeric_limits<M>::min ();

  inline Node () { }

  inline Node (const Node& o) : a0most (o.a0most), a1most (o.a1most)
    {
    }

  inline Node (const T& v)
    {
      a0most = std::get<M, axis> (v) [A0_MOST];
      a1most = std::get<M, axis> (v) [A1_MOST];
    }

  inline Node (const Node* left, const Node* right)
        : a0most (std::min (left->a0most, right->a0most))
        , a1most (std::max (left->a1most, right->a1most))
    {
    }
};

template<typename T, int axis, typename M = T::value_type> class SegmentTreeA : public SegmentTree<T, Node<T, axis, M>, axis, M>
{
private:
  using size_type = SegmentTree<T, Node<T, axis, M>, axis, M>::size_type;
public:

  inline std::array<M, 2> query_mosts (size_type l, size_type r)
    {
      auto n = this->query (0, 0, this->size - 1, l, r);
      return { n.a0most, n.a1most };
    }
};

int depth = 0;

static std::ostream& derout ()
{
  return std::cerr << std::string (depth * 2, '-');
}

template<typename T, typename Ti> class Problem
{
  static const int X_AXIS = 0;
  static const int Y_AXIS = 1;
  static const int A0_MOST = SegmentTreeA<Rectangle<T>*, X_AXIS, T>::A0_MOST;
  static const int A1_MOST = SegmentTreeA<Rectangle<T>*, X_AXIS, T>::A1_MOST;
public:

  class Range : public std::tuple<Ti, Ti>
    {
    public:
      inline Range (const Ti& l, const Ti& r) : std::tuple<Ti, Ti> (l, r) { }
      inline Range (const Ti&& l, const Ti&& r) : std::tuple<Ti, Ti> (l, r) { }
      inline constexpr const Ti& l () const { return std::get<0> (*this); }
      inline constexpr const Ti& r () const { return std::get<1> (*this); }
    };

  static inline bool solve (std::span<Rectangle<T>*> rects)
    {
      //if (depth > 11) std::__throw_overflow_error ("overflow");
      Ti size;
      bool b;

      derout () << "solve size: " << rects.size () << std::endl;

      if ((size = rects.size ()) < 2)

        return true;
      else
        return (depth++, b = solve (rects, size), depth--, b);
    }

private:

  template<int axis>
  using STree = SegmentTreeA<Rectangle<T> *, axis, T>;

  template<int axis>
  static inline STree<axis> prepare (std::span<Rectangle<T>*> rects)
    {
      auto vec = std::vector<Rectangle<T> *> (rects.begin (), rects.end ());

      std::sort (vec.begin (), vec.end (), [](const Rectangle<T>* a, const Rectangle<T>* b)
        {
          return std::get<T, axis> (a) [A0_MOST] < std::get<T, axis> (b) [A0_MOST];
        });

      return SegmentTreeA<Rectangle<T> *, axis, T> (std::move (vec));
    }

  static inline bool solve (std::span<Rectangle<T>*> rects, const Ti& size)
    {
      auto xtree = prepare<X_AXIS> (rects);
      auto ytree = prepare<Y_AXIS> (rects);

      for (Ti i = size / 2, j = size / 2; i > 0 || j < size - 1;)
        {
          if (i > 0)
            {
              if (std::get<T, X_AXIS> (xtree.get_values () [i]) [A0_MOST] >= xtree.query_mosts (0, i - 1) [A1_MOST])
                {
                  return
                      solve (xtree.get_values ().subspan (0, i))
                   && solve (xtree.get_values ().subspan (i, size - i));
                }
              else
              if (std::get<T, Y_AXIS> (ytree.get_values () [i]) [A0_MOST] >= ytree.query_mosts (0, i - 1) [A1_MOST])
                {
                  return
                      solve (ytree.get_values ().subspan (0, i))
                   && solve (ytree.get_values ().subspan (i, size - i));
                }
              --i;
            }

          if (j < size - 1)
            {
              if (std::get<T, X_AXIS> (xtree.get_values () [j + 1]) [A0_MOST] >= xtree.query_mosts (0, j) [A1_MOST])
                {
                  return
                      solve (xtree.get_values ().subspan (0, j))
                   && solve (xtree.get_values ().subspan (j, size - (j + 1)));
                }
              else
              if (std::get<T, Y_AXIS> (ytree.get_values () [j + 1]) [A0_MOST] >= ytree.query_mosts (0, j) [A1_MOST])
                {
                  return
                      solve (ytree.get_values ().subspan (0, j))
                   && solve (ytree.get_values ().subspan (j, size - (j + 1)));
                }
              ++j;
            }
        }

      return false;
    }
};

template<typename T, typename Ti> int program ()
{
  Ti nrects;
  Rectangle<T>* rects;
  std::vector<Rectangle<T>*> rectv;
  T x0, y0, x1, y1;

  std::cin >> nrects;
  rects = (Rectangle<T>*) malloc (nrects * sizeof (Rectangle<T>));
  rectv = std::vector<Rectangle<T>*> ();
  rectv.reserve (nrects);

  for (Ti i = 0; i < nrects; ++i)
    {
      std::cin >> x0 >> y0 >> x1 >> y1;
      rectv.push_back (new (& rects [i]) Rectangle<T> (x0, y0, x1, y1));
    }

  std::cout << (! Problem<T, Ti> ().solve (rectv) ? "NO" : "YES") << std::endl;
  return 0;
}

int main ()
{
  return program<unsigned long, int> ();
}
