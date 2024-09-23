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

  std::map<T, size_type> fastidx;
  size_type size;
  std::vector<T> store;
  std::vector<N> tree;

  inline const N* build (size_type node, size_type start, size_type end)
    {
      if (start == end)
        {
          auto n = N (store [start]);
          fastidx.insert (std::make_pair (store [start], start));
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

  inline const N* update (size_type node, size_type start, size_type end, size_type l, size_type r)
    {
      if (start == end)
        {
          auto n = N (store [start]);
          return (tree [node] = n, & tree [node]);
        }
      else
        {
          auto m = mid (start, end);

          if (l > m)
            {
              auto l_ = & tree [left (node)];
              auto r_ = update (right (node), m + 1, end, l, r);
              return (tree [node] = N (*l_, *r_), & tree [node]);
            }
          else if (m >= r)
            {
              auto l_ = update (left (node), start, m, l, r);
              auto r_ = & tree [right (node)];
              return (tree [node] = N (*l_, *r_), & tree [node]);
            }
          else
            {
              auto l_ = update (left (node), start, m, l, r);
              auto r_ = update (right (node), m + 1, end, l, r);
              return (tree [node] = N (*l_, *r_), & tree [node]);
            }
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

  inline void update ()
    {
      update (0, 0, size - 1, 0, size - 1);
    }

  inline void update (const T& hint)
    {
      auto idx = fastidx [hint];
      update (0, 0, size - 1, idx, idx);
    }

  inline void update (size_type l, size_type r)
    {
      update (0, 0, size - 1, l, r);
    }
};

template<typename T, std::array<T, 0>::size_type size>
inline const T& array_min (const std::array<T, size>& ar);
template<typename T, std::array<T, 0>::size_type size, int ... Is>
inline const T& array_min_helper (const std::array<T, size>& ar, std::integer_sequence<int, Is ...> const&);
template<typename T, typename size_type = typename std::span<Deletable<T> *>::size_type>
inline bool collect (std::vector<Deletable<T> *>&& vec, std::array<size_type, 4>&& lasts, int depth);
template<typename T, typename Ti>
bool solve (std::span<Rectangle<T>*>&& rects);
template<typename T>
inline bool solve (std::vector<Deletable<T>*>&& rects, int depth);
template<typename T, typename size_type = typename std::span<Deletable<T> *>::size_type>
inline std::vector<Deletable<T> *> split (const std::span<Deletable<T> *>& vec, size_type last, size_type second);

template<typename T, typename M, int I> class NodeMax
{
  M most = std::numeric_limits<M>::min ();
public:

  inline NodeMax () {}
  inline NodeMax (const NodeMax& o) : most (o.most) { }
  inline NodeMax (const T& v) : most (v->get_deleted () == false ? std::get<M, I> (v) : std::numeric_limits<M>::min ()) { }
  inline NodeMax (const NodeMax& l, const NodeMax& r) : most (std::max (l.most, r.most)) { }
  inline constexpr const M& get_most () const { return most; }
};

template<typename T, typename M, int I> class NodeMin
{
  M most = std::numeric_limits<M>::max ();
public:

  inline NodeMin () {}
  inline NodeMin (const NodeMin& o) : most (o.most) { }
  inline NodeMin (const T& v) : most (v->get_deleted () == false ? std::get<M, I> (v) : std::numeric_limits<M>::max ()) { }
  inline NodeMin (const NodeMin& l, const NodeMin& r) : most (std::min (l.most, r.most)) { }
  inline constexpr const M& get_most () const { return most; }
};

template<typename T, std::array<T, 0>::size_type size> inline const T& array_min (const std::array<T, size>& ar)
{
  static_assert (size > 0);
  return array_min_helper (ar, std::make_integer_sequence<int, size> ());
}

template<typename T, std::array<T, 0>::size_type size, int ... Is> inline const T& array_min_helper (const std::array<T, size>& ar, std::integer_sequence<int, Is ...> const&)
{
  Maybe<T> value;
  using unused = int [];
  (void) unused { (value = (value.is_none () ? ar [Is] : (ar [Is] < *value ? ar [Is] : *value)), 0)... };
  return *value;
}

template<typename T, typename size_type> inline bool collect (std::vector<Deletable<T> *>&& vec, std::array<size_type, 4>&& lasts, int depth)
{
  std::vector<Deletable<T> *> descent;

  for (size_type i = array_min (lasts); i < vec.size (); ++i) if (vec [i]->get_deleted () == false)
    descent.push_back (std::move (vec [i]));

  if (descent.size () == vec.size () && vec.size () > 0)

    return false;
  else
    return solve (std::move (descent), 1 + depth);
}

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

template<typename T, typename ST = typename std::vector<Deletable<T>*>::size_type> inline Maybe<ST> find_hold (const std::span<Deletable<T>*>& rects, ST hold)
{
  for (ST i = hold; i < rects.size (); ++i) if (rects [i]->get_deleted () == false)
    return std::make_maybe<ST> (i);
    return Maybe<ST> ();
}

template<typename T, typename ST = typename std::vector<Deletable<T>*>::size_type> inline Maybe<ST> find_next (const std::span<Deletable<T>*>& rects, ST skip)
{
  for (ST i = skip + 1; i < rects.size (); ++i) if (rects [i]->get_deleted () == false)
    return std::make_maybe<ST> (i);
    return Maybe<ST> ();
}

template<typename T> inline bool solve (std::vector<Deletable<T> *>&& rects, int depth)
{
  using size_type = std::span<Rectangle<T>*>::size_type;

  if (rects.size () < 2)
    {
      for (auto& r : rects) r->set_deleted (true);
      return true;
    }
  else
    {
      size_type checks, cuts = 1;
      Maybe<size_type> firstxl = 0;
      Maybe<size_type> firstyl = 0;
      Maybe<size_type> firstxr = 0;
      Maybe<size_type> firstyr = 0;
      size_t lastxl = 0, lastyl = 0;
      size_t lastxr = 0, lastyr = 0;
      Maybe<size_type> second;

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

      while (cuts > 0)
        {
          cuts = 0;
        #define CHECKBLOCK(tree,first,last,axis,op,debugprefix,other1,other2,other3) \
          if (first.is_none () == false) \
          if ((first = find_hold (tree.get_values (), *first)).is_none () == false) \
            { \
          if ((second = find_next (tree.get_values (), *first)).is_none () == false) \
            { \
              ++checks; \
              if (std::get<T, axis> (tree.get_values () [*second]) op tree.query (last, *first).get_most ()) \
                first = *second; \
              else \
                { \
                  if (! solve (split (tree.get_values (), last, *second), 1 + depth)) \
                    return false; \
                  tree.update (last, *second); \
                  if (true) \
                    { \
                      auto ar = tree.get_values ().subspan (last, *second - last); \
                      for (const auto& r : ar) other1.update (r); \
                      for (const auto& r : ar) other2.update (r); \
                      for (const auto& r : ar) other3.update (r); \
                    } \
                  first = (last = *second); \
                  ++cuts; \
                } } }

          while (checks > 0)
            {
              checks = 0;
              CHECKBLOCK (x0tree, firstxl, lastxl, Rectangle<T>::X0, <, "left on x", y0tree, x1tree, y1tree)
              CHECKBLOCK (y0tree, firstyl, lastyl, Rectangle<T>::Y0, <, "left on y", x0tree, x1tree, y1tree)
              CHECKBLOCK (x1tree, firstxr, lastxr, Rectangle<T>::X1, >, "right on x", x0tree, y0tree, y1tree)
              CHECKBLOCK (y1tree, firstyr, lastyr, Rectangle<T>::Y1, >, "right on y", x0tree, y0tree, x1tree)
            }
        #undef CHECKBLOCK
        }

      return collect (std::move (rects), { lastxl, lastyl, lastxr, lastyr }, 1 + depth);
    }
}

template<typename T, typename size_type> inline std::vector<Deletable<T> *> split (const std::span<Deletable<T> *>& vec, size_type last, size_type second)
{
  std::vector<Deletable<T> *> descent;

  // Wild guess
  descent.reserve (second - last);

  for (size_type i = last; i < second; ++i) if (vec [i]->get_deleted () == false)
    descent.push_back (vec [i]);

  return std::move (descent);
}

template<typename T> inline bool solve (std::span<Rectangle<T> *>&& rects)
{
  std::vector<Deletable<T> *> descent;

  descent.reserve (rects.size ());
  for (const auto& r : rects) descent.push_back (new Deletable<T> (r));
  return solve<T> (std::move (descent), 0);
}

int main ()
{
  return program<unsigned long, int> ();
}
