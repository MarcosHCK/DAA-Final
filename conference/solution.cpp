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
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

template<typename TClass> class BNode
{
private:

  std::set<TClass> segments;

public:

  inline BNode () : segments ()
    {
    }

  inline BNode (const BNode<TClass>& o) : segments (o.segments)
    {
    }

  inline BNode (TClass&& val, BNode<TClass>&& o) : segments (std::move (o.segments))
    {
      segments.insert (std::move (val));
    }

  inline BNode (const BNode<TClass>& a, const BNode<TClass>& b)
    {
      auto r = & segments;
      auto x = & a.segments;
      auto y = & b.segments;

      std::set_union (x->begin (), x->end (), y->begin (), y->end (), std::inserter (*r, r->end ()));
    }

  inline constexpr const std::set<TClass>& lookup () const
    {
      return segments;
    }
};

template<typename TInterval, typename TNode, typename TVal> class BTree
{
private:

  TInterval end, start;
  std::unique_ptr<BTree<TInterval, TNode, TVal>> left = nullptr;
  std::unique_ptr<BTree<TInterval, TNode, TVal>> right = nullptr;
  TNode value;

  inline BTree* get_left ()
    {
      if (left == nullptr) split ();
      return left.get ();
    }

  inline BTree* get_right ()
    {
      if (right == nullptr) split ();
      return right.get ();
    }

  inline BTree<TInterval, TNode, TVal>* inserti (const TInterval& l, const TInterval& r, TVal&& val)
    {
      if (l < start || r > end || l > end)

        std::__throw_out_of_range ("inserting segment out of range");

      if (l == start && r == end && (left == nullptr && right == nullptr))

        return (value = TNode (std::forward<TVal> (val), std::move (value)), this);
      else
        {
          auto m = mid (start, end);
          BTree<TInterval, TNode, TVal>* lt;
          BTree<TInterval, TNode, TVal>* rt;

          if (l > m)
            {
              rt = get_right ()->inserti (l, r, std::forward<TVal> (val));
              lt = left.get ();
            }
          else if (m >= r)
            {
              lt = get_left ()->inserti (l, r, std::forward<TVal> (val));
              rt = right.get ();
            }
          else
            {
              lt = get_left ()->inserti (l, m, std::forward<TVal> (val));
              rt = get_right ()->inserti (m + 1, r, std::forward<TVal> (val));
            }

          return (value = TNode (lt->value, rt->value), this);
        }
    }

  inline const TNode lookupi (TInterval l, TInterval r) const
    {
      if (l == start && r == end && (left == nullptr && right == nullptr))

        return value;
      else
        {
          auto m = mid (start, end);

          if (l > m)
            {
              return right->lookupi (l, r);
            }
          else if (m >= r)
            {
              return left->lookupi (l, r);
            }
          else
            {
              auto lt = left->lookupi (l, m);
              auto rt = right->lookupi (m + 1, r);
              return TNode (lt, rt);
            }
        }
    }

  static inline TInterval mid (TInterval l, TInterval r)
    {
      return l + (r - l) / 2;
    }

  inline void split ()
    {
      auto m = mid (start, end);
      (left = std::make_unique<BTree<TInterval, TNode, TVal>> (start, m))->value = value;
      (right = std::make_unique<BTree<TInterval, TNode, TVal>> (m + 1, end))->value = value;
      value = TNode (left->value, right->value);
    }

public:

  inline BTree (const TInterval& l, const TInterval& r) : end (r), start (l)
    {
      if (l < 0 || l > r)

        std::__throw_invalid_argument ("invalid BTree interval");
    }

  inline constexpr const auto get_left () const { return left.get (); }
  inline constexpr const auto get_right () const { return right.get (); }
  inline constexpr const TNode& get_value () const { return value; }

  inline void insert (const TInterval& l, const TInterval& r, TVal&& val)
    {
      inserti (l, r, std::move (val));
    }

  inline const auto lookup (TInterval l, TInterval r) const
    {
      return lookupi (l, r).lookup ();
    }
};

template<typename C, typename P> inline P case_ ()
{
  P npeople;
  std::cin >> npeople;

  C country;
  P end, maxl = 0, start;

  std::vector<std::tuple<C, P, P>> vec;
  vec.reserve (npeople);

  for (P i = 0; i < npeople; ++i)
    {
      std::cin >> start >> end >> country;

      maxl = std::max (maxl, end);
      vec.push_back (std::make_tuple (country, end, start));
    }

  auto copp = std::vector<C> ();
  auto tree = BTree<P, BNode<P>, P> (0, maxl);

  std::sort (vec.begin (), vec.end (), [](const std::tuple<C, P, P>& a, const std::tuple<C, P, P>& b)
    {
      return std::get<2> (a) < std::get<2> (b);
    });

  copp.reserve (npeople);

  P i = 0;

  for (auto iter = vec.begin (); iter != vec.end (); ++iter)
    {
      auto c = std::get<0> (*iter);
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);

      copp.push_back (c);
      tree.insert (s, e, i++);
    }

  P upset = 0;
  auto u = false;
  auto r = false;

  do

    for (auto iter = vec.begin (); iter != vec.end (); ++iter)
      {
        auto c = std::get<0> (*iter);
        auto e = std::get<1> (*iter);
        auto s = std::get<2> (*iter);

        if (c > 0) for (const auto& s : tree.lookup (s, e))
          {
            auto k = copp [s];
            if (k == 0) { u = true; copp [s] = c; };
          }
      }
  while ((r = u, u = false, r) == true);

  i = 0;

  for (auto iter = vec.begin (); iter != vec.end (); ++iter)
    {
      auto c = std::get<0> (*iter);
      auto e = std::get<1> (*iter);
      auto s = std::get<2> (*iter);
      auto f = false;

      c = c > 0 ? c : copp [i];

      for (const auto& s : tree.lookup (s, e))
        {
          auto k = copp [s];
          if (c != k) { f = true; break; }
        }

      if (f == false) ++upset;
      ++i;
    }

  return upset;
}

template<typename C, typename P> inline int program ()
{
  P ncases;
  std::cin >> ncases;

  for (P i = 0; i < ncases; ++i) std::cout << case_<C, P> () << std::endl;
  return 0;
}

int main ()
{
  return program<unsigned, unsigned> ();
}
