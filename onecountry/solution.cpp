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
#include <variant>
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
  static const int X0 = 0;
  static const int X1 = 2;
  static const int Y0 = 1;
  static const int Y1 = 3;

  template<typename... _Args>
  inline Rectangle (_Args&... __args) : Point<T, 4> ({ std::forward<_Args> (__args)... }) { }
  template<typename... _Args>
  inline Rectangle (_Args&&... __args) : Point<T, 4> ({ std::forward<_Args> (__args)... }) { }

  inline constexpr const T& get_x0 () const noexcept { return (*this) [X0]; }
  inline constexpr const T& get_x1 () const noexcept { return (*this) [X1]; }
  inline constexpr const T& get_y0 () const noexcept { return (*this) [Y0]; }
  inline constexpr const T& get_y1 () const noexcept { return (*this) [Y1]; }
};

template<typename T> class Deletable
{
private:
  bool deleted = false;
  const Rectangle<T>* value = nullptr;
public:

  inline Deletable (const Rectangle<T>* v) : value (v) { }

  inline constexpr const bool get_deleted () const { return deleted; }
  inline constexpr const Rectangle<T>* get_value () const { return value; }
  inline constexpr const T& get_x0 () const noexcept { return value->get_x0 (); }
  inline constexpr const T& get_x1 () const noexcept { return value->get_x1 (); }
  inline constexpr const T& get_y0 () const noexcept { return value->get_y0 (); }
  inline constexpr const T& get_y1 () const noexcept { return value->get_y1 (); }
  inline constexpr void set_deleted (bool value) { deleted = value; }
};

class MaybeNot
{
public:
  inline MaybeNot () { }
};

template<typename T> class Maybe
{
private:
  std::variant<T, MaybeNot> holder;
public:
  inline Maybe () : holder (MaybeNot ()) { }
  inline Maybe (const T& value) : holder (value) { }
  inline Maybe (const T&& value) : holder (std::move (value)) { }
  inline Maybe (T&& value) : holder (std::move (value)) { }

  inline constexpr const T& operator* () const
    {
      if (is_none ())
        std::__throw_runtime_error ("Maybe holds nothing");
      return std::get<T> (holder);
    }

  inline bool is_none () const
    {
      return std::holds_alternative<MaybeNot> (holder);
    }
};

namespace std
{
  template<typename T, int I>
  static inline constexpr const T& get (const Rectangle<T>& r) noexcept
    {
      if constexpr (I == Rectangle<T>::X0) return r.get_x0 ();
      else if constexpr (I == Rectangle<T>::X1) return r.get_x1 ();
      else if constexpr (I == Rectangle<T>::Y0) return r.get_y0 ();
      else if constexpr (I == Rectangle<T>::Y1) return r.get_y1 ();
    }

  template<typename T, int I>
  static inline constexpr const T& get (const Rectangle<T>* r) noexcept
    {
      return get<T, I> (*r);
    }

  template<typename T, int I>
  static inline constexpr const T& get (Rectangle<T>* r) noexcept
    {
      return get<T, I> (*r);
    }

  template<typename T, int I>
  static inline constexpr const T& get (const Deletable<T>* r) noexcept
    {
      return get<T, I> (r->get_value ());
    }

  template<typename T, int I>
  static inline constexpr const T& get (Deletable<T>* r) noexcept
    {
      return get<T, I> (r->get_value ());
    }

  template<typename T>
  static inline constexpr Maybe<T> make_maybe (const T& value) { return Maybe<T> (value); }
  template<typename T>
  static inline constexpr Maybe<T> make_maybe (const T&& value) { return Maybe<T> (std::move (value)); }
  template<typename T>
  static inline constexpr Maybe<T> make_maybe (T&& value) { return Maybe<T> (std::move (value)); }
}

template<typename T, typename N> class SegmentTree
{
public:
  using size_type = std::vector<T>::size_type;
protected:
  static inline size_type left (size_type node) { return 2 * node + 1; }
  static inline size_type mid (size_type l, size_type r) { return l + (r - l) / 2; }
  static inline size_type right (size_type node) { return 2 * node + 2; }

  size_type size;
  std::vector<T> store;
  std::vector<N> tree;

  inline const N* build (size_type node, size_type start, size_type end)
    {
      if (start == end)
        {
          auto n = N (store [start]);

          return (tree [node] = n, & tree [node]);
        }
      else
        {
          auto m = mid (start, end);
          auto l = build (left (node), start, m);
          auto r = build (right (node), m + 1, end);

          return (tree [node] = N (*l, *r), & tree [node]);
        }
    }

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
          return N (l_, r_);
        }
    }

public:

  inline SegmentTree (const std::span<T>& vec) : size (vec.size ()), store (vec.begin (), vec.end ()), tree (4 * size)
    {
      build (0, 0, size - 1);
    }

  inline constexpr std::span<T> get_values () { return std::span (store); }

  inline const N query (size_type l, size_type r)
    {
      return query (0, 0, size - 1, l, r);
    }
};

template<typename T, typename Ti> bool solve (std::span<Rectangle<T>*>&& rects);

template<typename T, typename M, int I> class NodeMax
{
  M most = std::numeric_limits<M>::min ();
public:

  inline NodeMax () {}
  inline NodeMax (const NodeMax& o) : most (o.most) { }
  inline NodeMax (const T& v) : most (std::get<M, I> (v)) { }
  inline NodeMax (const NodeMax& l, const NodeMax& r) : most (std::max (l.most, r.most)) { }
  inline constexpr const M& get_most () const { return most; }
};

template<typename T, typename M, int I> class NodeMin
{
  M most = std::numeric_limits<M>::max ();
public:

  inline NodeMin () {}
  inline NodeMin (const NodeMin& o) : most (o.most) { }
  inline NodeMin (const T& v) : most (std::get<M, I> (v)) { }
  inline NodeMin (const NodeMin& l, const NodeMin& r) : most (std::min (l.most, r.most)) { }
  inline constexpr const M& get_most () const { return most; }
};

template<typename T, typename Ti> inline int program ()
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

  std::cout << (! solve (std::span (rectv)) ? "NO" : "YES") << std::endl;

  for (Ti i = 0; i < nrects; ++i)
    {
      (&rects [i])->~Rectangle<T> ();
    }

  return (free (rects), 0);
}

template<typename T, typename ST = typename std::span<Deletable<T>*>::size_type> inline Maybe<ST> find_next (const std::span<Deletable<T>*>& rects, ST skip)
{
  for (ST i = skip + 1; i < rects.size (); ++i) if (rects [i]->get_deleted () == false)
    return std::make_maybe<ST> (i);
    return Maybe<ST> ();
}

template<typename T, typename ST = typename std::span<Deletable<T>*>::size_type> inline Maybe<ST> find_first (const std::span<Deletable<T>*>& rects)
{
  return rects [0]->get_deleted () == false ? std::make_maybe<ST> (0) : find_next<T, ST> (rects, 0);
}

template<typename T> inline bool solve (std::span<Deletable<T>*>&& rects)
{
  const int X0 = Rectangle<T>::X0;
  const int X1 = Rectangle<T>::X1;
  const int Y0 = Rectangle<T>::Y0;
  const int Y1 = Rectangle<T>::Y1;

  using size_type = std::span<Rectangle<T>*>::size_type;

  std::cerr << "solve (length " << rects.size () << ")" << std::endl;

  if (rects.size () < 2)
    {
      for (auto& r : rects) r->set_deleted (true);
      return true;
    }
  else
    {
      std::sort (rects.begin (), rects.end (), [](const Deletable<T>* a, const Deletable<T>* b)
        { return a->get_x0 () < b->get_x0 (); });
        auto x0tree = SegmentTree<Deletable<T> *, NodeMax<Deletable<T> *, T, Rectangle<T>::X1>> (rects);
      std::sort (rects.begin (), rects.end (), [](const Deletable<T>* a, const Deletable<T>* b)
        { return a->get_y0 () < b->get_y0 (); });
        auto y0tree = SegmentTree<Deletable<T> *, NodeMax<Deletable<T> *, T, Rectangle<T>::Y1>> (rects);
      std::sort (rects.begin (), rects.end (), [](const Deletable<T>* a, const Deletable<T>* b)
        { return a->get_x1 () > b->get_x1 (); });
        auto x1tree = SegmentTree<Deletable<T> *, NodeMin<Deletable<T> *, T, Rectangle<T>::X0>> (rects);
      std::sort (rects.begin (), rects.end (), [](const Deletable<T>* a, const Deletable<T>* b)
        { return a->get_y1 () > b->get_y1 (); });
        auto y1tree = SegmentTree<Deletable<T> *, NodeMin<Deletable<T> *, T, Rectangle<T>::Y0>> (rects);

      size_type cuts = 1;
      auto firstxl = std::make_maybe<size_type> (0);
      auto firstyl = std::make_maybe<size_type> (0);
      auto firstxr = std::make_maybe<size_type> (0);
      auto firstyr = std::make_maybe<size_type> (0);
      Maybe<size_type> second;

      while (cuts > 0)
        {
          cuts = 0;

          while (true)
            {
              if ((second = find_next (rects, *firstxl)).is_none ())
                break;
              if (std::get<T, X0> (x0tree.get_values () [*second]) >= x0tree.query (0, *firstxl).get_most ())
                { std::cerr << "cut on left x " << *firstxl << "|" << *second << std::endl;
                  second = *firstxl; }

              if ((second = find_next (rects, *firstyl)).is_none ())
                break;
              if (std::get<T, Y0> (x0tree.get_values () [*second]) >= y0tree.query (0, *firstyl).get_most ())
                { std::cerr << "cut on left y " << *firstyl << "|" << *second << std::endl;
                  second = *firstyl; }

              if ((second = find_next (rects, *firstxr)).is_none ())
                break;
              if (std::get<T, X1> (x0tree.get_values () [*second]) <= x1tree.query (0, *firstxr).get_most ())
                { std::cerr << "cut on right x " << *firstxr << "|" << *second << std::endl;
                  second = *firstxr; }

              if ((second = find_next (rects, *firstyr)).is_none ())
                break;
              if (std::get<T, Y1> (x0tree.get_values () [*second]) <= y1tree.query (0, *firstyr).get_most ())
                { std::cerr << "cut on right y " << *firstyr << "|" << *second << std::endl;
                  second = *firstyr; }
            }
        }

      return false;
    }
}

template<typename T> inline bool solve (std::span<Rectangle<T>*>&& rects)
{
  std::vector<Deletable<T> *> down;

  down.reserve (rects.size ());

  for (const auto& r : rects) down.push_back (new Deletable<T> (r));
  return solve<T> (std::span (down));
}

int main ()
{
  return program<unsigned long, int> ();
}
